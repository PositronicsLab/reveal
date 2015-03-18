#include "Reveal/client/gazebo.h"

#include <string>
#include <vector>
#include <sstream>

#include "Reveal/core/ipc.h"
#include "Reveal/core/console.h"
#include "Reveal/core/error.h"
#include "Reveal/core/system.h"

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

#include "Reveal/client/package.h"

#include <gazebo/gazebo.hh>
#include <gazebo/common/common.hh>
#include <gazebo/physics/physics.hh>

//-----------------------------------------------------------------------------
Reveal::Core::pipe_ptr gzexit_write;

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Client {

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

  _world_path = build_path + '/' + "reveal.world";
  _plugin_path = build_path;
  _model_path = build_path;

  const bool USE_SDF = true;
  if( USE_SDF )
    _model_path = build_path + "/sdf/models";
  else
    _model_path = build_path + "/urdf/models";

  // build package
  _package = package_ptr( new package_c( src_path, build_path ) );

  // TODO : add robust error checking and handling
  bool read_result = _package->read();

  // configure package
  bool cmake_result = _package->configure();
  if( !cmake_result ) {
    printf( "ERROR: Failed to configure make for experimental package\nExiting\n" );
    return false;
  } else {
    printf( "Package configuration succeeded\n" );
  }

  // build package
  bool make_result = _package->make();
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
  _ipc = Reveal::Core::pipe_ptr( new Reveal::Core::pipe_c( MONITOR_PORT, _ipc_context ) );
  _exit_read = Reveal::Core::pipe_ptr( new Reveal::Core::pipe_c( "gzsignal", true, _ipc_context ) );
  _exit_write = Reveal::Core::pipe_ptr( new Reveal::Core::pipe_c( "gzsignal", false, _ipc_context ) );

  if( _ipc->open() != Reveal::Core::pipe_c::ERROR_NONE ) {
    // TODO: Error handling
    Reveal::Core::error_c::printline( "ERROR: failed to open ipc" );
  }
  if( _exit_read->open() != Reveal::Core::pipe_c::ERROR_NONE ) {
    // TODO: Error handling
    Reveal::Core::error_c::printline( "ERROR: failed to open exit_read" );
  }
  if( _exit_write->open() != Reveal::Core::pipe_c::ERROR_NONE ) {
    // TODO: Error handling
    Reveal::Core::error_c::printline( "ERROR: failed to open exit_write" );
  }
  gzexit_write = _exit_write;

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
    case Reveal::Client::gazebo_c::DYNAMICS_ODE:
      dynamics_param = "ode";         break;
    case Reveal::Client::gazebo_c::DYNAMICS_BULLET:
      dynamics_param = "bullet";      break;
    case Reveal::Client::gazebo_c::DYNAMICS_DART:
      dynamics_param = "dart";        break;
    case Reveal::Client::gazebo_c::DYNAMICS_SIMBODY:
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
        } else if( errno == EFAULT ) {
          // The provided channels was NULL
        } else if( errno == EINTR ) {
          // signal interrupted polling before events were available
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
    _exit_read->close();
    _exit_write->close();
  }

  return true;

}
/*
//-----------------------------------------------------------------------------
// NOTE: probably a misnomer as of now.  experiment may be better described
// as run, execute, or simulate
bool gazebo_c::experiment( Reveal::Core::authorization_ptr auth ) {
  std::string source_path = package_root_path() + "industrial_arm/shared";
  std::string build_path = package_root_path() + "industrial_arm/shared/build";

  _world_path = build_path + '/' + "reveal.world";
  _plugin_path = build_path;
  _model_path = build_path;

  const bool USE_SDF = true;
  if( USE_SDF )
    _model_path = build_path + "/sdf/models";
  else
    _model_path = build_path + "/urdf/models";

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

  // user selects scenario
  std::vector< std::string > scenario_list;
  for( unsigned i = 0; i < digest->scenarios(); i++ ) 
    scenario_list.push_back( digest->get_scenario( i )->description );

  unsigned scenario_choice = Reveal::Core::console_c::menu( "--Scenario Menu--", "Select a scenario", scenario_list );

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
  ui_select_configuration();

  // print tuning menu
  if( Reveal::Core::console_c::prompt_yes_no( "Would you like to tune the experiment (Y/N)?" ) )
    //print_tuning_menu();
    ui_select_tuning();

  // build package
  package_c package( source_path, build_path );

  // TODO : add robust error checking and handling
  bool read_result = package.read();


  bool cmake_result = package.configure();

  // build scenario
  if( !cmake_result ) {
    printf( "ERROR: Failed to configure make for experiment\nExiting\n" );
    exit( 1 );
  } else {
    printf( "Built controller\n" );
  }

  bool make_result = package.make();
  if( !make_result ) {
    printf( "ERROR: Failed to build experiment\nExiting\n" );
    exit( 1 );
  } else {
    printf( "Built controller\n" );
  }

  printf( "launching gzserver\n" );

  //unsigned port = PORT + 1;

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
  // TODO: make sighandler class more compatible with using a member function
  struct sigaction action;
  memset( &action, 0, sizeof(struct sigaction) );
  action.sa_handler = exit_sighandler;
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

    printf( "dynamics: %s\n", dynamics_param.c_str() );

    // build the make command line arguments array
    std::vector<std::string> arg_strings;
    arg_strings.push_back( "reveal-gzserver" );
    //arg_strings.push_back( "-u" );  // paused
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

      // TODO : encapsulate in ipc
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
      if( rc == -1 ) {
        if( errno == ETERM ) {
          // At least one member of channels refers to a socket whose context
          // was terminated
        } else if( errno == EFAULT ) {
          // The provided channels was NULL
        } else if( errno == EINTR ) {
          // signal interrupted polling before events were available
        }
      }

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
*/
//-----------------------------------------------------------------------------
// Note: only one instance of gzserver can run at a time at present, so having
// a static function tied to a global variable for the signal handler is not
// a major issue as of right now.
void gazebo_c::exit_sighandler( int signum ) {
  // write to gzexit_write

  printf( "exit sighandler called\n" );

  std::string msg = "true";
  gzexit_write->write( msg );

  assert( signum );  // to suppress compiler warning of unused variable
}

