#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <sstream>
#include <vector>
#include <map>

#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <cstdio>

#include <Reveal/pointers.h>
#include <Reveal/ipc.h>
#include <Reveal/authorization.h>
#include <Reveal/user.h>
#include <Reveal/client.h>
#include <Reveal/digest.h>
#include <Reveal/experiment.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>
#include <Reveal/transport_exchange.h>

extern char **environ;

//-----------------------------------------------------------------------------
Reveal::Client::client_c client;

Reveal::Core::pipe_ptr gzipc; 
Reveal::Core::pipe_ptr gzexit_write;
Reveal::Core::pipe_ptr gzexit_read;

//-----------------------------------------------------------------------------
void test_identified_user( void ) {
  Reveal::Core::user_ptr user = Reveal::Core::user_ptr(new Reveal::Core::user_c() );
  user->id = "alice";

  Reveal::Core::authorization_ptr auth = Reveal::Core::authorization_ptr(new Reveal::Core::authorization_c() );
  auth->set_type( Reveal::Core::authorization_c::TYPE_IDENTIFIED );
  auth->set_user( user->id );

  Reveal::Client::client_c::error_e client_error;
  client_error = client.request_authorization( auth );
  if( client_error != Reveal::Client::client_c::ERROR_NONE ) {
    printf( "ERROR: client failed to gain identified authorization\n" );
  } else {
    printf( "SUCCESS: identified client gained authorization: session[%s]\n", auth->get_session().c_str() );
  }
}

//-----------------------------------------------------------------------------
void test_anonymous_user( void ) {

  Reveal::Core::authorization_ptr auth = Reveal::Core::authorization_ptr(new Reveal::Core::authorization_c() );
  auth->set_type( Reveal::Core::authorization_c::TYPE_ANONYMOUS );

  Reveal::Client::client_c::error_e client_error;
  client_error = client.request_authorization( auth );
  if( client_error != Reveal::Client::client_c::ERROR_NONE ) {
    printf( "ERROR: client failed to gain anonymous authorization\n" );
  } else {
    printf( "SUCCESS: anonymous client gained authorization: session[%s]\n", auth->get_session().c_str() );
  }
}

