#include "Reveal/client/system.h"

/*
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <cstdio>
*/
//#include <cstdio>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <cstring>

#include "Reveal/core/pointers.h"
#include "Reveal/core/digest.h"

#include "Reveal/core/experiment.h"
#include "Reveal/core/scenario.h"
#include "Reveal/core/trial.h"
#include "Reveal/core/solution.h"

//-----------------------------------------------------------------------------
extern char **environ;

//-----------------------------------------------------------------------------
std::vector< std::string > system_environment_vars( void ) {
  std::vector<std::string> ev;

  for (char **env = environ; *env; ++env) {
    ev.push_back( *env );
  }

  return ev;
}

/*
//-----------------------------------------------------------------------------
bool copy_file( std::string src, std::string dest ) {

  char buf[BUFSIZ];
  size_t size;

  int fd_src = open( src.c_str(), O_RDONLY, 0 );
  int fd_dest = open( dest.c_str(), O_WRONLY | O_CREAT, 0644 );
//  int fd_dest = open( dest.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644 );

  while( ( size = read( fd_src, buf, BUFSIZ ) ) > 0 )
    write( fd_dest, buf, size );

  close( fd_src );
  close( fd_dest );

  return true;
}
*/

//-----------------------------------------------------------------------------
std::string make_temp_dir( void ) {
  char tmp_path_template[] = "/tmp/reveal-XXXXXX";
  std::string tmp_path = mkdtemp( tmp_path_template );
  //printf( "tmp_path: %s\n", tmp_path.c_str() );

  return tmp_path;
}

//-----------------------------------------------------------------------------
// TODO: move to utilities
void print_vector_of_strings( std::vector<std::string> v ) {
  for( std::vector<std::string>::iterator it = v.begin(); it != v.end(); it++ )
    printf( "%s\n", it->c_str() );
}

//-----------------------------------------------------------------------------
// TODO: move to utilities
void print_param_array( char* const* a ) {
  for( char* const* ptr = (char* const*)a; *ptr != NULL; ptr++ )
    printf( "%s\n", *ptr );
}

//-----------------------------------------------------------------------------
// TODO: move to a class to have destructor for correct cleanup.
char* const* param_array( std::vector< std::string > params ) {

  const char** pa = (const char**)malloc( sizeof(char*) * params.size() + 1 );
  for( unsigned i = 0; i < params.size(); i++ ) {
    pa[i] = (const char*)params[i].c_str();
  }
  pa[ params.size() ] = NULL;

  return (char* const*) pa;
}

//-----------------------------------------------------------------------------
bool prompt_yes_no( std::string prompt, bool default_value ) {
  char yn;

  if( default_value )
    yn = 'Y';
  else
    yn = 'N';

  do {
    printf( "%s ", prompt.c_str() );
    int result = scanf( "%c", &yn );

    if( yn == 'y' || yn == 'Y' ) { 
      return true;
    } else if( yn == 'n' || yn == 'N' ) {
      return false;
    }
    
    //if( yn != 13 && yn != 10 ) printf( "\n" );
    printf( "ERROR: Invalid Input. Enter y or n\n" );
    
  } while( true );

}

//-----------------------------------------------------------------------------
void print_digest_menu( Reveal::Core::digest_ptr digest ) {
  Reveal::Core::scenario_ptr scenario;

  printf( "- Scenario Menu -\n" );

  for( unsigned i = 0; i < digest->scenarios(); i++ ) {
    scenario = digest->get_scenario( i );

    printf( "%d: %s\n", i, scenario->description.c_str() );
  }
}

//-----------------------------------------------------------------------------
unsigned prompt_digest( Reveal::Core::digest_ptr digest ) {
  Reveal::Core::scenario_ptr scenario;

  unsigned choice = 0;
  unsigned range = digest->scenarios() - 1;
  int result;

  do {
    printf( "Select a scenario: " );
    result = scanf( "%u", &choice );
    
    if( !result || choice > range ) 
      printf( "ERROR: Invalid Input. Enter a value in the range [0,%u]\n", range );
  } while( !result || choice > range );
 
  return choice;
}

//-----------------------------------------------------------------------------
unsigned prompt_select_string_from_list( std::string prompt, std::vector< std::string > list, unsigned default_value ) {
  unsigned choice = default_value;
  unsigned range = list.size() - 1;
  int result;

  do {
    for( unsigned i = 0; i < list.size(); i++ ) {
      printf( "%d: %s\n", i, list[i].c_str() );
    }

    printf( "%s: ", prompt.c_str() );
    result = scanf( "%u", &choice );
    
    if( !result || choice > range ) 
      printf( "ERROR: Invalid Input. Enter a value in the range [0,%u]\n", range );
  } while( !result || choice > range );
 
  return choice; 
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool child_exit_handler_c::_quit = false;

child_exit_handler_c::child_exit_handler_c( void ) { _quit = false; }
child_exit_handler_c::~child_exit_handler_c( void ) { }

void child_exit_handler_c::handler( int signum ) {
  _quit = true; 
}

bool child_exit_handler_c::quit( void ) { return _quit; }
void child_exit_handler_c::set( bool quit_ ) { _quit = quit_; }
void child_exit_handler_c::reset( void ) { _quit = false; }

void child_exit_handler_c::install( void ) {   
  struct sigaction action;
  memset( &action, 0, sizeof(struct sigaction) );
  action.sa_handler = handler;
  sigaction( SIGCHLD, &action, NULL );
}
void child_exit_handler_c::uninstall( void ) {
  struct sigaction action;
  action.sa_handler = SIG_DFL;
  sigaction( SIGCHLD, &action, NULL );
}

void child_exit_handler_c::trip( void ) {
  _quit = true;
}

//-----------------------------------------------------------------------------
void* make_worker( void* args ) {

  // allocate a return variable
  bool* result = new bool;

  // install signal handler to detect when the child process (make) finishes
  child_exit_handler_c exithandler;
  exithandler.install();

  // fork a child process
  pid_t pid = fork();
  if( pid < 0 ) {
    printf( "ERROR: bad pid\n" );
    // TODO : Improve error handling
  }

  // NOTE: paths need to be programatic
  std::string build_path = *((char* const*)args);

  //std::string file_name = "libcontroller.so";
  //std::string file_path = build_path + '/' + file_name;
  // NOTE: may be multiple libraries built by process

  // needs to be determined from manifest
  std::string file_name = "libgz-arm-plugin.so";
  std::string file_path = build_path + '/' + file_name;

  // And also
  //std::string file_name = "libworld-plugin.so";
  //std::string file_path = build_path + '/' + file_name;

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
    while( !exithandler.quit() ) sleep(1);

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

  // uninstall signal handler
  exithandler.uninstall();

  return (void*) result;
}

//-----------------------------------------------------------------------------
void* cmake_worker( void* args ) {

  // allocate a return variable
  bool* result = new bool;

  // install signal handler to detect when the child process (make) finishes
  child_exit_handler_c exithandler;
  exithandler.install();

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
    while( !exithandler.quit() ) sleep(1);

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

  // uninstall signal handler
  exithandler.uninstall();

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


