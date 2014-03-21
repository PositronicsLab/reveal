#include <Reveal/connection.h>

#include <assert.h>
#include <sstream>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------
connection_c::connection_c( void ) {
  _role = UNDEFINED;
  _open = false;
  _context = NULL;
  _socket = NULL;
}

//-----------------------------------------------------------------------------
connection_c::connection_c( const unsigned& port ) {
  _role = ROUTER;
  _open = false;
  _port = port;
}

//-----------------------------------------------------------------------------
connection_c::connection_c( const connection_c::role_e& role, void* context ) {
  assert( role == connection_c::WORKER || role == connection_c::DEALER );
  _role = role;
  _open = false;
  _context = context;
}


//-----------------------------------------------------------------------------
connection_c::connection_c( const std::string& host, const unsigned& port ) {
  _role = CLIENT;
  _open = false;
  _host = host;
  _port = port;
}

//-----------------------------------------------------------------------------
connection_c::~connection_c( void ) {
  if( _open ) close();
}

//-----------------------------------------------------------------------------
bool connection_c::open( void ) {
  assert( _role != UNDEFINED );

  if( _role == CLIENT ) {
    _context = zmq_ctx_new();
    _socket = zmq_socket( _context, ZMQ_REQ );
    zmq_connect( _socket, connection_string().c_str() );
  } else if( _role == ROUTER ) {
    _context = zmq_ctx_new();
    _socket = zmq_socket( _context, ZMQ_ROUTER );
    int rc = zmq_bind( _socket, connection_string().c_str() );
    assert( rc == 0 );
  } else if( _role == DEALER ) {
    _socket = zmq_socket( _context, ZMQ_DEALER );
    int rc = zmq_bind( _socket, connection_string().c_str() );
    assert( rc == 0 );
  } else if( _role == WORKER ) {
    _socket = zmq_socket( _context, ZMQ_REP );
    zmq_connect( _socket, connection_string().c_str() );
  }
  // TODO: Add robust error checking/handling in critical path development

  _open = true;
  return _open;
}

//-----------------------------------------------------------------------------
void connection_c::close( void ) {
  zmq_close( _socket );
  if( _role == CLIENT || _role == ROUTER )
    zmq_ctx_destroy( _context );

  _open = false;
}

//-----------------------------------------------------------------------------
bool connection_c::read( std::string& msg ) {
  assert( _open );

  char buffer[ RCV_BUFFER_SZ ];
  msg = "";

  int more;
  size_t sz = sizeof( int );
    
  do {
    int bytes = zmq_recv( _socket, buffer, RCV_BUFFER_SZ, 0 );
    assert( bytes != -1 );
    if( bytes == -1 ) {
      // errno set
    }
    zmq_getsockopt( _socket, ZMQ_RCVMORE, &more, &sz );
    if( more && bytes ) msg.append( buffer, bytes );
  } while( more );

  return true;
}

//-----------------------------------------------------------------------------
bool connection_c::write( const std::string& msg ) {
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
      // errno set
      return false;
    }
  }
  // Transmit a tiny notification that the message has been entirely sent
  bytes = zmq_send( _socket, "0", 1, 0 );
  if( bytes == -1) {
    // errno set
    return false;
  }
  
  // otherwise success
  return true;
}

//-----------------------------------------------------------------------------
void connection_c::route( connection_c& dealer ) {
  assert( _role == ROUTER && dealer._role == DEALER );
  zmq_proxy( _socket, dealer._socket, NULL );
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
  }

  return ss.str();
}

//-----------------------------------------------------------------------------
void* connection_c::context( void ) {
  return _context;
}

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------
