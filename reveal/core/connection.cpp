#include "reveal/core/connection.h"

#include <zmq.h>
#include <assert.h>
#include <sstream>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------
connection_c::connection_c( void ) {
  _role = UNDEFINED;
  _open = false;
  _context = NULL;
  _socket = NULL;
  _id = "";
  _owns_context = false;
}

//-----------------------------------------------------------------------------
connection_c::connection_c( const unsigned& port ) {
  _role = ROUTER;
  _open = false;
  _port = port;
  _context = NULL;
  _socket = NULL;
  _id = "";
  _owns_context = true;
}

//-----------------------------------------------------------------------------
connection_c::connection_c( const unsigned& port, void* context ) {
  _role = WORKER;
  _open = false;
  _port = port;
  _context = context;
  _socket = NULL;
  _id = "";
  _owns_context = false;
}

//-----------------------------------------------------------------------------
connection_c::connection_c( const connection_c::role_e& role, void* context ) {
  assert( role == connection_c::WORKER || role == connection_c::DEALER );
  _role = role;
  _open = false;
  _context = context;
  _socket = NULL;
  _id = "";
  _owns_context = false;
}

//-----------------------------------------------------------------------------
connection_c::connection_c( const std::string& host, const unsigned& port ) {
  _role = CLIENT;
  _open = false;
  _host = host;
  _port = port;
  _context = NULL;
  _socket = NULL;
  _id = "";
  _owns_context = true;
}

/*
//-----------------------------------------------------------------------------
connection_c::connection_c( const connection_c::role_e& role, void* context, std::string id ) {
  assert( role == connection_c::IPC_SERVER || role == connection_c::IPC_CLIENT );
  _role = role;
  _open = false;
  _context = context;
  _socket = NULL;
  _id = id;
}
*/
//-----------------------------------------------------------------------------
connection_c::connection_c( const connection_c::role_e& role, std::string id ) {
  assert( role == connection_c::IPC_SERVER || role == connection_c::IPC_CLIENT );
  _role = role;
  _open = false;
  _context = NULL;
  _socket = NULL;
  _id = id;
  _owns_context = true;
}
//-----------------------------------------------------------------------------
connection_c::~connection_c( void ) {
  if( _open ) close();
}

//-----------------------------------------------------------------------------
// TODO: determine level of detail for return value, i.e. bool or enum
connection_c::error_e connection_c::open( void ) {
  // sanity checks
  if( _open ) return ERROR_NONE;

  assert( _role != UNDEFINED );  

  //if( _role == CLIENT || _role == ROUTER ) {  
  //if( _role == CLIENT || _role == ROUTER || _role == IPC_SERVER || _role == _IPC_CLIENT ) {  
  if( _owns_context ) {  
    // if client or router, build a context
    // Note: for DEALER or WORKER, the context was supplied in construction
    _context = zmq_ctx_new();
    if( _context == NULL ) {
      return ERROR_CONTEXT;
      // Note: no error values are defined for zmq_ctx_new
    }
  }

  // open a socket
  if( _role == CLIENT ) {
    _socket = zmq_socket( _context, ZMQ_REQ );
  } else if( _role == ROUTER ) {
    _socket = zmq_socket( _context, ZMQ_ROUTER );
  } else if( _role == DEALER ) {
    _socket = zmq_socket( _context, ZMQ_DEALER );
  } else if( _role == WORKER ) {
    _socket = zmq_socket( _context, ZMQ_REP );
///*
  } else if( _role == IPC_SERVER ) {
    _socket = zmq_socket( _context, ZMQ_PAIR );
  } else if( _role == IPC_CLIENT ) {
    _socket = zmq_socket( _context, ZMQ_PAIR );
//*/
  } else {
    // Note: sanity checks should guarantee never getting here.
    return ERROR_SOCKET;
  }

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

  if( _role == CLIENT || _role == WORKER || _role == IPC_CLIENT ) {  
//  if( _role == CLIENT || _role == WORKER ) {  
    // if client or worker, connect to the socket
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
  } else if( _role == ROUTER || _role == DEALER || _role == IPC_SERVER ) {  
//  } else if( _role == ROUTER || _role == DEALER ) {  
    // if router or dealer, bind to the socket
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
  }

  // otherwise success, socket is open
  _open = true;
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
// TODO: determine level of detail for return value, i.e. void, bool or enum or
// if the errors should cause this function to automatically try again.
connection_c::error_e connection_c::close( void ) {
  // sanity checks
  assert( _open );

  // close the socket
  if( zmq_close( _socket ) == -1 ) {
    if( errno == ENOTSOCK ) {
      // the provided socket was invalid
      return ERROR_SOCKET;
    }
  }

  // if a client or router, destroy the context
  //if( _role == CLIENT || _role == ROUTER ) {
  if( _owns_context ) {
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
connection_c::error_e connection_c::read( std::string& msg ) {
  // sanity checks
  assert( _open );

  char buffer[ RCV_BUFFER_SZ ];
  msg = "";

  int more;
  size_t sz = sizeof( int );
    
  do {
    int bytes = zmq_recv( _socket, buffer, RCV_BUFFER_SZ, 0 );
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
// TODO: determine oevel of detail for return value, i.e. bool or enum
connection_c::error_e connection_c::write( const std::string& msg ) {
  // sanity checks
  assert( _open );

  std::string chunk;        // a chunk of the message
  int bytes;

  // Compute the number of chunks
  unsigned chunks = msg.size() / SND_BUFFER_SZ;    // compute whole chunks
  if( msg.size() % SND_BUFFER_SZ ) chunks++;       // compute partial chunk

  for( unsigned i = 0; i < chunks; i++ ) {
    // Compute the size of the current chunk
    unsigned chunk_sz = ( (i+1) * SND_BUFFER_SZ > msg.size() ) ? 
                        msg.size() - (chunks - 1) * SND_BUFFER_SZ : 
                        SND_BUFFER_SZ;

    // Get the chunk out of the message
    chunk = msg.substr( i * SND_BUFFER_SZ, chunk_sz );
      
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
void connection_c::route( connection_c& dealer ) {
  // sanity checks
  assert( _role == ROUTER && dealer._role == DEALER );

  zmq_proxy( _socket, dealer._socket, NULL );
  // Note per zeromq API: zmq_proxy always returns -1 and errno == ETERM
  // but that annotation is ambiguous at best.
}

//-----------------------------------------------------------------------------
std::string connection_c::connection_string( void ) {
  std::stringstream ss;

  assert( _role != UNDEFINED );

  if( _role == CLIENT ) {
    ss << "tcp://" << _host << ":" << _port;
  } else if( _role == ROUTER ) {
    ss << "tcp://*:" << _port;
  } else if( _role == DEALER || _role == WORKER ) {
    ss << "inproc://workers";
///*
  } else if( _role == IPC_SERVER || _role == IPC_CLIENT ) {
    ss << "ipc://" << _id;
//*/
  }

  return ss.str();
}

//-----------------------------------------------------------------------------
void* connection_c::context( void ) {
  return _context;
}

//-----------------------------------------------------------------------------
void* connection_c::socket( void ) {
  return _socket;
}

//-----------------------------------------------------------------------------
}  // namespace Core
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------
