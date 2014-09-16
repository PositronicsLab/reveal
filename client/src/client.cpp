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
client_c::error_e client_c::request_reply( const std::string& request, std::string& reply ) {
  // send the request message to the server
  if( _connection.write( request ) != Reveal::Core::connection_c::ERROR_NONE ) {
    // write failed at connection
    return ERROR_WRITE;
  }
  // block waiting for a server response
  if( _connection.read( reply ) != Reveal::Core::connection_c::ERROR_NONE ) {
    // read failed at connection
    return ERROR_READ;
  }
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
client_c::error_e client_c::request_authorization( Reveal::Core::authorization_ptr& auth ) {
  std::string request;
  std::string reply;

  Reveal::Core::transport_exchange_c client_exchange;
  Reveal::Core::transport_exchange_c server_exchange;
  Reveal::Core::transport_exchange_c::error_e exchg_err;

  // create a digest request
  client_exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_CLIENT );
  client_exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_HANDSHAKE );
  client_exchange.set_error( Reveal::Core::transport_exchange_c::ERROR_NONE );

  client_exchange.set_authorization( auth );

  // build the request message
  exchg_err = client_exchange.build( request );
  if( exchg_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    return ERROR_EXCHANGE_BUILD;
  }

  // send the request message and wait for reply message
  client_c::error_e com_err = request_reply( request, reply );
  if( com_err != ERROR_NONE ) return com_err;

  // parse the reply message
  exchg_err = server_exchange.parse( reply );
  if( exchg_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    return ERROR_EXCHANGE_PARSE;
  }

  Reveal::Core::transport_exchange_c::error_e transport_error = server_exchange.get_error();
  if( transport_error != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    // The server sent a general error.  Suggest retrying the request.
    return ERROR_EXCHANGE_RESPONSE;
    // TODO : extend validation and error handling for any new cases that emerge
  }

  // extract the digest
  auth = server_exchange.get_authorization();
 
  return ERROR_NONE;
}
//-----------------------------------------------------------------------------
client_c::error_e client_c::request_digest( Reveal::Core::digest_ptr& digest ) {
  std::string request;
  std::string reply;

  Reveal::Core::transport_exchange_c client_exchange;
  Reveal::Core::transport_exchange_c server_exchange;
  Reveal::Core::transport_exchange_c::error_e exchg_err;

  // create a digest request
  client_exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_CLIENT );
  client_exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_DIGEST );
  client_exchange.set_error( Reveal::Core::transport_exchange_c::ERROR_NONE );

  // build the request message
  exchg_err = client_exchange.build( request );
  if( exchg_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    return ERROR_EXCHANGE_BUILD;
  }

  // send the request message and wait for reply message
  client_c::error_e com_err = request_reply( request, reply );
  if( com_err != ERROR_NONE ) return com_err;

  // parse the reply message
  exchg_err = server_exchange.parse( reply );
  if( exchg_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    return ERROR_EXCHANGE_PARSE;
  }

  Reveal::Core::transport_exchange_c::error_e transport_error = server_exchange.get_error();
  if( transport_error != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    // The server sent a general error.  Suggest retrying the request.
    return ERROR_EXCHANGE_RESPONSE;
    // TODO : extend validation and error handling for any new cases that emerge
  }

  // extract the digest
  digest = server_exchange.get_digest();
 
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
client_c::error_e client_c::request_scenario( Reveal::Core::scenario_ptr& scenario ) {
  std::string request;
  std::string reply;

  Reveal::Core::transport_exchange_c client_exchange;
  Reveal::Core::transport_exchange_c server_exchange;
  Reveal::Core::transport_exchange_c::error_e exchg_err;

  // create a scenario request 
  client_exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_CLIENT );
  client_exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_SCENARIO );
  client_exchange.set_error( Reveal::Core::transport_exchange_c::ERROR_NONE );
  client_exchange.set_scenario( scenario );

  // build the request message
  exchg_err = client_exchange.build( request );
  if( exchg_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    return ERROR_EXCHANGE_BUILD;
  }

  // send the request message and wait for reply message
  client_c::error_e com_err = request_reply( request, reply );
  if( com_err != ERROR_NONE ) return com_err;

  // parse the reply message
  exchg_err = server_exchange.parse( reply );
  if( exchg_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    return ERROR_EXCHANGE_PARSE;
  }

  Reveal::Core::transport_exchange_c::error_e transport_error = server_exchange.get_error();
  if( transport_error != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    if( transport_error == Reveal::Core::transport_exchange_c::ERROR_BAD_SCENARIO_REQUEST ) {
      // The server could not service the scenario request due to bad data in 
      // the request packet
      return ERROR_INVALID_SCENARIO_REQUEST;
    } else {
      // The server sent a general error.  Suggest retrying the request.
      return ERROR_EXCHANGE_RESPONSE;
    }
    // TODO : extend validation and error handling for any new cases that emerge
  }

  // extract the scenario
  scenario = server_exchange.get_scenario();
  assert( scenario );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
