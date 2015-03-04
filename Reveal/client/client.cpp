#include "Reveal/client/client.h"

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>

#include "Reveal/core/authorization.h"
#include "Reveal/core/user.h"

#include "Reveal/core/transport_exchange.h"
#include "Reveal/core/pointers.h"
#include "Reveal/core/digest.h"
#include "Reveal/core/experiment.h"
#include "Reveal/core/scenario.h"
#include "Reveal/core/trial.h"
#include "Reveal/core/solution.h"

#include "Reveal/client/system.h"

//#include <Reveal/pendulum.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include "Reveal/client/gazebo.h"

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
Reveal::Client::client_ptr client_c::ptr( void ) {
  return shared_from_this();
}

//-----------------------------------------------------------------------------
/// Initialization
bool client_c::init( void ) {
  Reveal::Core::transport_exchange_c::open();
  gz = Reveal::Client::gazebo_ptr( new Reveal::Client::gazebo_c( ptr() ) );
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

  // create a authorization request
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

  // extract the authorization
  auth = server_exchange.get_authorization();
 
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
client_c::error_e client_c::request_digest( Reveal::Core::authorization_ptr& auth, Reveal::Core::digest_ptr& digest ) {
  std::string request;
  std::string reply;

  Reveal::Core::transport_exchange_c client_exchange;
  Reveal::Core::transport_exchange_c server_exchange;
  Reveal::Core::transport_exchange_c::error_e exchg_err;

  // create a digest request
  client_exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_CLIENT );
  client_exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_DIGEST );
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
  digest = server_exchange.get_digest();
 
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
client_c::error_e client_c::request_experiment( Reveal::Core::authorization_ptr& auth, Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr& experiment ) {
  std::string request;
  std::string reply;

  Reveal::Core::transport_exchange_c client_exchange;
  Reveal::Core::transport_exchange_c server_exchange;
  Reveal::Core::transport_exchange_c::error_e exchg_err;

  // create an experiment request
  client_exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_CLIENT );
  client_exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_EXPERIMENT );
  client_exchange.set_error( Reveal::Core::transport_exchange_c::ERROR_NONE );

  client_exchange.set_authorization( auth );

  experiment = Reveal::Core::experiment_ptr( new Reveal::Core::experiment_c() );
  experiment->scenario_id = scenario->id;
  client_exchange.set_experiment( experiment );

  // build the request message
  exchg_err = client_exchange.build( request );
  if( exchg_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    printf( "ERROR: failed to build experiment message\n" );
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

  // extract the experiment
  experiment = server_exchange.get_experiment();
 
  return ERROR_NONE;
}
/*
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
*/
//-----------------------------------------------------------------------------
client_c::error_e client_c::request_trial( Reveal::Core::authorization_ptr& auth, Reveal::Core::experiment_ptr experiment, Reveal::Core::trial_ptr& trial ) {
  std::string request;
  std::string reply;

  Reveal::Core::transport_exchange_c client_exchange;
  Reveal::Core::transport_exchange_c server_exchange;
  Reveal::Core::transport_exchange_c::error_e exchg_err;

  // create a trial request
  client_exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_CLIENT );
  client_exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_TRIAL );
  client_exchange.set_error( Reveal::Core::transport_exchange_c::ERROR_NONE );
  client_exchange.set_authorization( auth );
  client_exchange.set_experiment( experiment );
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
client_c::error_e client_c::submit_solution( Reveal::Core::authorization_ptr& auth, Reveal::Core::experiment_ptr experiment, Reveal::Core::solution_ptr& solution ) {
  std::string request;
  std::string reply;

  Reveal::Core::transport_exchange_c client_exchange;
  Reveal::Core::transport_exchange_c server_exchange;
  Reveal::Core::transport_exchange_c::error_e exchg_err;

  // create a solution 'request'
  client_exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_CLIENT );
  client_exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_SOLUTION );
  client_exchange.set_authorization( auth );
  client_exchange.set_experiment( experiment );
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
bool client_c::execute( void ) {

  // Initialization
  if( !init() ) {
    printf( "ERROR: Failed to initialize properly.\nExiting.\n" );
    terminate();
    return false;
  }

  // Salutations
  printf( "Welcome to Reveal\n" );

  // Connect to Server
  printf( "Connecting to Reveal Server\n" );

  if( !connect() ) {
    printf( "ERROR: Unable to reach Reveal Server.\nExiting.\n" );
    terminate();
    return false;
  }

  printf("Connected to Reveal Server\n");

  // Prompt for Login
  char input_buffer[512];

  bool anonymous_login = !prompt_yes_no( "Would you like to login (Y/N)?", true );

  // Authorization
  Reveal::Core::user_ptr user = Reveal::Core::user_ptr(new Reveal::Core::user_c() );
  Reveal::Core::authorization_ptr auth = Reveal::Core::authorization_ptr(new Reveal::Core::authorization_c() );

  if( anonymous_login ) {
    auth->set_type( Reveal::Core::authorization_c::TYPE_ANONYMOUS );
  } else {
    printf( "Please enter username: " );
    scanf( "%32s", input_buffer );
    user->id = input_buffer;
    auth->set_type( Reveal::Core::authorization_c::TYPE_IDENTIFIED );
    auth->set_user( user->id );
  }  

  if( anonymous_login ) 
    printf( "Logging in and requesting anonymous authorization.\n" );
  else
    printf( "Logging in and requesting authorization for user %s.\n", user->id.c_str() );

  // TODO: hammer on server side validation.  Noticed that authorization is
  // granted if a bad user name is specified
  error_e client_error;
  client_error = request_authorization( auth );
  if( client_error != ERROR_NONE ) {
    printf( "ERROR: Failed to gain authorization\n" );
  } else {
    printf( "Authorization granted\n" );
  }

  // MOTD
  printf("Message of the Day\n");

  bool recycle;
  do {
    //bool result = run_experiment( auth );
    bool result = gz->experiment( auth );

    recycle = prompt_yes_no( "Would you like to run another experiment (Y/N)?", false );
  } while( recycle );

  terminate();

  return true;
}

//-----------------------------------------------------------------------------

} // namespace Client

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------
