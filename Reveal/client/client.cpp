#include "Reveal/client/client.h"

#include <boost/bind.hpp>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <limits>
#include <math.h>

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
client_c::client_c( void ) {

}

//-----------------------------------------------------------------------------
client_c::~client_c( void ) {

}

//-----------------------------------------------------------------------------
Reveal::Client::client_ptr client_c::ptr( void ) {
  return shared_from_this();
}

//-----------------------------------------------------------------------------
bool client_c::init( void ) {
  _system = boost::shared_ptr<Reveal::Core::system_c>( new Reveal::Core::system_c( Reveal::Core::system_c::CLIENT ) );

  if( !_system->open() ) return false;

  Reveal::Core::transport_exchange_c::open();

  if( !get_temp_directory( _working_directory ) ) {
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
void client_c::terminate( void ) {
  // prune and remove temporary working directory
  remove_directory( _working_directory );

  // Note: may need to loop on connection close operation if they don't
  // return ERROR_NONE and do return ERROR_INTERRUPT
  _connection.close();

  Reveal::Core::transport_exchange_c::close();

  _system->close();
}

//-----------------------------------------------------------------------------
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
  std::string menu_title = "-- Scenario Menu --";
  std::string menu_prompt = "Select a scenario";

  // build a menu list based on the set of scenarios in the digest
  std::vector< std::string > scenario_list;
  for( unsigned i = 0; i < digest->scenarios(); i++ ) 
    scenario_list.push_back( digest->get_scenario( i )->description );

  // pass selection on to the console and return the menu item selected
  return Reveal::Core::console_c::menu( menu_title, menu_prompt, scenario_list );
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
  client_exchange.set_scenario( scenario );
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

  // check for an error in the reply
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

//-----------------------------------------------------------------------------
// TODO: convert to enumerated return value
bool client_c::request_trial( Reveal::Core::authorization_ptr& auth, Reveal::Core::experiment_ptr experiment, Reveal::Core::trial_ptr& trial ) {
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
    //Reveal::Core::console_c::printline( "ERROR_EXCHANGE_BUILD" );
    //return ERROR_EXCHANGE_BUILD;
    return false;
  }

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

  // check for an error in the reply 
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

  //return ERROR_NONE;
  return true;
}

//-----------------------------------------------------------------------------
// TODO: convert to enumerated return value
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

  // check for an error in the reply 
  Reveal::Core::transport_exchange_c::error_e transport_error = server_exchange.get_error();
  if( transport_error != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
    if( transport_error == Reveal::Core::transport_exchange_c::ERROR_BAD_SOLUTION_SUBMISSION ) {
      // The server rejected the solution because the message contained invalid
      // data, e.g. scenario or trial keys invalid or the solution state was
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
void client_c::prompt_trials_to_ignore( Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr experiment ) {
  std::string prompt = "How many trials should be ignored before resetting simulator to trial state";

  unsigned choice = Reveal::Core::console_c::prompt_unsigned( prompt, true );

  experiment->intermediate_trials_to_ignore = choice;
}
//-----------------------------------------------------------------------------
void client_c::prompt_time_step( Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr experiment ) {
  bool valid = false;
  double candidate_time_step;
  double x;
  std::stringstream ratemsg;
  ratemsg << "The sample rate is " << scenario->sample_rate;
  double epsilon;

  std::string prompt = "What time-step would you like the experiment to use";

  do {
    Reveal::Core::console_c::printline( ratemsg );
    candidate_time_step = (double)Reveal::Core::console_c::prompt_double( prompt, epsilon, false );

    if( candidate_time_step == 0.0 ) {
      Reveal::Core::console_c::printline( "INVALID: The time-step cannot be zero!" );
      continue;
    }

    if( scenario->sample_rate > candidate_time_step ) {
      //x = scenario->sample_rate / candidate_time_step;
      x = scenario->sample_rate;

      do {
        x -= candidate_time_step;
      } while( x > epsilon );
    } else if( candidate_time_step > scenario->sample_rate ) {
      //x = candidate_time_step / scenario->sample_rate;
      x = candidate_time_step;

      do {
        x -= scenario->sample_rate;
      } while( x > epsilon );
    } else {
      x = 0.0;
    }
    //double m = floor(x);
    //printf( "sample_rate[%1.24f], time_step[%1.24f], x[%1.24f], EPSILON[%1.24f]\n", scenario->sample_rate, candidate_time_step, x, epsilon );
    if( fabs(x) <= epsilon ) {
      valid = true;
    } else {
      Reveal::Core::console_c::printline( "INVALID: The time-step must be a factor of the sample rate!" );
      continue;
    }
  } while( !valid );

  experiment->time_step = candidate_time_step;
  experiment->epsilon = epsilon;
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
//  _simulator = boost::dynamic_pointer_cast<Reveal::Core::simulator_c>( Reveal::Sim::gazebo_ptr( new Reveal::Sim::gazebo_c( rtf, ssf, _connection.context() ) ) );

  // - Approach 2 -  This approach is universal for all sims
  // create the simulator
  _simulator = boost::dynamic_pointer_cast<Reveal::Core::simulator_c>( Reveal::Sim::gazebo_ptr( new Reveal::Sim::gazebo_c() ) );
  // bind the client request_trial function to the simulator for callback
  _simulator->set_request_trial( boost::bind(&client_c::request_trial, this, _1, _2, _3) );
  // bind the client submit_solution function to the simulator for callback
  _simulator->set_submit_solution( boost::bind(&client_c::submit_solution, this, _1, _2, _3) );
  // set the simulator ipc context so that it is in the same comm layer
  _simulator->set_ipc_context( _connection.context() );

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

  // get the root paths for the selected package image and the temporary path
  std::string pkg_root_image_path = _system->package_path();
  std::string pkg_root_tmp_path = _working_directory;

  // if the package cannot be located, bomb out
  if( !path_exists( pkg_root_image_path ) ) {
    std::cerr << "ERROR(client.cpp): Unable to locate the package path." << std::endl;
    terminate();
    return false;
  }

  // if the temporary directory does not exist (should have been created in 
  // init), bomb out
  if( !path_exists( pkg_root_tmp_path ) ) {
    std::cerr << "ERROR(client.cpp): The temporary working directory was not available." << std::endl;
    terminate();
    return false;
  }

  // main loop.  reiterate until either an error occurs or the user chooses not
  // to recycle the program 
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
    // --
    scenario->print();
    // --

    // create an experiment
    experiment = Reveal::Core::experiment_ptr( new Reveal::Core::experiment_c( scenario ) );

    // user selects any engine specific parameters
    if( !_simulator->ui_select_configuration( scenario, experiment ) ) {
      // TODO: error handling.
      // if false, no choice but to bomb with unrecognized/unrecoverable
    }

    prompt_time_step( scenario, experiment );
    prompt_trials_to_ignore( scenario, experiment );

    std::stringstream ss;
    ss << "The experiment is configured to use a time step of " << experiment->time_step << " seconds and will ignore the next " << experiment->intermediate_trials_to_ignore << " trials after being reset by new trial state.";
    Reveal::Core::console_c::printline( ss );

    // user selects any package specific parameters
    if( !_simulator->ui_select_tuning() ) {
      // TODO: error handling.
      // if false, no choice but to bomb with unrecognized/unrecoverable
    }

    // request experiment
    error = request_experiment( _auth, scenario, experiment );
    if( error != ERROR_NONE ) {
      printf( "ERROR: client failed to receive experiment\n" );
      // TODO: error handling
      // TODO: convert error reporting over to Core::error_c
    }

    experiment->print();
    printf( "eps[%1.24f]\n", experiment->epsilon );

    std::string pkg_image_path, pkg_tmp_path;
    std::string pkg_source_path, pkg_build_path;

    // create a path to the package image
    pkg_image_path = combine_path( pkg_root_image_path, scenario->package_id );
    // create a path to the package temporary directory
    pkg_tmp_path = combine_path( pkg_root_tmp_path, scenario->package_id );
    // get the package temporary directory
    if( !get_directory( pkg_tmp_path ) ) {
      terminate();
      return false;
    }
    // extend those paths to find the shared package component
    pkg_image_path = combine_path( pkg_image_path, "shared" );
    pkg_tmp_path = combine_path( pkg_tmp_path, "shared" );
    // get the temporary shared package component
    if( !get_directory( pkg_tmp_path ) ) {
      terminate();
      return false;
    }

    // clean the temporary package path to remove any artifacts
    if( !clean_directory( pkg_tmp_path ) ) {
      terminate();
      return false;
    }

    // copy the package image the temporary path
    if( !copy_directory( pkg_image_path, pkg_tmp_path ) ) {
      terminate();
      return false;
    }
    // set the source and build paths to the temporary path
    pkg_source_path = pkg_tmp_path;
    pkg_build_path = combine_path( pkg_source_path, "build" );
   
    if( path_exists( pkg_build_path ) ) {
      // if there are build artifacts (copied from the image) clean them
      if( !clean_directory( pkg_build_path ) ) {
        terminate();
        return false;
      } 
    } else {
      // if the build path did not exist, then create it
      if( !get_directory( pkg_build_path ) ) {
        terminate();
        return false;
      }
    }

    // build the package in the temporary path
    if( !_simulator->build_package( pkg_source_path, pkg_build_path ) ) {
      printf( "Exiting\n" );
      exit( 1 );
    }

    // execute the simulator using the build products in the temporary path
    bool result = _simulator->execute( _auth, scenario, experiment );
    if( !result ) {
      // TODO: determine how to recover
    }

    // prompt the user as to whether or not to rerun reveal
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
