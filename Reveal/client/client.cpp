#include "Reveal/client/client.h"

#include <boost/bind.hpp>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>

#include "Reveal/core/console.h"
#include "Reveal/core/error.h"

#include "Reveal/core/authorization.h"
#include "Reveal/core/user.h"

#include "Reveal/core/system.h"
#include "Reveal/core/transport_exchange.h"
#include "Reveal/core/pointers.h"
#include "Reveal/core/digest.h"
#include "Reveal/core/experiment.h"
#include "Reveal/core/scenario.h"
#include "Reveal/core/trial.h"
#include "Reveal/core/solution.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include "Reveal/sim/gazebo/gazebo.h"
//TODO once CMakeLists supports change above include to "Reveal/sim/gazebo.h"

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
  _system = boost::shared_ptr<Reveal::Core::system_c>( new Reveal::Core::system_c( Reveal::Core::system_c::CLIENT ) );

  if( !_system->open() ) return false;

  Reveal::Core::transport_exchange_c::open();
  return true;
}

//-----------------------------------------------------------------------------
/// Clean up
void client_c::terminate( void ) {
  // Note: may need to loop on connection close operation if they don't
  // return ERROR_NONE and do return ERROR_INTERRUPT
  _connection.close();

  Reveal::Core::transport_exchange_c::close();

  _system->close();
}

//-----------------------------------------------------------------------------
/// Connect to the transport layer as a client
bool client_c::connect( void ) {
  printf( "Connecting to server...\n" );

  _connection = Reveal::Core::connection_c( _system->server_host(), _system->server_port() );
  if( _connection.open() != Reveal::Core::connection_c::ERROR_NONE ) {
    return false;
  }

  printf( "Connected\n" );
  return true;
}