//-----------------------------------------------------------------------------
void gazebo_c::print_tuning_menu( void ) {
  printf( "- Tuning Menu -\n" );
  printf( "TODO\n" );
}

/*
//-----------------------------------------------------------------------------
bool gazebo_c::write_scenario( void ) {

}
*/
//-----------------------------------------------------------------------------
bool gazebo_c::write_model( Reveal::Core::model_ptr model, gazebo::physics::WorldPtr world ) {

  gazebo::physics::ModelPtr gzmodel = world->GetModel( model->id );
  if( !gzmodel ) return false;

  for( unsigned j = 0; j < model->links.size(); j++ ) {
    Reveal::Core::link_ptr link = model->links[j];

    gazebo::physics::LinkPtr gzlink = gzmodel->GetLink( link->id );
    // TODO : refactor assert into something more flexible and valid
    assert( gzlink );

    // map the reveal link state parameters to the gazebo link
    gzlink->SetWorldPose( pose( link->state ) );
    gzlink->SetLinearVel( linear_velocity( link->state ) );
    gzlink->SetAngularVel( angular_velocity( link->state ) );
  }
  for( unsigned j = 0; j < model->joints.size(); j++ ) {
    Reveal::Core::joint_ptr joint = model->joints[j];

    gazebo::physics::JointPtr gzjoint = gzmodel->GetJoint( joint->id );
    // TODO : refactor assert into something more flexible and valid
    assert( gzjoint );

    // NOTE : GetAngleCount is not implemented for Bullet or ODE
    //unsigned max_k = gzjoint->GetAngleCount();
    //for( unsigned k = 0; k < max_k; k++ )
    //  gzjoint->SetForce( k, joint->control[k] );

    // Bullet and ODE only use 1 DOF?
    gzjoint->SetForce( 0, joint->control[0] ); 
  }
  return true;
}

