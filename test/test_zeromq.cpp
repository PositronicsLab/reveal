/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

A unit test to validate zeromq server and client communication using variable
length messages

-----------------------------------------------------------------------------*/

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <sstream>

#include <protocols.pb.h>

//-----------------------------------------------------------------------------

#define SENDER_CHUNK_SZ    64
#define RECEIVE_BUFFER_SZ  SENDER_CHUNK_SZ+1
// Note: Receive buffer needs to be as large or larger than the chunk size

#define PORT 20700  
// ports around this range appear to not be assigned in public registeries

//-----------------------------------------------------------------------------
/// Creates a standard variable length test message that can be transmitted 
/// then validated
reveal::ip::Models generate_test_msg( void ) {
  reveal::ip::Models models;
  models.add_url( "http://robotics.gwu.edu/~positronics/" );
  models.add_url( "http://www.osrfoundation.org/" );
  models.add_url( "http://www.gazebosim.org/" );
  return models;
}

//-----------------------------------------------------------------------------
/// Build a ZeroMQ compatible server (bind) string
std::string server_connection_string( void ) {
  std::stringstream ss;
  ss << "tcp://*:";
  ss << PORT;
  return ss.str();
}

//-----------------------------------------------------------------------------
/// Build a ZeroMQ compatible client remote (connect) string
std::string client_connection_string( void ) {
  std::stringstream ss;
  ss << "tcp://localhost:";
  ss << PORT;
  return ss.str();
}


//-----------------------------------------------------------------------------
/// The server side communication
void server( void ) {

  //  Set up ZeroMQ socket
  void *context = zmq_ctx_new();
  void *socket = zmq_socket( context, ZMQ_REP );
  int rc = zmq_bind( socket, server_connection_string().c_str() );
  assert( rc == 0 );
  // TODO: Add robust error checking/handling in critical path development

  printf( "Server is listening…\n" );

  // generate a sample expected message for any error checking
  reveal::ip::Models model_expected = generate_test_msg();
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

  // Clean up any open zmq connections
  zmq_close( socket );
  zmq_ctx_destroy( context );
  // Note: Only reachable on failed validation
}

//-----------------------------------------------------------------------------
/// The client side communication
void client( void ) {
  
  printf( "Connecting to server…\n" );

  //  Set up ZeroMQ socket
  void *context = zmq_ctx_new();
  void *socket = zmq_socket( context, ZMQ_REQ );
  zmq_connect( socket, client_connection_string().c_str() );
  // TODO: Add robust error checking/handling in critical path development

  printf( "Connected\n" );

  // Create a test message
  reveal::ip::Models models = generate_test_msg();

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

  // Clean up any open zmq connections
  zmq_close( socket );
  zmq_ctx_destroy( context );
}

//-----------------------------------------------------------------------------
/// Test Entry Point.
/// Usage: if the program is executed with no parameters, it acts as a server
///        if the program is executed with any parameters, it acts as a client
int main( int argv, char* argc[] ) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  if( argv == 1 ) { 
    // if parameter is only the program name, run as server
    server();
  } else {
    // otherwise if there are any additional parameters, run as client
    client();
  }

  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}

//-----------------------------------------------------------------------------