//-----------------------------------------------------------------------------
std::vector< std::string > gz_environment_keys( void ) {
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
std::vector< std::string > system_environment_vars( void ) {
  std::vector<std::string> ev;

  for (char **env = environ; *env; ++env) {
    ev.push_back( *env );
  }

  return ev;
}

//-----------------------------------------------------------------------------
void print_vector_of_strings( std::vector<std::string> v ) {
  for( std::vector<std::string>::iterator it = v.begin(); it != v.end(); it++ )
    printf( "%s\n", it->c_str() );
}

//-----------------------------------------------------------------------------
char* const* param_array( std::vector< std::string > params ) {

  const char** pa = (const char**)malloc( sizeof(char*) * params.size() + 1 );
  for( unsigned i = 0; i < params.size(); i++ ) {
    pa[i] = (const char*)params[i].c_str();
  }
  pa[ params.size() ] = NULL;

  return (char* const*) pa;
}

//-----------------------------------------------------------------------------
void print_param_array( char* const* a ) {
  for( char* const* ptr = (char* const*)a; *ptr != NULL; ptr++ )
    printf( "%s\n", *ptr );
}

//-----------------------------------------------------------------------------
bool child_quit;

void child_sighandler( int signum ) {
  // child process has terminated so flip the signal variable
  child_quit = true; 
}

//-----------------------------------------------------------------------------
void* make_worker( void* args ) {

  // allocate a return variable
  bool* result = new bool;

  // initialize the signal variable
  child_quit = false;
  // NOTE: do not need mutex or atomicity since only in parent context.

  // install sighandler to detect when the child process (make) finishes
  struct sigaction action;
  memset( &action, 0, sizeof(struct sigaction) );
  action.sa_handler = child_sighandler;
  sigaction( SIGCHLD, &action, NULL );

  // fork a child process
  pid_t pid = fork();
  if( pid < 0 ) {
    printf( "ERROR: bad pid\n" );
    // TODO : Improve error handling
  }

  // NOTE: paths need to be programatic
  std::string build_path = *((char* const*)args);

  std::string file_name = "libcontroller.so";
  std::string file_path = build_path + '/' + file_name;
  // NOTE: may be multiple libraries built by process

  if( pid == 0 ) {
    // CHILD PROCESS
    pid = getpid();

    // build the make command line arguments array
    std::vector<std::string> arg_strings;
    arg_strings.push_back( "make" );
    arg_strings.push_back( "-C" );
    arg_strings.push_back( build_path );
    char* const* exec_argv = param_array( arg_strings );

    // build the enviroment variables array
    std::vector<std::string> env_strings = system_environment_vars();
    char* const* exec_envars = param_array( env_strings );

    // execute make
    execve( MAKE_BIN, exec_argv, exec_envars );

    // trap and report any error.
    perror( "execve" );
    exit( EXIT_FAILURE );
    // NOTE: Unreachable on successful exec

    // FORKED PROCESS IS DEAD
  } else {
    // PARENT PROCESS
  
    // wait for signal the child has terminated
    while( !child_quit ) sleep(1);

    // check for the existence of the controller from make
    struct stat st;
    stat( file_path.c_str(), &st );
    if( S_ISREG( st.st_mode ) ) {
      // if the controller file exists then return true
      *result = true;
    } else {
      // otherwise return false
      *result = false;
    }
    // NOTE:  for platform independence, replace stat with boost filesystem
  }

  // uninstall sighandler
  action.sa_handler = SIG_DFL;
  sigaction( SIGCHLD, &action, NULL );

  return (void*) result;
}

//-----------------------------------------------------------------------------
void* cmake_worker( void* args ) {

  // allocate a return variable
  bool* result = new bool;

  // initialize the signal variable
  child_quit = false;
  // NOTE: do not need mutex or atomicity since only in parent context.

  // install sighandler to detect when the child process (make) finishes
  struct sigaction action;
  memset( &action, 0, sizeof(struct sigaction) );
  action.sa_handler = child_sighandler;
  sigaction( SIGCHLD, &action, NULL );

  // fork a child process
  pid_t pid = fork();
  if( pid < 0 ) {
    printf( "ERROR: bad pid\n" );
    // TODO : Improve error handling
  }

  // NOTE: paths need to be programatic
  std::string source_path = *((char* const*)args);
  std::string build_path = *((char* const*)args + 1);
 
  //printf( "source_path %s\n", source_path.c_str() );
  //printf( "build_path %s\n", build_path.c_str() );

  if( pid == 0 ) {
    // CHILD PROCESS
    pid = getpid();

    // change the working directory to the build path
    chdir( build_path.c_str() );

    // build the cmake command line arguments array
    std::vector<std::string> arg_strings;
    arg_strings.push_back( "cmake" );
    arg_strings.push_back( source_path );
    char* const* exec_argv = param_array( arg_strings );

    // build the enviroment variables array
    std::vector<std::string> env_strings = system_environment_vars();
    char* const* exec_envars = param_array( env_strings );

    // execute cmake
    execve( CMAKE_BIN, exec_argv, exec_envars );

    // trap and report any error.
    perror( "execve" );
    exit( EXIT_FAILURE );
    // NOTE: Unreachable on successful exec

    // FORKED PROCESS IS DEAD
  } else {
    // PARENT PROCESS
  
    // wait for signal the child has terminated
    while( !child_quit ) sleep(1);

    // check for the existence Makefile from cmake
    std::string makefile_path = build_path + '/' + "Makefile";
    struct stat st;
    stat( makefile_path.c_str(), &st );
    if( S_ISREG( st.st_mode ) ) {
      // if the Makefile exists then return true
      *result = true;
    } else {
      // otherwise return false
      *result = false;
    }
    // NOTE:  for platform independence, replace stat with boost filesystem
  }

  // uninstall sighandler
  action.sa_handler = SIG_DFL;
  sigaction( SIGCHLD, &action, NULL );

  return (void*) result;
}

//-----------------------------------------------------------------------------
bool make_package( char* const* args ) {
  pthread_t thread;
  pthread_attr_t attr;

  // allocate thread attributes structure
  if( pthread_attr_init( &attr ) != 0 ) {
    // error creating thread attributes
    printf( "ERROR: failed to create thread attributes\n" );
    // TODO : Improve error handling
  }
  // explicitly set the thread as joinable
  pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );

  // create the thread
  if( pthread_create( &thread, &attr, &make_worker, (void*)args ) != 0 ) {
    // error creating thread
    printf( "ERROR: failed to create thread\n" );
    // TODO : Improve error handling
  }

  // deallocate thread attributes structure
  if( pthread_attr_destroy( &attr ) != 0 ) {
    // error destroying thread attributes
    printf( "ERROR: failed to destroy thread attributes\n" );
    // TODO : Improve error handling
  }

  // join the thread
  void* retval;
  if( pthread_join( thread, &retval ) != 0 ) {
    // error joining thread
    printf( "ERROR: failed to join thread\n" );
    // TODO : Improve error handling
  }
  // copy the allocated return value to a local variable
  bool result = *(bool*)retval;

  // deallocate the memory allocated by joining
  free( retval );
 
  // return the local variable 
  return result;
}

//-----------------------------------------------------------------------------
bool make_package( std::string build_path ) {

  std::vector<std::string> args;

  args.push_back( build_path );

  char* const* make_args = param_array( args );

  return make_package( make_args );
}