//-----------------------------------------------------------------------------
bool gazebo_c::write_trial( Reveal::Core::trial_ptr trial, gazebo::physics::WorldPtr world ) {

  world->SetSimTime( trial->t );
  world->GetPhysicsEngine()->SetMaxStepSize( trial->dt );

  for( unsigned i = 0; i < trial->models.size(); i++ ) {
    Reveal::Core::model_ptr model = trial->models[i];

    bool result = write_model( model, world );
    if( !result ) {
      // TODO : handle and recover.  This branch implies that the model does
      // not exist in the gazebo world for some reason.
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
// TODO : could be improved by actually returning an error code or writing to
// std::err
// returns an empty pointer if it fails to find the model in the gazebo world
Reveal::Core::model_ptr gazebo_c::read_model( gazebo::physics::WorldPtr world, std::string model_name, std::map<std::string,double> control ) {

  Reveal::Core::model_ptr rvlmodel;
  // attempt to find the model in the gazebo world
  gazebo::physics::ModelPtr gzmodel = world->GetModel( model_name );

  // if gazebo could not find the model, return a reveal model null reference
  if( !gzmodel ) return rvlmodel;

  // otherwise, start mapping the model
  rvlmodel = Reveal::Core::model_ptr( new Reveal::Core::model_c() );
  rvlmodel->id = model_name;

  // get all the links in the gazebo model and iterate through them
  gazebo::physics::Link_V gzlinks = gzmodel->GetLinks();
  for( unsigned i = 0; i < gzlinks.size(); i++ ) {

    // create a new reveal link and assign the correlating properties to it
    Reveal::Core::link_ptr link = Reveal::Core::link_ptr( new Reveal::Core::link_c() );
    link->id = gzlinks[i]->GetName();

    // map the gazebo link state parameters into the reveal link
    pose( gzlinks[i]->GetWorldPose(), link->state );
    linear_velocity( gzlinks[i]->GetWorldLinearVel(), link->state );
    angular_velocity( gzlinks[i]->GetWorldAngularVel(), link->state );

    // insert the link into the reveal model
    rvlmodel->insert( link );
  }

  // get all the joints in the gazebo model and iterate through them
  gazebo::physics::Joint_V gzjoints = gzmodel->GetJoints();
  for( unsigned i = 0; i < gzjoints.size(); i++ ) {

    // read the joint identifier from gazebo
    std::string id = gzjoints[i]->GetName();
    // assume it is mapped
    bool mapped = true;
    // assume a zero control
    double u = 0;
    try{
      // try to get the value stored in the map
      u = control.at( id );
    } catch( std::exception ex ) {
      // if the map failed to return a reference, then the joint is not mapped
      mapped = false;
    }

    // skip this joint if it is not mapped
    if( !mapped ) continue;

    // otherwise, create a joint and assign it the name and control
    Reveal::Core::joint_ptr joint = Reveal::Core::joint_ptr( new Reveal::Core::joint_c() );
    joint->id = gzjoints[i]->GetName();
    // TODO : Note this only currently supports single axis joints
    joint->control[0] = u;
  
    // insert the joint into the reveal model
    rvlmodel->insert( joint );
  }

  // return the reveal model
  return rvlmodel;
}

//-----------------------------------------------------------------------------
Reveal::Core::solution_ptr gazebo_c::read_model_solution( gazebo::physics::WorldPtr world, std::vector<std::string> model_list, std::string scenario_id, unsigned trial_id ) {

  // create a model solution
  Reveal::Core::solution_ptr solution = Reveal::Core::solution_ptr( new Reveal::Core::solution_c( Reveal::Core::solution_c::MODEL ) );

  // map required parameters
  solution->scenario_id = scenario_id;
  solution->trial_id = trial_id;
  solution->t = sim_time( world );
  solution->dt = step_size( world );

  // map the states of all desired models
  for( unsigned i = 0; i < model_list.size(); i++ ) {
    std::map<std::string,double> null_controls;
    Reveal::Core::model_ptr model = read_model( world, model_list[i], null_controls );

    if( model ) solution->models.push_back( model );
  }
  return solution;
}

//-----------------------------------------------------------------------------
Reveal::Core::solution_ptr gazebo_c::read_client_solution( gazebo::physics::WorldPtr world, std::vector<std::string> model_list, std::string scenario_id, unsigned trial_id ) {

  // create a client solution
  Reveal::Core::solution_ptr solution = Reveal::Core::solution_ptr( new Reveal::Core::solution_c( Reveal::Core::solution_c::CLIENT ) );

  // map required parameters
  solution->scenario_id = scenario_id;
  solution->trial_id = trial_id;
  solution->t = sim_time( world );
  solution->dt = step_size( world );

  // map the states of all desired models
  for( unsigned i = 0; i < model_list.size(); i++ ) {
    std::map<std::string,double> null_controls;
    Reveal::Core::model_ptr model = read_model( world, model_list[i], null_controls );

    if( model ) solution->models.push_back( model );
  }
  return solution;
}

//-----------------------------------------------------------------------------
gazebo::math::Vector3 gazebo_c::position( const Reveal::Core::state_c& state ) {
  return gazebo::math::Vector3( state.linear_x(), state.linear_y(), state.linear_z() );
}

//-----------------------------------------------------------------------------
gazebo::math::Quaternion gazebo_c::rotation( const Reveal::Core::state_c& state ) {
  return gazebo::math::Quaternion( state.angular_w(), state.angular_x(), state.angular_y(), state.angular_z() );
}

//-----------------------------------------------------------------------------
gazebo::math::Vector3 gazebo_c::linear_velocity( const Reveal::Core::state_c& state ) {
  return gazebo::math::Vector3( state.linear_dx(), state.linear_dy(), state.linear_dz() );
}

//-----------------------------------------------------------------------------
gazebo::math::Vector3 gazebo_c::angular_velocity( const Reveal::Core::state_c& state ) {
  return gazebo::math::Vector3( state.angular_dx(), state.angular_dy(), state.angular_dz() );
}
  
//-----------------------------------------------------------------------------
gazebo::math::Pose gazebo_c::pose( const Reveal::Core::state_c& state ) {
  return gazebo::math::Pose( position( state ), rotation( state ) );
}

//-----------------------------------------------------------------------------
void gazebo_c::position( const gazebo::math::Vector3 pos, Reveal::Core::state_c& state ) {
  state.linear_x( pos.x );
  state.linear_y( pos.y );
  state.linear_z( pos.z );
}

//-----------------------------------------------------------------------------
void gazebo_c::rotation( const gazebo::math::Quaternion rot, Reveal::Core::state_c& state ) {
  state.angular_x( rot.x );
  state.angular_y( rot.y );
  state.angular_z( rot.z );
  state.angular_w( rot.w );
}

//-----------------------------------------------------------------------------
void gazebo_c::linear_velocity( const gazebo::math::Vector3 lvel, Reveal::Core::state_c& state ) {
  state.linear_dx( lvel.x );
  state.linear_dy( lvel.y );
  state.linear_dz( lvel.z );
}

//-----------------------------------------------------------------------------
void gazebo_c::angular_velocity( const gazebo::math::Vector3 avel, Reveal::Core::state_c& state ) {
  state.angular_dx( avel.x );
  state.angular_dy( avel.y );
  state.angular_dz( avel.z );
}

//-----------------------------------------------------------------------------
void gazebo_c::pose( const gazebo::math::Pose pose, Reveal::Core::state_c& state ) {
  position( pose.pos, state );
  rotation( pose.rot, state );
}

//-----------------------------------------------------------------------------
double gazebo_c::sim_time( gazebo::physics::WorldPtr world ) {
  return world->GetSimTime().Double();
}

//-----------------------------------------------------------------------------
void gazebo_c::sim_time( double t, gazebo::physics::WorldPtr world ) {
  return world->SetSimTime( gazebo::common::Time( t ) );
}

//-----------------------------------------------------------------------------
double gazebo_c::step_size( gazebo::physics::WorldPtr world ) {
  return world->GetPhysicsEngine()->GetMaxStepSize();
}

//-----------------------------------------------------------------------------
double gazebo_c::real_time( gazebo::physics::WorldPtr world ) {
  return world->GetRealTime().Double();
}

//-----------------------------------------------------------------------------
void gazebo_c::reset( gazebo::physics::WorldPtr world ) {
  world->Reset();
}

//-----------------------------------------------------------------------------
} // Client
//-----------------------------------------------------------------------------
} // Reveal
//-----------------------------------------------------------------------------
