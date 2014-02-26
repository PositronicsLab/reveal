#include <reveal/server.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>

namespace reveal {

//-----------------------------------------------------------------------------
Server::Server( void ) {

}

//-----------------------------------------------------------------------------
Server::~Server( void ) {

}

//-----------------------------------------------------------------------------
bool Server::Init( void ) {
  context = zmq_ctx_new();
  socket = zmq_socket( context, ZMQ_REP );
  int rc = zmq_bind( socket, getConnectionString().c_str() );
  assert( rc == 0 );
  // TODO: add robust error checking

  printf( "Server is listening...\n" );

  return true; 
}

//-----------------------------------------------------------------------------
void Server::Run( void ) {
  // generate a sample expected message for any error checking
  reveal::ip::Models model_expected = genTestMessage();
  std::string ser_expected;
  model_expected.SerializeToString( &ser_expected );

  while( true ) {
    char buffer[ RECEIVE_BUFFER_SZ ];
    std::string ser_actual = "";

    int more;
    int nbytes;
    size_t sz = sizeof( int );
    
    int chunks = 0;
    do {
      nbytes = zmq_recv( socket, buffer, RECEIVE_BUFFER_SZ, 0 ); 
      assert( nbytes != -1 );
      zmq_getsockopt( socket, ZMQ_RCVMORE, &more, &sz );
      if( more ) {
        ser_actual.append( buffer, nbytes );
      }
      chunks++;
      //printf( "received chunk: %d, bytes: %d, more: %d\n", chunks, nbytes, more );
    } while( more );

    // Validation
    // Check that actual and expected have same length
    if( ser_expected.size() != ser_actual.size() ) {
      printf( "Validation Failed: Actual size of the message did not equal expected size\n" );
      // Reply with a 'failure' notification
      zmq_send( socket, "0", 1, 0 );
      break;
    }
    // Compare the two strings
    if( ser_expected.compare( ser_actual ) != 0 ) {
      printf( "Validation Failed: Actual string did not equal expected string\n" );
      // Reply with a 'failure' notification
      zmq_send( socket, "0", 1, 0 );
      break;
    }
    // Validation succeeded!.  The message should parse.

    // Print the message to the screen for the user
    reveal::ip::Models models;
    models.ParseFromString( ser_actual.c_str() );
    std::string url;
    unsigned urls = models.url_size();
    for( unsigned i = 0; i < urls; i++ ) {
      url = models.url( i );
      printf( "url[%u]: %s\n", i, url.c_str() );
    }

    // Reply with a 'success' notification
    zmq_send( socket, "1", 1, 0 );

    // Force a yield
    sleep( 1 );
  }
}

//-----------------------------------------------------------------------------
void Server::Terminate( void ) {
  zmq_close( socket );
  zmq_ctx_destroy( context );
} 

//-----------------------------------------------------------------------------
std::string Server::getConnectionString( void ) {
  std::stringstream ss;
  ss << "tcp://*:";
  ss << PORT;
  return ss.str();
}

//-----------------------------------------------------------------------------
ip::Models Server::genTestMessage( void ) {
  ip::Models models;
  models.add_url( "http://robotics.gwu.edu/~positronics/" );
  models.add_url( "http://www.osrfoundation.org/" );
  models.add_url( "http://www.gazebosim.org/" );
  return models;
}

//-----------------------------------------------------------------------------
void* Server::Worker( void* arg ) {
  // send state & command

  // wait for state reply
  
  return NULL;
}

//-----------------------------------------------------------------------------

} // namespace reveal

//-----------------------------------------------------------------------------