//-----------------------------------------------------------------------------
// NOTE: this is the same as make_package other than the worker function
// can be abstracted if the function call can be determined by the args
bool cmake_package( char* const* args ) {
  pthread_t thread;
  pthread_attr_t attr;

  // allocate thread attributes structure
  if( pthread_attr_init( &attr ) != 0 ) {
    // error creating thread attributes
    printf( "ERROR: failed to create thread attributes\n" );
    // TODO : Improve error handling
  }
  // explicitly set the thread as joinable
  pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );

  // create the thread
  if( pthread_create( &thread, &attr, &cmake_worker, (void*)args ) != 0 ) {
    // error creating thread
    printf( "ERROR: failed to create thread\n" );
    // TODO : Improve error handling
  }

  // deallocate thread attributes structure
  if( pthread_attr_destroy( &attr ) != 0 ) {
    // error destroying thread attributes
    printf( "ERROR: failed to destroy thread attributes\n" );
    // TODO : Improve error handling
  }

  // join the thread
  void* retval;
  if( pthread_join( thread, &retval ) != 0 ) {
    // error joining thread
    printf( "ERROR: failed to join thread\n" );
    // TODO : Improve error handling
  }
  // copy the allocated return value to a local variable
  bool result = *(bool*)retval;

  // deallocate the memory allocated by joining
  free( retval );
 
  // return the local variable 
  return result;
}

//-----------------------------------------------------------------------------
bool cmake_package( std::string source_path, std::string build_path ) {

  std::vector<std::string> args;

  args.push_back( source_path );
  args.push_back( build_path );

  char* const* cmake_args = param_array( args );

  return cmake_package( cmake_args );
}

//-----------------------------------------------------------------------------
void gzexit_sighandler( int signum ) {
  // write to gzexit_write

  std::string msg = "true";
  gzexit_write->write( msg );
}

//-----------------------------------------------------------------------------
bool copy( std::string src, std::string dest ) {

  char buf[BUFSIZ];
  size_t size;

  int fd_src = open( src.c_str(), O_RDONLY, 0 );
  int fd_dest = open( dest.c_str(), O_WRONLY | O_CREAT /*| O_TRUNC*/, 0644 );

  while( ( size = read( fd_src, buf, BUFSIZ ) ) > 0 )
    write( fd_dest, buf, size );

  close( fd_src );
  close( fd_dest );

  return true;
}

//-----------------------------------------------------------------------------
std::string make_temp_dir( void ) {
  char tmp_path_template[] = "/tmp/reveal-XXXXXX";
  std::string tmp_path = mkdtemp( tmp_path_template );
  //printf( "tmp_path: %s\n", tmp_path.c_str() );

  return tmp_path;
}

