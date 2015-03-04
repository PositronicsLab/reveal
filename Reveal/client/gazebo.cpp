#include "Reveal/client/gazebo.h"

#include <string>
#include <vector>
#include <sstream>

#include "Reveal/core/ipc.h"
#include "Reveal/client/system.h"

//-----------------------------------------------------------------------------
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include "Reveal/core/pointers.h"
#include "Reveal/core/ipc.h"
#include "Reveal/core/authorization.h"
#include "Reveal/core/user.h"
#include "Reveal/core/digest.h"
#include "Reveal/core/experiment.h"
#include "Reveal/core/scenario.h"
#include "Reveal/core/trial.h"
#include "Reveal/core/solution.h"
#include "Reveal/core/transport_exchange.h"

#include "Reveal/client/client.h"

//-----------------------------------------------------------------------------
Reveal::Core::pipe_ptr gzexit_write;

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Client {


//-----------------------------------------------------------------------------
gazebo_c::gazebo_c( void ) { 

}
//-----------------------------------------------------------------------------
gazebo_c::gazebo_c( Reveal::Client::client_ptr client ) { 
  _client = client;
}
//-----------------------------------------------------------------------------
gazebo_c::~gazebo_c( void ) { 

}

//-----------------------------------------------------------------------------
std::vector< std::string > gazebo_c::environment_keys( void ) {
  std::vector< std::string > enkeys;
  enkeys.push_back( "HOME" );
  enkeys.push_back( "LD_LIBRARY_PATH" );
  enkeys.push_back( "OGRE_RESOURCE_PATH" );
  enkeys.push_back( "GAZEBO_MASTER_URI" );
  enkeys.push_back( "GAZEBO_MODEL_DATABASE_URI" );
  enkeys.push_back( "GAZEBO_RESOURCE_PATH" );
  enkeys.push_back( "GAZEBO_PLUGIN_PATH" );
  enkeys.push_back( "GAZEBO_MODEL_PATH" );
  return enkeys;
}

//-----------------------------------------------------------------------------
bool gazebo_c::execute( void ) {
  std::string dynamics_param;
  switch( dynamics ) {
  case DYNAMICS_ODE:
    dynamics_param = "ode";         break;
  case DYNAMICS_BULLET:
    dynamics_param = "bullet";      break;
  case DYNAMICS_DART:
    dynamics_param = "dart";        break;
  case DYNAMICS_SIMBODY:
    dynamics_param = "simbody";     break;
  }

  printf( "dyanamics: %s\n", dynamics_param.c_str() );

  // build the make command line arguments array
  std::vector<std::string> arg_strings;
  arg_strings.push_back( "reveal-gzserver" );
  //arg_strings.push_back( "-u" );  // paused
  arg_strings.push_back( "-e" );
  arg_strings.push_back( dynamics_param );
  arg_strings.push_back( "--verbose" ); 
  arg_strings.push_back( world_path );
  char* const* exec_argv = param_array( arg_strings );

  // build the enviroment variables array
  std::vector<std::string> env_strings = system_environment_vars();
  // overrides
  for( std::vector<std::string>::iterator it = env_strings.begin(); it != env_strings.end(); it++ ) {
     std::string search_val;
     std::size_t found;
     search_val = "GAZEBO_PLUGIN_PATH";
     found = it->find( search_val );
     if( found != std::string::npos ) { 
       //if( found == 0 ) {
         //printf( "l_substr: %s\n", it->substr(0,search_val.size()).c_str() );
         //printf( "r_substr: %s\n", it->substr(search_val.size()+1).c_str() );
         std::string l,r;
         l = it->substr( 0,search_val.size() ).c_str();
         r = it->substr( search_val.size()+1 ).c_str();
         std::stringstream ss;
         ss << search_val << '=';
         ss << plugin_path << ":";
         ss << r;
         *it = ss.str();
       //}
     }
     search_val = "GAZEBO_MODEL_PATH";
     found = it->find( search_val );
     if( found != std::string::npos ) { 
       //if( found == 0 ) {
         std::string l,r;
         l = it->substr( 0, search_val.size() ).c_str();
         r = it->substr( search_val.size() + 1 ).c_str();
         std::stringstream ss;
         ss << search_val << '=';
         ss << model_path << ":";
         ss << r;
         *it = ss.str();
       //}
     }
  }

  //print_vector_of_strings( env_strings );

  char* const* exec_envars = param_array( env_strings );

  printf( "gazebo_executable: %s\n", GZSERVER_BIN );

  execve( GZSERVER_BIN, exec_argv, exec_envars );

  perror( "execve" );
  exit( EXIT_FAILURE );
}

//-----------------------------------------------------------------------------
gazebo_c::dynamics_e gazebo_c::prompt_dynamics( void ) {

  printf( "- Dynamics Menu -\n" );
  printf( "0: ODE\n" );
  printf( "1: Bullet\n" );
  printf( "2: DART\n" );
  printf( "3: Simbody\n" );

  unsigned choice = 0;
  unsigned range = 3;
  int result;

  do {
    printf( "Select a dynamics engine: " );
    result = scanf( "%u", &choice );
  
    if( !result || choice > range ) 
      printf( "ERROR: Invalid Input. Enter a value in the range [0,%u]\n", range );
  } while( !result || choice > range );

  if( choice == 0 ) 
    return DYNAMICS_ODE;
  else if( choice == 1 ) 
    return DYNAMICS_BULLET;
  else if( choice == 2 ) 
    return DYNAMICS_DART;
  else if( choice == 3 ) 
    return DYNAMICS_SIMBODY;

  // return a default if fall through for safety
  return DYNAMICS_ODE;
}

//-----------------------------------------------------------------------------
void gazebo_c::select_dynamics( void ) {
  dynamics = prompt_dynamics();  
}

//-----------------------------------------------------------------------------
// NOTE: probably a misnomer as of now.  experiment may be better described
// as run, execute, or simulate
bool gazebo_c::experiment( Reveal::Core::authorization_ptr auth ) {
  std::string source_path = package_root_path() + "industrial_arm/shared";
  std::string build_path = package_root_path() + "industrial_arm/shared/build";

  world_path = build_path + '/' + "reveal.world";
  plugin_path = build_path;
  model_path = build_path;

  const bool USE_SDF = true;
  if( USE_SDF )
    model_path = build_path + "/sdf/models";
  else
    model_path = build_path + "/urdf/models";

  Reveal::Core::digest_ptr digest;
  Reveal::Core::scenario_ptr scenario;
  Reveal::Core::experiment_ptr experiment;
  Reveal::Core::trial_ptr trial;
  Reveal::Core::solution_ptr solution;
  Reveal::Core::transport_exchange_c ex;
  std::string msg;

  Reveal::Client::client_c::error_e client_error;

  // Digest
  printf( "Fetching Scenario Digest\n" );

  // request the digest
  _client->request_digest( auth, digest );
  // TODO: error handling

  // print digest menu
  print_digest_menu( digest );

  // user selects scenario
  unsigned scenario_choice = prompt_digest( digest );

  // fetch scenario
  scenario = digest->get_scenario( scenario_choice );

  // request experiment
  client_error = _client->request_experiment( auth, scenario, experiment );
  if( client_error != Reveal::Client::client_c::ERROR_NONE ) {
    printf( "ERROR: client failed to receive experiment\n" );
  } else {
    printf( "SUCCESS: client received: experiment[%s]\n", experiment->experiment_id.c_str() );
    experiment->print();
    //printf( "experiment->number_of_trials: %d\n", experiment->number_of_trials );
    scenario->print();
  }

  // print dynamics menu
  // user selects dynamics
  select_dynamics();
  //Reveal::Client::gazebo_c::dynamics_e dynamics = gz.prompt_dynamics();

  // print tuning menu
  if( prompt_yes_no( "Would you like to tune the experiment (Y/N)?", true ) )
    print_tuning_menu();

  // build scenario
  bool cmake_result = cmake_package( source_path, build_path );
  if( !cmake_result ) {
    printf( "ERROR: Failed to configure make for experiment\nExiting\n" );
    exit( 1 );
  } else {
    printf( "Built controller\n" );
  }
  //exit( 0 );

  bool make_result = make_package( build_path );

  if( !make_result ) {
    printf( "ERROR: Failed to build experiment\nExiting\n" );
    exit( 1 );
  } else {
    printf( "Built controller\n" );
  }

  printf( "launching gzserver\n" );

  unsigned port = PORT + 1;

  // open gzconnection and gzsignal
  // TODO: use correct constructors
  void* context = _client->_connection.context();
  ipc = Reveal::Core::pipe_ptr( new Reveal::Core::pipe_c( MONITOR_PORT, context ) );
  exit_read = Reveal::Core::pipe_ptr( new Reveal::Core::pipe_c( "gzsignal", true, context ) );
  exit_write = Reveal::Core::pipe_ptr( new Reveal::Core::pipe_c( "gzsignal", false, context ) );

  if( ipc->open() != Reveal::Core::pipe_c::ERROR_NONE ) {
    // TODO: Error handling
  }
  if( exit_read->open() != Reveal::Core::pipe_c::ERROR_NONE ) {
    // TODO: Error handling
  }
  if( exit_write->open() != Reveal::Core::pipe_c::ERROR_NONE ) {
    // TODO: Error handling
  }
  gzexit_write = exit_write;

  printf( "ipc pipes open\n" );

  // install sighandler to detect when the gazebo finishes
  struct sigaction action;
  memset( &action, 0, sizeof(struct sigaction) );
  action.sa_handler = gzexit_sighandler;
  sigaction( SIGCHLD, &action, NULL );

  printf( "signal handler installed\n" );

  pid_t pid = fork();
  if( pid < 0 ) {
    // error
  }

  if( pid == 0 ) {
    // child process
    pid = getpid();

    std::string dynamics_param;
    switch( dynamics ) {
    case Reveal::Client::gazebo_c::DYNAMICS_ODE:
      dynamics_param = "ode";         break;
    case Reveal::Client::gazebo_c::DYNAMICS_BULLET:
      dynamics_param = "bullet";      break;
    case Reveal::Client::gazebo_c::DYNAMICS_DART:
      dynamics_param = "dart";        break;
    case Reveal::Client::gazebo_c::DYNAMICS_SIMBODY:
      dynamics_param = "simbody";     break;
    }

    printf( "dyanamics: %s\n", dynamics_param.c_str() );

    // build the make command line arguments array
    std::vector<std::string> arg_strings;
    arg_strings.push_back( "reveal-gzserver" );
    //arg_strings.push_back( "-u" );  // paused
    arg_strings.push_back( "-e" );
    arg_strings.push_back( dynamics_param );
    arg_strings.push_back( "--verbose" ); 
    arg_strings.push_back( world_path );
    char* const* exec_argv = param_array( arg_strings );

    // build the enviroment variables array
    std::vector<std::string> env_strings = system_environment_vars();
    // overrides
    for( std::vector<std::string>::iterator it = env_strings.begin(); it != env_strings.end(); it++ ) {
       std::string search_val;
       std::size_t found;
       search_val = "GAZEBO_PLUGIN_PATH";
       found = it->find( search_val );
       if( found != std::string::npos ) { 
         //if( found == 0 ) {
           //printf( "l_substr: %s\n", it->substr(0,search_val.size()).c_str() );
           //printf( "r_substr: %s\n", it->substr(search_val.size()+1).c_str() );
           std::string l,r;
           l = it->substr( 0,search_val.size() ).c_str();
           r = it->substr( search_val.size()+1 ).c_str();
           std::stringstream ss;
           ss << search_val << '=';
           ss << plugin_path << ":";
           ss << r;
           *it = ss.str();
         //}
       }
       search_val = "GAZEBO_MODEL_PATH";
       found = it->find( search_val );
       if( found != std::string::npos ) { 
         //if( found == 0 ) {
           std::string l,r;
           l = it->substr( 0, search_val.size() ).c_str();
           r = it->substr( search_val.size() + 1 ).c_str();
           std::stringstream ss;
           ss << search_val << '=';
           ss << model_path << ":";
           ss << r;
           *it = ss.str();
         //}
       }
    }

    //print_vector_of_strings( env_strings );

    char* const* exec_envars = param_array( env_strings );

    printf( "gazebo_executable: %s\n", GZSERVER_BIN );

    execve( GZSERVER_BIN, exec_argv, exec_envars );

    perror( "execve" );
    exit( EXIT_FAILURE );

  } else {
    // parent process

    // yield for a short while to give gazebo time to spin up
    printf( "reveal client yielding\n" );
    sleep( 1 );

    // write experiment
    ex.build_server_experiment( msg, auth, scenario, experiment );

    printf( "writing experiment to gazebo\n" );
    //printf( "experiment->number_of_trials: %d\n", experiment->number_of_trials );
    if( ipc->write( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
      // TODO: trap and recover
    }

    unsigned trial_index = 0;

    printf( "Starting Main Loop\n" );
    while( true ) {

      if( trial_index < scenario->trials ) {
        // request trial from revealserver
        // create a trial
        trial = scenario->get_trial( trial_index );

        _client->request_trial( auth, experiment, trial );
        // TODO: error handling

        // write trial to gzipc
        ex.build_server_trial( msg, auth, experiment, trial );

        printf( "writing experiment to gazebo\n" );
        //printf( "experiment->number_of_trials: %d\n", experiment->number_of_trials );
        if( ipc->write( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
          // TODO: trap and recover
        }
      }

      zmq_pollitem_t channels[2];
      channels[0].socket = ipc->socket();
      channels[0].fd = 0;
      channels[0].events = ZMQ_POLLIN;
      channels[0].revents = 0;

      channels[1].socket = exit_read->socket();
      channels[1].fd = 0;
      channels[1].events = ZMQ_POLLIN;
      channels[1].revents = 0;

      int rc = zmq_poll( channels, 2, -1);
      //assert (rc >= 0); // Returned events will be stored in items[].revents 

      if( channels[0].revents & ZMQ_POLLIN ) {

        // read solution from message
        if( ipc->read( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
          // TODO: trap and recover
        }

        // forward solution to revealserver
        ex.parse_client_solution( msg, auth, experiment, solution );

        // submit the solution to the server
        _client->submit_solution( auth, experiment, solution );

        // increment trial
        trial_index++;
      }

      if( channels[1].revents & ZMQ_POLLIN ) {
        // received a signal that gazebo has exited therefore kill the loop
        printf( "Detected Gazebo Exit\n" );
        break;
      }
    }

    // uninstall sighandler
    action.sa_handler = SIG_DFL;
    sigaction( SIGCHLD, &action, NULL );

    //close gzconnection and gzsignal
    ipc->close();
    exit_read->close();
    exit_write->close();
  }

  return true;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
bool gazebo_exit_handler_c::_quit = false;

gazebo_exit_handler_c::gazebo_exit_handler_c( void ) { _quit = false; }
gazebo_exit_handler_c::~gazebo_exit_handler_c( void ) { }

void gazebo_exit_handler_c::handler( int signum ) {
  _quit = true; 
}

bool gazebo_exit_handler_c::quit( void ) { return _quit; }
void gazebo_exit_handler_c::set( bool quit_ ) { _quit = quit_; }
void gazebo_exit_handler_c::reset( void ) { _quit = false; }

void gazebo_exit_handler_c::install( void ) {   
  struct sigaction action;
  memset( &action, 0, sizeof(struct sigaction) );
  action.sa_handler = handler;
  sigaction( SIGCHLD, &action, NULL );
}
void gazebo_exit_handler_c::uninstall( void ) {
  struct sigaction action;
  action.sa_handler = SIG_DFL;
  sigaction( SIGCHLD, &action, NULL );
}

void gazebo_exit_handler_c::trip( void ) {
  _quit = true;
}
*/

//-----------------------------------------------------------------------------
void gazebo_c::print_tuning_menu( void ) {
  printf( "- Tuning Menu -\n" );
  printf( "TODO\n" );
}



//-----------------------------------------------------------------------------
} // Client
//-----------------------------------------------------------------------------
} // Reveal
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// TODO: determine best location for this method.  May end up with a whole 
// package system in the end anyway, so this method would be best served in
// that set of class and function definitions.
std::string package_root_path( void ) {
  return PACKAGE_ROOT_PATH;
}


//-----------------------------------------------------------------------------
// Note: only one instance of gzserver can run at a time at present, so having
// a static function tied to a global variable for the signal handler is not
// a major issue as of right now.
void gzexit_sighandler( int signum ) {
  // write to gzexit_write

  std::string msg = "true";
  gzexit_write->write( msg );
}

//-----------------------------------------------------------------------------
