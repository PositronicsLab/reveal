#include <Reveal/connection.h>

#include <assert.h>
#include <sstream>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------
Connection::Connection( void ) {
  _role = UNDEFINED;
  _open = false;
}

//-----------------------------------------------------------------------------
Connection::Connection( const unsigned& port ) {
  _role = SERVER;
  _open = false;
  _port = port;
}

//-----------------------------------------------------------------------------
Connection::Connection( const std::string& host, const unsigned& port ) {
  _role = CLIENT;
  _open = false;
  _host = host;
  _port = port;
}

//-----------------------------------------------------------------------------
Connection::~Connection( void ) {
  if( _open ) Close();
}

//-----------------------------------------------------------------------------
bool Connection::Open( void ) {
  assert( _role != UNDEFINED );

  _context = zmq_ctx_new();
  if( _role == CLIENT ) {
    _socket = zmq_socket( _context, ZMQ_REQ );
    zmq_connect( _socket, ConnectionString().c_str() );
  } else if( _role == SERVER ) {
    _socket = zmq_socket( _context, ZMQ_REP );
    int rc = zmq_bind( _socket, ConnectionString().c_str() );
    assert( rc == 0 );
  }
  // TODO: Add robust error checking/handling in critical path development

  _open = true;
  return _open;
}

//-----------------------------------------------------------------------------
void Connection::Close( void ) {
  zmq_close( _socket );
  zmq_ctx_destroy( _context );

  _open = false;
}

//-----------------------------------------------------------------------------
bool Connection::Read( std::string& msg ) {
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
bool Connection::Write( const std::string& msg ) {
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

  return true;
}

//-----------------------------------------------------------------------------
std::string Connection::ConnectionString( void ) {
  std::stringstream ss;

  assert( _role != UNDEFINED );

  if( _role == CLIENT ) {
    ss << "tcp://" << _host << ":" << _port;
  } else if( _role == SERVER ) {
    ss << "tcp://*:" << _port;
  }

  return ss.str();
}

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------