//-----------------------------------------------------------------------------
int main( int argc, char* argv[] ) {

  std::string source_path = "/home/james/osrf/Reveal/tmp";
  std::string build_path = "/home/james/osrf/Reveal/tmp/build";

  std::string world_path = build_path + '/' + "test.world";
  std::string plugin_path = build_path;
  std::string model_path = build_path;

  // Initialization
  if( !client.init() ) {
    printf( "ERROR: Failed to initialize properly.\nExiting.\n" );
    client.terminate();
    exit(1);
  }

  // Salutations
  printf( "Welcome to Reveal\n" );

  // Connect to Server
  printf( "Connecting to Reveal Server\n" );

  if( !client.connect() ) {
    printf( "ERROR: Unable to reach Reveal Server.\nExiting.\n" );
    client.terminate();
    exit(1);
  }

  printf("Connected to Reveal Server\n");

  // Prompt for Login
  bool anonymous_login = false;
  char yn = 'Y';
  char input_buffer[512];

  do {
    printf( "Would you like to login (Y/N)?: " );
    scanf( "%c", &yn );

    if( yn == 'y' || yn == 'Y' ) { 
      anonymous_login = false;
    } else if( yn == 'n' || yn == 'N' ) {
      anonymous_login = true;
    } else {
      if( yn != 13 && yn != 10 ) printf( "\n" );
      printf( "ERROR: Invalid Input. Enter y or n\n" );
    }
  } while( yn != 'n' && yn != 'N' && yn != 'y' && yn != 'Y' );

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
  Reveal::Client::client_c::error_e client_error;
  client_error = client.request_authorization( auth );
  if( client_error != Reveal::Client::client_c::ERROR_NONE ) {
    printf( "ERROR: Failed to gain authorization\n" );
  } else {
    printf( "Authorization granted\n" );
  }

  // MOTD
  printf("Message of the Day\n");

  Reveal::Core::digest_ptr digest;
  Reveal::Core::scenario_ptr scenario;
  Reveal::Core::experiment_ptr experiment;
  Reveal::Core::trial_ptr trial;
  Reveal::Core::solution_ptr solution;
  Reveal::Core::transport_exchange_c ex;

  // Digest
  printf( "Fetching Scenario Digest\n" );

  // request the digest
  client.request_digest( auth, digest );
  // TODO: error handling

  // print digest menu

  // user selects scenario

  // fetch scenario
  // for testing purposes, pick a random scenario
  assert( digest->scenarios() );
  scenario = digest->get_scenario( 0 );
  //scenario->print();

  // request experiment
  client_error = client.request_experiment( auth, scenario, experiment );
  if( client_error != Reveal::Client::client_c::ERROR_NONE ) {
    printf( "ERROR: client failed to receive experiment\n" );
  } else {
    printf( "SUCCESS: client received: experiment[%s]\n", experiment->experiment_id.c_str() );
    //experiment->print();
    scenario->print();
  }

  // print dynamics menu

  // user selects dynamics

  // print tuning menu

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
//  exit( 0 );

  // NOTE: The fork should only happen once the whole trial has been set up.
  // CORRECTION: The IPC needs to be inside the while.  We don't want to 
  // to refork gzserver repeatedly.  The while effectively gets substituted for
  // the **SIMULATION WOULD BE RUN HERE** comment below.

  printf( "launching gzserver\n" );

  unsigned port = PORT + 1;

  // open gzconnection and gzsignal
  // TODO: use correct constructors
  void* context = client._connection.context();
  gzipc = Reveal::Core::pipe_ptr( new Reveal::Core::pipe_c( GAZEBO_PORT, context ) );
  gzexit_read = Reveal::Core::pipe_ptr( new Reveal::Core::pipe_c( "gzsignal", true, context ) );
  gzexit_write = Reveal::Core::pipe_ptr( new Reveal::Core::pipe_c( "gzsignal", false, context ) );

  if( gzipc->open() != Reveal::Core::pipe_c::ERROR_NONE ) {
    // TODO: Error handling
  }
  if( gzexit_read->open() != Reveal::Core::pipe_c::ERROR_NONE ) {
    // TODO: Error handling
  }
  if( gzexit_write->open() != Reveal::Core::pipe_c::ERROR_NONE ) {
    // TODO: Error handling
  }

  printf( "ipc pipes open\n" );

  //void* context = client._connection.context();
  //unsigned int ctx = *(unsigned int*)context;
  //printf( "Extracted context: %d\n", ctx );

//  exit(0);

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

    // build the make command line arguments array
    std::vector<std::string> arg_strings;
    arg_strings.push_back( "reveal-gzserver" );
//    arg_strings.push_back( "-u" ); 
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

    execve( GZSERVER_BIN, exec_argv, exec_envars );

    perror( "execve" );
    exit( EXIT_FAILURE );

  } else {
    // parent process

    printf( "reveal client yielding\n" );
    sleep( 5 );

    // TODO: find exception here
    // write experiment
    std::string msg;

    ex.build_server_experiment( msg, auth, scenario, experiment );

    printf( "writing experiment to gazebo\n" );
    if( gzipc->write( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
      // TODO: trap and recover
    }


    unsigned trial_index = 0;

    printf( "Starting Main Loop\n" );
    while( true ) {

      if( trial_index < scenario->trials ) {
        // request trial from revealserver
        // create a trial
        trial = scenario->get_trial( trial_index );

        client.request_trial( auth, experiment, trial );
        // TODO: error handling

        // write trial to gzipc
        ex.build_server_trial( msg, auth, experiment, trial );

        printf( "writing experiment to gazebo\n" );
        if( gzipc->write( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
          // TODO: trap and recover
        }
      }

      zmq_pollitem_t channels[2];
      channels[0].socket = gzipc->socket();
      channels[0].fd = 0;
      channels[0].events = ZMQ_POLLIN;
      channels[0].revents = 0;

      channels[1].socket = gzexit_read->socket();
      channels[1].fd = 0;
      channels[1].events = ZMQ_POLLIN;
      channels[1].revents = 0;

      int rc = zmq_poll( channels, 2, -1);
      //assert (rc >= 0); // Returned events will be stored in items[].revents 

      if( channels[0].revents & ZMQ_POLLIN ) {

        // read solution from message
        if( gzipc->read( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
          // TODO: trap and recover
        }

        // forward solution to revealserver
        ex.parse_client_solution( msg, auth, experiment, solution );

        // submit the solution to the server
        client.submit_solution( auth, experiment, solution );

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
    gzipc->close();
    gzexit_read->close();
    gzexit_write->close();
  }

  client.terminate();

  return 0;
}

//-----------------------------------------------------------------------------