client_c::error_e client_c::request_trial( Reveal::Core::trial_ptr& trial ) {
  std::string request;
  std::string reply;

  Reveal::Core::transport_exchange_c client_exchange;
  Reveal::Core::transport_exchange_c server_exchange;
  Reveal::Core::transport_exchange_c::error_e exchg_err;

  // create a trial request 
  client_exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_CLIENT );
  client_exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_TRIAL );
  client_exchange.set_trial( trial );

  // build the request message
  exchg_err = client_exchange.build( request );
  if( exchg_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    return ERROR_EXCHANGE_BUILD;
  }

  // send the request message and wait for reply message
  client_c::error_e com_err = request_reply( request, reply );
  if( com_err != ERROR_NONE ) return com_err;

  // parse the reply message
  exchg_err = server_exchange.parse( reply );
  if( exchg_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    return ERROR_EXCHANGE_PARSE;
  }

  Reveal::Core::transport_exchange_c::error_e transport_error = server_exchange.get_error();
  if( transport_error != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    if( transport_error == Reveal::Core::transport_exchange_c::ERROR_BAD_SCENARIO_REQUEST ) {
      return ERROR_INVALID_SCENARIO_REQUEST;
    } else if( transport_error == Reveal::Core::transport_exchange_c::ERROR_BAD_TRIAL_REQUEST ) {
      // The server could not service the trial request due to bad data in the
      // request packet
      return ERROR_INVALID_TRIAL_REQUEST;
    } else {
      // The server sent a general error.  Suggest retrying the request.
      return ERROR_EXCHANGE_RESPONSE;
    }
    // TODO : extend validation and error handling for any new cases that emerge
  }

  // extract the trial
  trial = server_exchange.get_trial();
  assert( trial );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
client_c::error_e client_c::submit_solution( Reveal::Core::solution_ptr& solution ) {
  std::string request;
  std::string reply;

  Reveal::Core::transport_exchange_c client_exchange;
  Reveal::Core::transport_exchange_c server_exchange;
  Reveal::Core::transport_exchange_c::error_e exchg_err;

  // create a solution 'request'
  client_exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_CLIENT );
  client_exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_SOLUTION );
  client_exchange.set_solution( solution );

  // build the 'request' message
  exchg_err = client_exchange.build( request );
  if( exchg_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    return ERROR_EXCHANGE_BUILD;
  }

  // send the request message and wait for receipt ('reply') message
  client_c::error_e com_err = request_reply( request, reply );
  if( com_err != ERROR_NONE ) return com_err;

  // parse the reply message
  exchg_err = server_exchange.parse( reply );
  if( exchg_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    return ERROR_EXCHANGE_PARSE;
  }

  //if( server_exchange.get_type() != Reveal::Core::transport_exchange_c::TYPE_SOLUTION ) {

  Reveal::Core::transport_exchange_c::error_e transport_error = server_exchange.get_error();
  if( transport_error != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    if( transport_error == Reveal::Core::transport_exchange_c::ERROR_BAD_SOLUTION_SUBMISSION ) {
      // The server rejected the solution because the message contained invalid
      // data, e.g. scenario_id or trial_id invalid or the solution state was
      // not formatted properly
      return ERROR_INVALID_SOLUTION;
    } else {
      // Client did not received the appropriate receipt from the server
      return ERROR_EXCHANGE_RESPONSE;
    }
    // TODO : extend validation and error handling for any new cases that emerge
  }

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
/// Clean up
void client_c::terminate( void ) {
  // Note: may need to loop on connection close operation if they don't
  // return ERROR_NONE and do return ERROR_INTERRUPT
  _connection.close();

  Reveal::Core::transport_exchange_c::close();
}

//-----------------------------------------------------------------------------
/// Connect to the transport layer as a client
bool client_c::connect( void ) {
  printf( "Connecting to server...\n" );

  _connection = Reveal::Core::connection_c( REVEAL_SERVER_URI, PORT );
  if( _connection.open() != Reveal::Core::connection_c::ERROR_NONE ) {
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
