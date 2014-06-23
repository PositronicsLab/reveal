#include <Reveal/client.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>

#include <Reveal/transport_exchange.h>
#include <Reveal/pointers.h>
#include <Reveal/digest.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>

//#include <Reveal/pendulum.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Client {

//-----------------------------------------------------------------------------
/// Default Constructor
client_c::client_c( void ) {

}

//-----------------------------------------------------------------------------
/// Destructor
client_c::~client_c( void ) {

}

//-----------------------------------------------------------------------------
/// Initialization
bool client_c::init( void ) {
  Reveal::Core::transport_exchange_c::open();

  return true;
}

//-----------------------------------------------------------------------------
bool client_c::request_reply( const std::string& request, std::string& reply ) {
  // send the request message to the server
  if( !_connection.write( request ) ) {
    // write failed at connection
    printf( "ERROR: failed to write message to connection\n" );
    // TODO: improve error handling.  Should bomb or recover here.
    return false;
  }
  // block waiting for a server response
  if( !_connection.read( reply ) ) {
    // read failed at connection
    // right now this should trigger an assert
    // TODO: improve error handling.  Should bomb or recover here.
    printf( "ERROR: failed to read message from connection\n" );
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
bool client_c::request_digest( Reveal::Core::digest_ptr& digest ) {
  std::string request;
  std::string reply;

  Reveal::Core::transport_exchange_c client_exchange;
  Reveal::Core::transport_exchange_c server_exchange;
  Reveal::Core::transport_exchange_c::error_e err;

  // create a digest request
  client_exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_CLIENT );
  client_exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_DIGEST );
  client_exchange.set_error( Reveal::Core::transport_exchange_c::ERROR_NONE );

  // build the request message
  err = client_exchange.build( request );
  if( err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    printf( "failed to build digest request message\n" );
    return false;
  }

  //std::cout << msg_request << std::endl;

  // send the request message and wait for reply message
  if( !request_reply( request, reply ) ) return false;

  // parse the reply message
  server_exchange.parse( reply );
  // TODO : validation and error handling

  // extract the digest
  digest = server_exchange.get_digest();
  digest->print();
 
  return true;
}

//-----------------------------------------------------------------------------
bool client_c::request_scenario( Reveal::Core::scenario_ptr& scenario ) {
  std::string request;
  std::string reply;

  Reveal::Core::transport_exchange_c client_exchange;
  Reveal::Core::transport_exchange_c server_exchange;
  Reveal::Core::transport_exchange_c::error_e err;

  // create a scenario request 
  client_exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_CLIENT );
  client_exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_SCENARIO );
  client_exchange.set_error( Reveal::Core::transport_exchange_c::ERROR_NONE );
  client_exchange.set_scenario( scenario );

  // build the request message
  err = client_exchange.build( request );
  if( err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    printf( "failed to build scenario request message\n" );
    return false;
  }

  // send the request message and wait for reply message
  if( !request_reply( request, reply ) ) return false;

  // parse the reply message
  server_exchange.parse( reply );
  // TODO : validation and error handling

  // extract the scenario
  scenario = server_exchange.get_scenario();

  // TODO: comment/remove later
  scenario->print();

  return true;
}

//-----------------------------------------------------------------------------
bool client_c::request_trial( Reveal::Core::trial_ptr& trial ) {
  std::string request;
  std::string reply;

  Reveal::Core::transport_exchange_c client_exchange;
  Reveal::Core::transport_exchange_c server_exchange;
  Reveal::Core::transport_exchange_c::error_e err;

  // create a trial request 
  client_exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_CLIENT );
  client_exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_TRIAL );
  client_exchange.set_trial( trial );

  // build the request message
  err = client_exchange.build( request );
  if( err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    printf( "failed to build scenario request message\n" );
    return false;
  }

  // send the request message and wait for reply message
  if( !request_reply( request, reply ) ) return false;

  // parse the reply message
  server_exchange.parse( reply );
  // TODO : validation and error handling

  // extract the trial
  trial = server_exchange.get_trial();
  // TODO : validation and error handling

  // TODO: comment/remove later
  trial->print();

  return true;
}

//-----------------------------------------------------------------------------
bool client_c::submit_solution( Reveal::Core::solution_ptr& solution ) {
  std::string request;
  std::string reply;

  Reveal::Core::transport_exchange_c client_exchange;
  Reveal::Core::transport_exchange_c server_exchange;
  Reveal::Core::transport_exchange_c::error_e err;

  // create a solution 'request'
  client_exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_CLIENT );
  client_exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_SOLUTION );
  client_exchange.set_solution( solution );

  // build the 'request' message
  err = client_exchange.build( request );
  if( err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    printf( "failed to build scenario request message\n" );
    return false;
  }

  // send the request message and wait for receipt ('reply') message
  if( !request_reply( request, reply ) ) return false;

  // parse the reply message
  server_exchange.parse( reply );
  // TODO : validation and error handling

  // TODO : validation and error handling
  if( server_exchange.get_type() != Reveal::Core::transport_exchange_c::TYPE_SOLUTION ) {
    // We have not received the appropriate receipt from the server
  }

  return true;
}

//-----------------------------------------------------------------------------
/// Clean up
void client_c::terminate( void ) {
  _connection.close();

  Reveal::Core::transport_exchange_c::close();
}

//-----------------------------------------------------------------------------
/// Connect to the transport layer as a client
bool client_c::connect( void ) {
  printf( "Connecting to server...\n" );

  _connection = Reveal::Core::connection_c( REVEAL_SERVER_URI, PORT );
  if( !_connection.open() ) {
    return false;
  }

  printf( "Connected\n" );
  return true;
}

//-----------------------------------------------------------------------------

} // namespace Client

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------
