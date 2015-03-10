#include "Reveal/client/package.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <cstring>
#include <sstream>

#include "Reveal/client/system.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Client {
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TODO: should be sequestered if possible in system.h/cpp
bool child_exit_handler_c::_quit = false;

child_exit_handler_c::child_exit_handler_c( void ) { _quit = false; }
child_exit_handler_c::~child_exit_handler_c( void ) { }

void child_exit_handler_c::handler( int signum ) {
  _quit = true; 
  assert( signum );
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
//-----------------------------------------------------------------------------
// TODO: build_products and any other additional parameters need to be 
// supplanted by a manifest 
package_c::package_c( std::string source_path, std::string build_path ) {
  _source_path = source_path;
  _build_path = build_path;
}

//-----------------------------------------------------------------------------
package_c::~package_c( void ) {

}

//-----------------------------------------------------------------------------
bool package_c::read( void ) {
  bool result;
  result = _manifest.parse( _source_path );
  return result;
}

//-----------------------------------------------------------------------------
bool package_c::configure( void ) {
  return cmake_package( _source_path, _build_path );
}

//-----------------------------------------------------------------------------
bool package_c::make( void ) {
  return make_package( _build_path, _manifest.build_products() );
}

//-----------------------------------------------------------------------------
void* package_c::make_worker( void* args ) {

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

  std::string build_path = *((char* const*)args);        // args[0]=root path

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
    //exit( EXIT_FAILURE );
    exit( 1 );
    // NOTE: Unreachable on successful exec

    // FORKED PROCESS IS DEAD
  } else {
    // PARENT PROCESS

    // monitoring for the building of the package build products
    // so extract the filenames of all the products into a iterable vector
    std::vector<std::string> build_products;               // args[1]=n files
    unsigned products = atoi( *((char* const*)args + 1) ); // args[2..n+1]=files
    for( unsigned i = 0; i < products; i++ ) {
      std::string file_name = build_path + '/' + std::string( *((char* const*)args + 2 + i) );
      build_products.push_back( file_name );
    }
  
    // wait for signal the child has terminated
    while( !exithandler.quit() ) sleep(1);

    *result = true;
    for( std::vector<std::string>::iterator it = build_products.begin(); it != build_products.end(); it++ ) {
      if( !file_exists( *it ) ) {
        *result = false;
        break;
      }
    }
  }

  // uninstall signal handler
  exithandler.uninstall();

  return (void*) result;
}

//-----------------------------------------------------------------------------
void* package_c::cmake_worker( void* args ) {

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

  if( pid == 0 ) {
    // CHILD PROCESS
    pid = getpid();

    // change the working directory to the build path
    if( !change_working_dir( build_path ) ) {
      // TODO: error handling
    }

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
    //exit( EXIT_FAILURE );
    exit( 1 );
    // NOTE: Unreachable on successful exec

    // FORKED PROCESS IS DEAD
  } else {
    // PARENT PROCESS
  
    // wait for signal the child has terminated
    while( !exithandler.quit() ) sleep(1);

    // check for the existence Makefile from cmake
    std::string makefile_path = build_path + '/' + "Makefile";
    if( file_exists( makefile_path ) )
      *result = true;
    else
      *result = false;
  }

  // uninstall signal handler
  exithandler.uninstall();

  return (void*) result;
}

//-----------------------------------------------------------------------------
bool package_c::make_package( char* const* args ) {
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
bool package_c::make_package( std::string build_path, std::vector<std::string> build_products ) {

  std::vector<std::string> args;

  args.push_back( build_path );
  std::stringstream sssize;
  sssize << build_products.size();
  args.push_back( sssize.str() );
  for( std::vector<std::string>::iterator it = build_products.begin(); it != build_products.end(); it++ )
    args.push_back( *it );

  char* const* make_args = param_array( args );

  return make_package( make_args );
}

//-----------------------------------------------------------------------------
// NOTE: this is the same as make_package other than the worker function
// can be abstracted if the function call can be determined by the args
bool package_c::cmake_package( char* const* args ) {
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
bool package_c::cmake_package( std::string source_path, std::string build_path ) {

  std::vector<std::string> args;

  args.push_back( source_path );
  args.push_back( build_path );

  char* const* cmake_args = param_array( args );

  return cmake_package( cmake_args );
}

//-----------------------------------------------------------------------------
} // namespace Client
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