//-----------------------------------------------------------------------------
bool client_c::login( void ) {

  // Note: this should actually be structured to trigger by a signal sent from 
  // server to reduce exploit
  for( unsigned i = 0; i < 3; i++ ) {
    // Prompt for Login
    bool anonymous_login = !Reveal::Core::console_c::prompt_yes_no( "Would you like to login (Y/N)?" );

    // Authorization
    _user = Reveal::Core::user_ptr( new Reveal::Core::user_c() );
    _auth = Reveal::Core::authorization_ptr( new Reveal::Core::authorization_c());

    if( anonymous_login ) {
      _auth->set_type( Reveal::Core::authorization_c::TYPE_ANONYMOUS );
    } else {
      _user->id = Reveal::Core::console_c::prompt( "Please enter a username:" );
      _auth->set_type( Reveal::Core::authorization_c::TYPE_IDENTIFIED );
      _auth->set_user( _user->id );
    }  

    if( anonymous_login ) 
      printf( "Logging in and requesting anonymous authorization.\n" );
    else
      printf( "Logging in and requesting authorization for user %s.\n", _user->id.c_str() );

    // TODO: hammer on server side validation.  Noticed that authorization is
    // granted if a bad user name is specified
    error_e client_error;
    client_error = request_authorization( _auth );
    if( client_error != ERROR_NONE ) {
      printf( "ERROR: Failed to gain authorization\n" );
    } else {
      printf( "Authorization granted\n" );
      return true;
    }
  }
  return false;
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
unsigned client_c::ui_select_scenario( Reveal::Core::digest_ptr digest ) {

  std::vector< std::string > scenario_list;
  for( unsigned i = 0; i < digest->scenarios(); i++ ) 
    scenario_list.push_back( digest->get_scenario( i )->description );

  return Reveal::Core::console_c::menu( "--Scenario Menu--", "Select a scenario", scenario_list );
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
bool client_c::request_trial( Reveal::Core::authorization_ptr& auth, Reveal::Core::experiment_ptr experiment, Reveal::Core::trial_ptr& trial ) {
  std::string request;
  std::string reply;

  Reveal::Core::transport_exchange_c client_exchange;
  Reveal::Core::transport_exchange_c server_exchange;
  Reveal::Core::transport_exchange_c::error_e exchg_err;

  //client_exchange.open();
  //server_exchange.open();

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
    //Reveal::Core::console_c::printline( "ERROR_EXCHANGE_BUILD" );
    //return ERROR_EXCHANGE_BUILD;
    return false;
  }

  //Reveal::Core::console_c::printline( request );

  // send the request message and wait for reply message
  client_c::error_e com_err = request_reply( request, reply );
  if( com_err != ERROR_NONE ) {
    //Reveal::Core::console_c::printline( "com_err" );
    //return com_err;
    return false;
  }

  // parse the reply message
  exchg_err = server_exchange.parse( reply );
  if( exchg_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    //Reveal::Core::console_c::printline( "ERROR_EXCHANGE_PARSE" );
    //return ERROR_EXCHANGE_PARSE;
    return false;
  }

  Reveal::Core::transport_exchange_c::error_e transport_error = server_exchange.get_error();
  if( transport_error != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    if( transport_error == Reveal::Core::transport_exchange_c::ERROR_BAD_SCENARIO_REQUEST ) {
      //Reveal::Core::console_c::printline( "ERROR_INVALID_SCENARIO_REQUEST" );
      //return ERROR_INVALID_SCENARIO_REQUEST;
      return false;
    } else if( transport_error == Reveal::Core::transport_exchange_c::ERROR_BAD_TRIAL_REQUEST ) {
      // The server could not service the trial request due to bad data in the
      // request packet
      //Reveal::Core::console_c::printline( "ERROR_INVALID_TRIAL_REQUEST" );
      //return ERROR_INVALID_TRIAL_REQUEST;
      return false;
    } else {
      // The server sent a general error.  Suggest retrying the request.
      //Reveal::Core::console_c::printline( "ERROR_EXCHANGE_RESPONSE" );
      //return ERROR_EXCHANGE_RESPONSE;
      return false;
    }
    // TODO : extend validation and error handling for any new cases that emerge
  }

  // extract the trial
  trial = server_exchange.get_trial();
  assert( trial );

  //trial->print();

  //Reveal::Core::console_c::printline( "completed request_trial" );

  //return ERROR_NONE;
  return true;
}

//-----------------------------------------------------------------------------
bool client_c::submit_solution( Reveal::Core::authorization_ptr& auth, Reveal::Core::experiment_ptr experiment, Reveal::Core::solution_ptr& solution ) {
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
    //return ERROR_EXCHANGE_BUILD;
    return false;
  }

  // send the request message and wait for receipt ('reply') message
  client_c::error_e com_err = request_reply( request, reply );
  if( com_err != ERROR_NONE ) {
    //return com_err;
    return false;
  }

  // parse the reply message
  exchg_err = server_exchange.parse( reply );
  if( exchg_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    //return ERROR_EXCHANGE_PARSE;
    return false;
  }

  //if( server_exchange.get_type() != Reveal::Core::transport_exchange_c::TYPE_SOLUTION ) {

  Reveal::Core::transport_exchange_c::error_e transport_error = server_exchange.get_error();
  if( transport_error != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    if( transport_error == Reveal::Core::transport_exchange_c::ERROR_BAD_SOLUTION_SUBMISSION ) {
      // The server rejected the solution because the message contained invalid
      // data, e.g. scenario_id or trial_id invalid or the solution state was
      // not formatted properly
      //return ERROR_INVALID_SOLUTION;
      return false;
    } else {
      // Client did not received the appropriate receipt from the server
      //return ERROR_EXCHANGE_RESPONSE;
      return false;
    }
    // TODO : extend validation and error handling for any new cases that emerge
  }

  //return ERROR_NONE;
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

  // - Approach 1 -  This constructor method may not be defined for all sims
//  bind the clients request_trial and submit_solution functions
//  simulator_c::request_trial_f rtf = boost::bind(&client_c::request_trial, this, _1, _2, _3);
//  simulator_c::submit_solution_f ssf = boost::bind(&client_c::submit_solution, this, _1, _2, _3);
  // create the simulator (using the bound functions and comm layer context)
//  simulator = boost::dynamic_pointer_cast<Reveal::Core::simulator_c>( Reveal::Sim::gazebo_ptr( new Reveal::Sim::gazebo_c( rtf, ssf, _connection.context() ) ) );

  // - Approach 2 -  This approach is universal for all sims
  // create the simulator
  simulator = boost::dynamic_pointer_cast<Reveal::Core::simulator_c>( Reveal::Sim::gazebo_ptr( new Reveal::Sim::gazebo_c() ) );
  // bind the client request_trial function to the simulator for callback
  simulator->set_request_trial( boost::bind(&client_c::request_trial, this, _1, _2, _3) );
  // bind the client submit_solution function to the simulator for callback
  simulator->set_submit_solution( boost::bind(&client_c::submit_solution, this, _1, _2, _3) );
  // set the simulator ipc context so that it is in the same comm layer
  simulator->set_ipc_context( _connection.context() );

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

  if( !login() ) { 
    printf( "Failed to login.\nExiting.\n" );
    terminate();
    return false;
  }

  // MOTD
  printf("Message of the Day\n");

  bool recycle;
  do {
    error_e error;

    Reveal::Core::digest_ptr digest;
    Reveal::Core::scenario_ptr scenario;
    Reveal::Core::experiment_ptr experiment;

    printf( "Fetching Scenario Digest\n" );

    // request the digest from the server
    // filter request_digest for scenarios compatible with installed simulators?
    error = request_digest( _auth, digest );
    if( error != ERROR_NONE ) {
      printf( "ERROR: client failed to receive digest\n" );
      // TODO: error handling
      // TODO: convert error reporting over to Core::error_c
    }

    // user selects a scenario from the digest
    unsigned scenario_choice = ui_select_scenario( digest );

    // fetch scenario
    scenario = digest->get_scenario( scenario_choice );

    // request experiment
    error = request_experiment( _auth, scenario, experiment );
    if( error != ERROR_NONE ) {
      printf( "ERROR: client failed to receive experiment\n" );
      // TODO: error handling
      // TODO: convert error reporting over to Core::error_c
    }

    // user selects any engine specific parameters
    if( !simulator->ui_select_configuration() ) {
      // TODO: error handling.
      // if false, no choice but to bomb with unrecognized/unrecoverable
    }

    // user selects any package specific parameters
    if( !simulator->ui_select_tuning() ) {
      // TODO: error handling.
      // if false, no choice but to bomb with unrecognized/unrecoverable
    }

    // TODO : package parameters.  At this point, now know the package path
    // via some mechanism
    std::string pkg_root_path = PACKAGE_ROOT_PATH;
    std::string pkg_source_path = pkg_root_path + "industrial_arm/shared";
    std::string pkg_build_path = pkg_root_path + "industrial_arm/shared/build";

    if( !simulator->build_package( pkg_source_path, pkg_build_path ) ) {
      printf( "Exiting\n" );
      exit( 1 );
    }

    bool result = simulator->execute( _auth, scenario, experiment );
    if( !result ) {
      // TODO: determine how to recover
    }

    recycle = Reveal::Core::console_c::prompt_yes_no( "Would you like to run another experiment (Y/N)?" );
  } while( recycle );

  terminate();

  return true;
}

//-----------------------------------------------------------------------------

} // namespace Client

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------
