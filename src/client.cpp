#include <reveal/client.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>

namespace reveal {

//-----------------------------------------------------------------------------
Client::Client( void ) {

}

//-----------------------------------------------------------------------------
Client::~Client( void ) {

}

//-----------------------------------------------------------------------------
bool Client::Init( void ) {
  return true;
}

//-----------------------------------------------------------------------------
bool Client::Connect( void ) {
  printf( "Connecting to server...\n" );

  //  Set up ZeroMQ socket
  context = zmq_ctx_new();
  socket = zmq_socket( context, ZMQ_REQ );
  zmq_connect( socket, getConnectionString().c_str() );
  // TODO: Add robust error checking/handling in critical path development

  printf( "Connected\n" );
  // Create a test message
  ip::Models models = genTestMessage();

  // Make several attempts to send the message
  for( int attempt = 0; attempt < 10; attempt++ ) {

    std::string ser;          // the serialized message
    std::string chunk;        // a chunk of the message

    // Serialize the protocol
    models.SerializeToString( &ser );

    // Compute the number of chunks
    unsigned chunks = ser.size() / SENDER_CHUNK_SZ;       // compute whole chunks
    if( ser.size() % SENDER_CHUNK_SZ ) chunks++;     // compute partial chunk

    for( unsigned i = 0; i < chunks; i++ ) {
      // Compute the size of the current chunk
      // Either this or the following.  Both are ugly
      //unsigned chunk_sz = ((i+1)*SENDER_CHUNK_SZ > ser.size()) ? ser.size()-(chunks - 1)*SENDER_CHUNK_SZ : SENDER_CHUNK_SZ;

      unsigned chunk_sz = SENDER_CHUNK_SZ;
      if( (i+1) * SENDER_CHUNK_SZ > ser.size() )
        chunk_sz = ser.size() - (chunks - 1) * SENDER_CHUNK_SZ;

      // Get the chunk out of the serialization string
      chunk = ser.substr( i * SENDER_CHUNK_SZ, chunk_sz );
      
      // Transmit the chunk with the notification flag that more is to follow
      zmq_send( socket, chunk.c_str(), chunk_sz, ZMQ_SNDMORE );
    }
    // Transmit a tiny notification that the message has been entirely sent
    zmq_send( socket, "0", 1, 0 );

    // Wait for a notification that the message was received
    char buf[1];
    zmq_recv( socket, buf, 1, 0 );

    // Print the result of the validation
    if( buf[0] == 0 ) {
      printf( "Validation Failed:  Read a failure notification from server\n" );
    } else {
      printf( "Read a successful receipt from server (attempt %d)\n", attempt );
    }

    // Force a yield
    sleep( 1 );
  }

  return true;
}

//-----------------------------------------------------------------------------
void Client::Terminate( void ) {
  zmq_close( socket );
  zmq_ctx_destroy( context );
}

//-----------------------------------------------------------------------------
std::string Client::getConnectionString( void ) {
  std::stringstream ss;
  ss << "tcp://localhost:";
  ss << PORT;
  return ss.str();
}

//-----------------------------------------------------------------------------
ip::Models Client::genTestMessage( void ) {
  ip::Models models;
  models.add_url( "http://robotics.gwu.edu/~positronics/" );
  models.add_url( "http://www.osrfoundation.org/" );
  models.add_url( "http://www.gazebosim.org/" );
  return models;
}

//-----------------------------------------------------------------------------

} // namespace reveal

//-----------------------------------------------------------------------------
