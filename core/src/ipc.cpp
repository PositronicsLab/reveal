#include <Reveal/ipc.h>

#include <assert.h>
#include <sstream>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Core {

//-----------------------------------------------------------------------------
pipe_c::pipe_c( const unsigned& port, void* context ) {
  _binder = true;
  _created_context = false;

  _open = false;
  _port = port;
  _context = context;
  _socket = NULL;
  _id = "";
  _interthread = false;
}

//-----------------------------------------------------------------------------
pipe_c::pipe_c( const std::string& host, const unsigned& port, void* context ) {
  _binder = false;
  _created_context = false;

  _open = false;
  _host = host;
  _port = port;
  _context = context;
  _socket = NULL;
  _id = "";
  _interthread = false;
}

//-----------------------------------------------------------------------------
pipe_c::pipe_c( const std::string& id, bool bind, void* context ) {
  _binder = bind;
  _created_context = false;

  _open = false;
  _context = context;
  _socket = NULL;
  _id = id;
  _interthread = true;
}

//-----------------------------------------------------------------------------
pipe_c::~pipe_c( void ) {
  if( _open ) close();
}

//-----------------------------------------------------------------------------
// TODO: determine level of detail for return value, i.e. bool or enum
pipe_c::error_e pipe_c::open( void ) {
  // sanity checks
  assert( !_open );

  // If no context was provided, create one.
  if( _context == NULL ) {
    _context = zmq_ctx_new();
    if( _context == NULL ) {
      return ERROR_CONTEXT;
    }
    _created_context = true;
  }

  // pipe_c exclusively uses ZMQ_DEALER socket
  _socket = zmq_socket( _context, ZMQ_DEALER );
 
  // validate socket creation
  if( _socket == NULL ) {
    if( errno == EINVAL ) {
      // the requested socket type is invalid
      return ERROR_SOCKET;
    } else if( errno == EFAULT ) {
      // the provided context is invalid
      return ERROR_CONTEXT;
    } else if( errno == EMFILE ) {
      // the limit on the number of open sockets has been reached
      return ERROR_LIMIT;
    } else if( errno == ETERM ) {
      // the context specified was terminated
      return ERROR_CONTEXT;
    } else {
      // Note: above should trap specifics, but if fall through then return
      // a socket error
      return ERROR_SOCKET;
    }
  }

  if( _binder ) {
    if( zmq_bind( _socket, connection_string().c_str() ) == -1 ) {
      if( errno == EINVAL ) {
        // endpoint invalid
        return ERROR_ADDRESS;
      } else if( errno == EPROTONOSUPPORT ) {
        // transport protocol is not supported
        return ERROR_SOCKET;
      } else if( errno == ENOCOMPATPROTO ) {
        // transport protocol incompatible with socket
        return ERROR_SOCKET;
      } else if( errno == EADDRINUSE ) {
        // requested address already in use
        return ERROR_ADDRESS;
      } else if( errno == EADDRNOTAVAIL ) {
        // requested address was not local
        return ERROR_ADDRESS;
      } else if( errno == ENODEV ) {
        // requested address specifies a nonexistent interface
        return ERROR_ADDRESS;
      } else if( errno == ETERM ) {
        // zeromq context was terminated
        return ERROR_CONTEXT;
      } else if( errno == ENOTSOCK ) {
        // socket is invalid
        return ERROR_SOCKET;
      } else if( errno == EMTHREAD ) {
        // no I/O thread available
        return ERROR_CONTEXT;
      } else {
        // Note: above should trap specifics, but if fall through then return
        // a socket error  
        return ERROR_SOCKET;
      }
    }
  } else {
    if( zmq_connect( _socket, connection_string().c_str() ) == -1 ) {
      if( errno == EINVAL ) {
        // endpoint invalid
        return ERROR_ADDRESS;
      } else if( errno == EPROTONOSUPPORT ) {
        // transport protocol is not supported
        return ERROR_SOCKET;
      } else if( errno == ENOCOMPATPROTO ) {
        // transport protocol incompatible with socket
        return ERROR_SOCKET;
      } else if( errno == ETERM ) {
        // zeromq context was terminated
        return ERROR_CONTEXT;
      } else if( errno == ENOTSOCK ) {
        // socket is invalid
        return ERROR_SOCKET;
      } else if( errno == EMTHREAD ) {
        // no I/O thread available
        return ERROR_CONTEXT;
      } else {
        // Note: above should trap specifics, but if fall through then return
        // a socket error  
        return ERROR_SOCKET;
      }
    }
  }

  // otherwise success, socket is open
  _open = true;
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
// TODO: determine level of detail for return value, i.e. void, bool or enum or
// if the errors should cause this function to automatically try again.
pipe_c::error_e pipe_c::close( void ) {
  // sanity checks
  assert( _open );

  // close the socket
  if( zmq_close( _socket ) == -1 ) {
    if( errno == ENOTSOCK ) {
      // the provided socket was invalid
      return ERROR_SOCKET;
    }
  }

  // if created a context, destroy the context
  if( _created_context ) {
    if( zmq_ctx_destroy( _context ) == -1 ) {
      if( errno == EFAULT ) {
        // The provided context was invalid
        return ERROR_CONTEXT;
      } else if( errno == EINTR ) {
        // Termination was interrupted by a signal, can be restarted.
        return ERROR_INTERRUPT;
      } else {
        // Note: above should trap specifics, but if fall through then return
        // a socket error  
        return ERROR_SOCKET;
      }    
    }
  }

  _open = false;
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
// TODO: determine level of detail for return value, i.e. bool or enum
pipe_c::error_e pipe_c::read( std::string& msg ) {
  // sanity checks
  assert( _open );

  char buffer[ PIPE_RCV_BUFFER_SZ ];
  msg = "";

  int more;
  size_t sz = sizeof( int );
    
  do {
    int bytes = zmq_recv( _socket, buffer, PIPE_RCV_BUFFER_SZ, 0 );
    if( bytes == -1 ) {
      if( errno == EAGAIN ) {
        // non-blocking mode was requested and no messages available
        return ERROR_EMPTY;
      } else if( errno == ENOTSUP ) {
        // zmq_recv is not supported by this socket type
        return ERROR_MODE;
      } else if( errno == EFSM ) {
        // zmq_recv operation cannot be performed as socket not in correct state
        return ERROR_STATE;
      } else if( errno == ETERM ) {
        // zeromq context associated with socket was terminated
        return ERROR_CONTEXT;
      } else if( errno == ENOTSOCK ) {
        // The socket was invalid
        return ERROR_SOCKET;
      } else if( errno == EINTR ) {
        // The operation was interrupted by delivery of a signal
        return ERROR_INTERRUPT;
      } else {
        // Note: above should trap specifics, but if fall through then return
        // a socket error  
        return ERROR_SOCKET;
      }
    }
    if( zmq_getsockopt( _socket, ZMQ_RCVMORE, &more, &sz ) == -1 ) {
      if( errno == EINVAL ) {
        // The requested option is unknown, or option_len or option_value is
        // invalid, or the size of the buffer is insufficient
        return ERROR_OPTION;
      } else if( errno == ETERM ) {
        // zeromq context associated with socket was terminated
        return ERROR_CONTEXT;
      } else if( errno == ENOTSOCK ) {
        // The socket was invalid
        return ERROR_SOCKET;
      } else if( errno == EINTR ) {
        // The operation was interrupted by delivery of a signal
        return ERROR_INTERRUPT;
      } else {
        // Note: above should trap specifics, but if fall through then return
        // a socket error  
        return ERROR_SOCKET;
      }
    }
    if( more && bytes ) msg.append( buffer, bytes );
  } while( more );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
// TODO: determine level of detail for return value, i.e. bool or enum
pipe_c::error_e pipe_c::write( const std::string& msg ) {
  // sanity checks
  assert( _open );

  std::string chunk;        // a chunk of the message
  int bytes;

  // Compute the number of chunks
  unsigned chunks = msg.size() / PIPE_SND_BUFFER_SZ;    // compute whole chunks
  if( msg.size() % PIPE_SND_BUFFER_SZ ) chunks++;       // compute partial chunk

  for( unsigned i = 0; i < chunks; i++ ) {
    // Compute the size of the current chunk
    unsigned chunk_sz = ( (i+1) * PIPE_SND_BUFFER_SZ > msg.size() ) ? 
                        msg.size() - (chunks - 1) * PIPE_SND_BUFFER_SZ : 
                        PIPE_SND_BUFFER_SZ;

    // Get the chunk out of the message
    chunk = msg.substr( i * PIPE_SND_BUFFER_SZ, chunk_sz );
      
    // Transmit the chunk with the notification flag that more is to follow
    bytes = zmq_send( _socket, chunk.c_str(), chunk_sz, ZMQ_SNDMORE );
    if( bytes == -1 ) {
      // errno is set, can check it and adjust method to return enum error
      if( errno == EAGAIN ) {
        // non-blocking mode was requested and message cannot be sent now
        return ERROR_STATE;
      } else if( errno == ENOTSUP ) {
        // zmq_send is not supported by this socket type
        return ERROR_MODE;
      } else if( errno == EFSM ) {
        // zmq_send operation cannot be performed as socket not in correct state
        return ERROR_STATE;
      } else if( errno == ETERM ) {
        // zeromq context associated with socket was terminated
        return ERROR_CONTEXT;
      } else if( errno == ENOTSOCK ) {
        // The socket was invalid
        return ERROR_SOCKET;
      } else if( errno == EINTR ) {
        // The operation was interrupted by delivery of a signal
        return ERROR_INTERRUPT;
/*
      } else if( errno == ECANTROUTE ) {
        // Message cannot be routed to the destination as peer either dead or
        // disconnected
*/
      } else {
        // Note: above should trap specifics, but if fall through then return
        // a socket error  
        return ERROR_SOCKET;
      }
    }
  }
  // Transmit a tiny notification that the message has been entirely sent
  bytes = zmq_send( _socket, "0", 1, 0 );
  if( bytes == -1) {
    // errno is set, can check it and adjust method to return enum error
    if( errno == EAGAIN ) {
      // non-blocking mode was requested and message cannot be sent now
      return ERROR_STATE;
    } else if( errno == ENOTSUP ) {
      // zmq_send is not supported by this socket type
      return ERROR_MODE;
    } else if( errno == EFSM ) {
      // zmq_send operation cannot be performed as socket not in correct state
      return ERROR_STATE;
    } else if( errno == ETERM ) {
      // zeromq context associated with socket was terminated
      return ERROR_CONTEXT;
    } else if( errno == ENOTSOCK ) {
      // The socket was invalid
      return ERROR_SOCKET;
    } else if( errno == EINTR ) {
      // The operation was interrupted by delivery of a signal
      return ERROR_INTERRUPT;
/*
    } else if( errno == ECANTROUTE ) {
      // Message cannot be routed to the destination as peer either dead or
      // disconnected
*/
    } else {
      // Note: above should trap specifics, but if fall through then return
      // a socket error  
      return ERROR_SOCKET;
    }
  }
  
  // otherwise success
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
std::string pipe_c::connection_string( void ) {
  std::stringstream ss;

  
  if( _interthread ) {
    ss << "inproc://" << _id;
  } else {
    if( _binder ) {
      ss << "tcp://*:" << _port;
    } else {
      ss << "tcp://" << _host << ":" << _port;
    }
  }
 
  return ss.str();
}

//-----------------------------------------------------------------------------
void* pipe_c::context( void ) {
  return _context;
}

//-----------------------------------------------------------------------------
void* pipe_c::socket( void ) {
  return _socket;
}

//-----------------------------------------------------------------------------

}  // namespace Core

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------
