#include "Reveal/sim/gazebo/gazebo.h"

#include <signal.h>

#include "Reveal/core/console.h"
#include "Reveal/core/error.h"
#include "Reveal/core/system.h"

#include "Reveal/core/transport_exchange.h"
#include "Reveal/core/scenario.h"

#include <sstream>

/*
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
*/

//-----------------------------------------------------------------------------
Reveal::Core::pipe_ptr gzexit_write;

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Sim {

//-----------------------------------------------------------------------------
gazebo_c::gazebo_c( void ) {
  _request_trial = NULL;
  _submit_solution = NULL;
  _ipc_context = NULL;
}

//-----------------------------------------------------------------------------
gazebo_c::gazebo_c( request_trial_f request_trial, submit_solution_f submit_solution, void* ipc_context ) {
  _request_trial = request_trial;
  _submit_solution = submit_solution;
  _ipc_context = ipc_context;
}

//-----------------------------------------------------------------------------
gazebo_c::~gazebo_c( void ) { 

}

//-----------------------------------------------------------------------------
void gazebo_c::set_request_trial( request_trial_f request_trial ) {
  _request_trial = request_trial;
}

//-----------------------------------------------------------------------------
void gazebo_c::set_submit_solution( submit_solution_f submit_solution ) {
  _submit_solution = submit_solution;
}
//-----------------------------------------------------------------------------
void gazebo_c::set_ipc_context( void* context ) {
  _ipc_context = context;
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
gazebo_c::dynamics_e gazebo_c::prompt_dynamics( void ) {

  std::vector< std::string > list;
  list.push_back( "ODE" );
  list.push_back( "Bullet" );
  list.push_back( "DART" );
  list.push_back( "Simbody" );
  
  unsigned choice = Reveal::Core::console_c::menu( "--Dynamics Menu--", "Select a dynamics engine", list );

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
bool gazebo_c::ui_select_configuration( void ) {
  _dynamics = prompt_dynamics();

  return true; 
}

//-----------------------------------------------------------------------------
bool gazebo_c::ui_select_tuning( void ) {
  if( Reveal::Core::console_c::prompt_yes_no( "Would you like to tune the experiment (Y/N)?" ) )
    print_tuning_menu();

  return true; 
}

//-----------------------------------------------------------------------------
bool gazebo_c::build_package( std::string src_path, std::string build_path ) {

  //_world_path = build_path + '/' + "reveal.world";
  _plugin_path = build_path;
  _model_path = build_path;

  const bool USE_SDF = true;
  if( USE_SDF )
    _model_path = build_path + "/sdf/models";
  else
    _model_path = build_path + "/urdf/models";

  // build package
  _package = Reveal::Core::package_ptr( new Reveal::Core::package_c( src_path, build_path ) );

  Reveal::Core::xml_element_ptr manifest = _package->read( "Gazebo" );
  // parse xml
  std::vector<std::string> build_products;
  Reveal::Core::xml_element_ptr element;
  Reveal::Core::xml_attribute_ptr attribute;
  for( unsigned i = 0; i < manifest->elements(); i++ ) {
    element = manifest->element( i );
    if( element->get_name() == "Product" ) {
      attribute = element->attribute( "file" );
      if( !attribute ) continue;  // malformed xml.  probably a bigger error
      // otherwise
      build_products.push_back( attribute->get_value() );
    } else if( element->get_name() == "World" ) {
      attribute = element->attribute( "file" );
      if( !attribute ) continue;  // malformed xml.  probably a bigger error
      // otherwise
      _world_path = build_path + '/' + attribute->get_value();
    }
  }

  // configure package
  bool cmake_result = _package->configure();
  if( !cmake_result ) {
    printf( "ERROR: Failed to configure make for experimental package\nExiting\n" );
    return false;
  } else {
    printf( "Package configuration succeeded\n" );
  }

  // build package
  bool make_result = _package->make( build_products );
  if( !make_result ) {
    printf( "ERROR: Failed to build experimental package\nExiting\n" );
    return false;
  } else {
    printf( "Built package\n" );
  }
  return true;
}

//-----------------------------------------------------------------------------
bool gazebo_c::execute( Reveal::Core::authorization_ptr auth, Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr experiment ) {

  unsigned monitor_port;
  Reveal::Core::system_c system( Reveal::Core::system_c::CLIENT );
  if( !system.open() ) return false;
  monitor_port = system.monitor_port();

  // sanity check
  if( _request_trial == NULL || _submit_solution == NULL || _ipc_context == NULL ) {
    // the simulator has not been properly set up and execute cannot proceed!
    // TODO: error handling
  }

  Reveal::Core::transport_exchange_c exchg;
  Reveal::Core::trial_ptr trial;
  Reveal::Core::solution_ptr solution;
  std::string msg;

  printf( "launching gzserver\n" );

  // open ipc channels between this instance and the gazebo process and 
  // for signal handling
  _ipc = Reveal::Core::pipe_ptr( new Reveal::Core::pipe_c( system.monitor_port(), _ipc_context ) );
  _exit_read = Reveal::Core::pipe_ptr( new Reveal::Core::pipe_c( "gzsignal", true, _ipc_context ) );
  //_exit_write = Reveal::Core::pipe_ptr( new Reveal::Core::pipe_c( "gzsignal", false, _ipc_context ) );

  if( _ipc->open() != Reveal::Core::pipe_c::ERROR_NONE ) {
    // TODO: Error handling
    Reveal::Core::error_c::printline( "ERROR: failed to open ipc" );
  }
  if( _exit_read->open() != Reveal::Core::pipe_c::ERROR_NONE ) {
    // TODO: Error handling
    Reveal::Core::error_c::printline( "ERROR: failed to open exit_read" );
  }
/*
  if( _exit_write->open() != Reveal::Core::pipe_c::ERROR_NONE ) {
    // TODO: Error handling
    Reveal::Core::error_c::printline( "ERROR: failed to open exit_write" );
  }
//  gzexit_write = _exit_write;
*/
  printf( "ipc pipes open\n" );

  // install sighandler to detect when gazebo finishes
  // TODO: make sighandler class more compatible with using a member function
  struct sigaction action;
  memset( &action, 0, sizeof(struct sigaction) );
  action.sa_handler = exit_sighandler;
  sigaction( SIGCHLD, &action, NULL );

  printf( "signal handler installed\n" );

  pid_t pid = fork();
  if( pid < 0 ) {
    // fork failed
    // TODO: error handling
  }

  if( pid == 0 ) {
    // child process
    pid = getpid();

    std::string dynamics_param;
    switch( _dynamics ) {
    case Reveal::Sim::gazebo_c::DYNAMICS_ODE:
      dynamics_param = "ode";         break;
    case Reveal::Sim::gazebo_c::DYNAMICS_BULLET:
      dynamics_param = "bullet";      break;
    case Reveal::Sim::gazebo_c::DYNAMICS_DART:
      dynamics_param = "dart";        break;
    case Reveal::Sim::gazebo_c::DYNAMICS_SIMBODY:
      dynamics_param = "simbody";     break;
    }

    printf( "dynamics: %s\n", dynamics_param.c_str() );

    // build the make command line arguments array
    std::vector<std::string> arg_strings;
    arg_strings.push_back( "reveal-gzserver" );
    //arg_strings.push_back( "-u" );  // paused    TODO: remark after debugging
    arg_strings.push_back( "-e" );
    arg_strings.push_back( dynamics_param );
    arg_strings.push_back( "--verbose" ); 
    arg_strings.push_back( _world_path );
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
          ss << _plugin_path << ":";
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
          ss << _model_path << ":";
          ss << r;
          *it = ss.str();
        //}
      }
    }

    //Reveal::Core::console_c::print( env_strings );

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
    exchg.build_server_experiment( msg, auth, scenario, experiment );

    printf( "writing experiment to gazebo\n" );
    //printf( "experiment->number_of_trials: %d\n", experiment->number_of_trials );
    if( _ipc->write( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
      // TODO: trap and recover
    }

    unsigned trial_index = 0;

    printf( "Starting Main Loop\n" );
    while( true ) {

      if( trial_index < scenario->trials ) {
        // request trial from revealserver
        // create a trial
        trial = scenario->get_trial( trial_index );

        bool result = _request_trial( auth, experiment, trial );
        if( !result ) {
          // TODO: error handling
          Reveal::Core::console_c::printline( "request_trial failed" );
        }

        //trial->print();

        // write trial to gzipc
        exchg.build_server_trial( msg, auth, experiment, trial );

        printf( "writing experiment to gazebo\n" );
        //printf( "experiment->number_of_trials: %d\n", experiment->number_of_trials );
        if( _ipc->write( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
          // TODO: trap and recover
        }
      }

      // TODO : encapsulate in ipc
      zmq_pollitem_t channels[2];
      channels[0].socket = _ipc->socket();
      channels[0].fd = 0;
      channels[0].events = ZMQ_POLLIN;
      channels[0].revents = 0;

      channels[1].socket = _exit_read->socket();
      channels[1].fd = 0;
      channels[1].events = ZMQ_POLLIN;
      channels[1].revents = 0;

      int rc = zmq_poll( channels, 2, -1);
      if( rc == -1 ) {
        if( errno == ETERM ) {
          // At least one member of channels refers to a socket whose context
          // was terminated
          printf( "ERROR: ETERM a socket was terminated\n" );
        } else if( errno == EFAULT ) {
          // The provided channels was NULL
          printf( "ERROR: EFAULT a provided channel was NULL\n" );
        } else if( errno == EINTR ) {
          // signal interrupted polling before events were available
          printf( "ERROR: EINTR signal interrupted polling\n" );
          break;  
          // Hack for now as _exit_read event is not triggered as before and
          // _exit_write->close() is locking up when channel simply ignored
        }
      }

      if( channels[0].revents & ZMQ_POLLIN ) {

        // read solution from message
        if( _ipc->read( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
          // TODO: trap and recover
        }

        // forward solution to revealserver
        exchg.parse_client_solution( msg, auth, experiment, solution );

        //solution->print();

        // submit the solution to the server
        bool result = _submit_solution( auth, experiment, solution );
        if( !result ) {
          // TODO: error handling
          Reveal::Core::console_c::printline( "submit_solution failed" );
        }

        // increment trial
        trial_index++;
      }

      if( channels[1].revents & ZMQ_POLLIN ) {
        // TODO: signal not being received for some reason after refactor
        // debug and then get rid of legacy experiment method after solved.

        // received a signal that gazebo has exited therefore kill the loop
        printf( "Detected Gazebo Exit\n" );
        break;
      }
    }

    // uninstall sighandler
    action.sa_handler = SIG_DFL;
    sigaction( SIGCHLD, &action, NULL );

    //close gzconnection and gzsignal
    _ipc->close();
    //_exit_write->close(); // 2nd half of hack in EINTR, disabled channel
    _exit_read->close();
  }

  return true;

}
//-----------------------------------------------------------------------------
// Note: only one instance of gzserver can run at a time at present, so having
// a static function tied to a global variable for the signal handler is not
// a major issue as of right now.
void gazebo_c::exit_sighandler( int signum ) {
  // write to gzexit_write

  printf( "exit sighandler called\n" );

  std::string msg = "true";
  //gzexit_write->write( msg );

  //assert( signum );  // to suppress compiler warning of unused variable
}

//-----------------------------------------------------------------------------
void gazebo_c::print_tuning_menu( void ) {
  printf( "- Tuning Menu -\n" );
  printf( "TODO\n" );
}

//-----------------------------------------------------------------------------
} // Sim
//-----------------------------------------------------------------------------
} // Reveal
//-----------------------------------------------------------------------------
