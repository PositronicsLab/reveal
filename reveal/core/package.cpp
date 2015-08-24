#include "reveal/core/package.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <cstring>
#include <sstream>

#include "reveal/core/system.h"

#include "reveal/core/console.h"
//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TODO: should be sequestered if possible in system.h/cpp
bool child_exit_handler_c::_quit = false;
//-----------------------------------------------------------------------------
child_exit_handler_c::child_exit_handler_c( void ) { _quit = false; }
//-----------------------------------------------------------------------------
child_exit_handler_c::~child_exit_handler_c( void ) { }
//-----------------------------------------------------------------------------
void child_exit_handler_c::handler( int signum ) {
  _quit = true; 
  assert( signum );
}
//-----------------------------------------------------------------------------
bool child_exit_handler_c::quit( void ) { return _quit; }
//-----------------------------------------------------------------------------
void child_exit_handler_c::install( void ) {   
  struct sigaction action;
  memset( &action, 0, sizeof(struct sigaction) );
  action.sa_handler = handler;
  sigaction( SIGCHLD, &action, NULL );
}
//-----------------------------------------------------------------------------
void child_exit_handler_c::uninstall( void ) {
  struct sigaction action;
  action.sa_handler = SIG_DFL;
  sigaction( SIGCHLD, &action, NULL );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
package_c::package_c( std::string source_path, std::string build_path ) {
  _source_path = source_path;
  _build_path = build_path;
  _sub_package = "";
}

//-----------------------------------------------------------------------------
package_c::~package_c( void ) {

}

//-----------------------------------------------------------------------------
xml_element_ptr package_c::read( std::string sim_key ) {
  std::string manifest_path = _source_path;

//  Reveal::Core::console_c::printline(_source_path);
  xml_element_ptr manifest = manifest_c::read( manifest_path );
  if( !manifest ) return manifest;

  xml_element_ptr element, sim;
  for( unsigned i = 0; i < manifest->elements(); i++ ) {
    element = manifest->element( i );
    if( element->get_name() == sim_key ) {
      sim = element;
      break;
    }
  }
  return sim;
}

//-----------------------------------------------------------------------------
xml_element_ptr package_c::read( std::string sub_package, std::string sim_key ) {
  _sub_package = sub_package;
  std::string manifest_path = combine_path( _source_path, _sub_package );

//  Reveal::Core::console_c::printline(_source_path);
  xml_element_ptr manifest = manifest_c::read( manifest_path );
  if( !manifest ) return manifest;

  xml_element_ptr element, sim;
  for( unsigned i = 0; i < manifest->elements(); i++ ) {
    element = manifest->element( i );
    if( element->get_name() == sim_key ) {
      sim = element;
      break;
    }
  }
  return sim;
}

//-----------------------------------------------------------------------------
bool package_c::configure( void ) {
  return cmake_package( _source_path, _build_path );
}

//-----------------------------------------------------------------------------
bool package_c::configure( std::vector<std::string> arguments ) {
  return cmake_package( _source_path, _build_path, &arguments );
}

//-----------------------------------------------------------------------------
bool package_c::make( std::vector<std::string> build_products, std::string sub_package ) {
  //if( !build_products.size() ) return false;  
  // above remarked because not definitive that build products needs anything 
  // for all sims
  return make_package( _build_path, build_products, sub_package );
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

  unsigned parameter_count = atoi( *((char* const*)args) );  //args[0]=count
  std::string working_path, source_path, build_path, subpackage;

  build_path = *((char* const*)args+1);        // args[1]=build path
  working_path = build_path;
/*
  if( parameter_count == 3 ) {
    // no sub_package
    subpackage = "";
  } else if( parameter_count == 4 ) {
    subpackage = *((char* const*)args+2);        // args[2]=subpackage
    working_path = combine_path( working_path, subpackage );
  } else {
    // impossible outcome
  }
*/
/*
  Reveal::Core::console_c::printline( working_path );
  Reveal::Core::console_c::printline( build_path );
  Reveal::Core::console_c::printline( subpackage );
*/

  if( pid == 0 ) {
    // CHILD PROCESS
    pid = getpid();

    // build the make command line arguments array
    std::vector<std::string> arg_strings;
    arg_strings.push_back( "make" );
    arg_strings.push_back( "-C" );
    arg_strings.push_back( working_path );
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
    std::vector<std::string> build_products;               // args[x]=n
    unsigned products = atoi( *((char* const*)args + parameter_count - 1) ); // args[x..n+1]=files
    for( unsigned i = 0; i < products; i++ ) {
      std::string file_name = working_path + '/' + std::string( *((char* const*)args + parameter_count + i) );
      build_products.push_back( file_name );
    }
    //Reveal::Core::console_c::print(build_products);
 
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
// Note: this can succeed even if cmake failed if the directory was not clean 
// before the attempt to cmake.  It is probably a rare instance that it exits
// and indicates success when it really failed, but it is possible.
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

  unsigned parameter_count = atoi( *((char* const*)args) );  //args[0]=count
  std::string working_path, source_path, build_path, subpackage;
  std::string optional_args;
//  if( parameter_count != 3 ) {
    // The interface is unsupported
//  }

  source_path = *((char* const*)args+1);        // args[1]=source path
  build_path = *((char* const*)args+2);        // args[2]=build path
//  if( parameter_count > 3 )
//    optional_args = *((char* const*)args+3);

  working_path = build_path;
/*
  Reveal::Core::console_c::printline( working_path );
  Reveal::Core::console_c::printline( build_path );
  Reveal::Core::console_c::printline( source_path );
*/
  if( pid == 0 ) {
    // CHILD PROCESS
    pid = getpid();

    // change the working directory to the build path
    if( !change_working_dir( working_path ) ) {
      // TODO: error handling
      printf( "ERROR: Failed to change working directory\n" );
    }

    // build the cmake command line arguments array
    std::vector<std::string> arg_strings;
    arg_strings.push_back( "cmake" );
    //if( optional_args != "" ) arg_strings.push_back( optional_args );
    for( unsigned i = 3; i < parameter_count; i++ ) {
      arg_strings.push_back( *((char* const*) args+i) );
    }

    //arg_strings.push_back( "-DGAZEBO=ON" );  
    // Note: above commented because it requires particular knowledge of the 
    // installed package and installed sims.  Needs to be handled a differently.
    // as at this level, needs to be abstracted.  Might be possible to send in
    // a list of optional make parameters.
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
    std::string makefile_path = working_path + '/' + "Makefile";
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
bool package_c::make_package( std::string build_path, std::vector<std::string> build_products, std::string sub_package ) {

  std::vector<std::string> args;
  std::stringstream param_count, product_count;

  unsigned params = 1;               // the counter column [0]
  params++;                          // the build_path column [1]
//  if( sub_package != "" ) params++;  // the subpackage column [2]
  params++;                          // the build product counter column [2]|[3]

  param_count << params;
  product_count << build_products.size();

  args.push_back( param_count.str() );          // arg[0] = number of parameters
  args.push_back( build_path );     // arg[1] = build path
//  if( sub_package != "" ) 
//    args.push_back( sub_package );  // arg[2] = sub_package
  args.push_back( product_count.str() );// arg[2] | arg[3] = build product count

  for( std::vector<std::string>::iterator it = build_products.begin(); it != build_products.end(); it++ )
    args.push_back( *it );

  char* const* make_args = param_array( args );

  bool result = make_package( make_args );
  
  free( (void*)make_args );

  return result;
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
bool package_c::cmake_package( std::string source_path, std::string build_path, std::vector<std::string>* optional_args ) {
  std::vector<std::string> args;
  std::stringstream param_count;

  unsigned params = 3;
  if( optional_args != NULL ) {
    params += optional_args->size();
  }
  param_count << params;

  args.push_back( param_count.str() ); // arg[0] = parameter count
  args.push_back( source_path );       // arg[1] = source path
  args.push_back( build_path );        // arg[2] = build path

  if( optional_args != NULL ) {
    for( std::vector<std::string>::iterator it = optional_args->begin(); it != optional_args->end(); it++ )
      args.push_back( *it ); 
  }

  char* const* cmake_args = param_array( args );

  return cmake_package( cmake_args );
}

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


