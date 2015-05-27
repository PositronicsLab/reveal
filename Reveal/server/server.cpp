#include "Reveal/server/server.h"

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>

#include "Reveal/core/console.h"
#include "Reveal/core/system.h"
#include "Reveal/core/pointers.h"
#include "Reveal/core/analyzer.h"
#include "Reveal/core/package.h"
#include "Reveal/core/transport_exchange.h"

#include "Reveal/db/database.h"

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Server {

//-----------------------------------------------------------------------------
/// Default Constructor
server_c::server_c( void ) {

}

//-----------------------------------------------------------------------------
/// Destructor
server_c::~server_c( void ) {

}

//-----------------------------------------------------------------------------
/// Initialization
bool server_c::init( void ) {
  // open the system
  Reveal::Core::system_c system( Reveal::Core::system_c::SERVER );
  if( !system.open() ) {
    std::string errmsg = "ERROR: Failed to open system.  Make sure the Reveal environment is properly configured";
    Reveal::Core::console_c::error( errmsg );
    return false;
  }

  // get system environmental parameters
  unsigned port = system.server_port();

  // close the system
  system.close();

  // open the database
  boost::shared_ptr<Reveal::DB::database_c> db( new Reveal::DB::database_c() );
  if( !db->open() ) {
    std::string errmsg = "ERROR: Failed to open database.";
    Reveal::Core::console_c::error( errmsg );
    return false;
  }
 
  // fetch the list of analyzer plugins
  std::vector<Reveal::Core::analyzer_ptr> analyzers;
  db->fetch( analyzers );
 
  // compile all analyzer plugins
  for( unsigned i = 0; i < analyzers.size(); i++ ) {
    // if not a plugin, then skip compiling
    if( analyzers[i]->type != Reveal::Core::analyzer_c::PLUGIN ) continue;

    std::string src_path = analyzers[i]->source_path;
    std::string build_path = analyzers[i]->build_path;
    std::string file_name = analyzers[i]->build_target;
    std::string library_path = combine_path( build_path, file_name );

    std::vector<std::string> build_products;
    build_products.push_back( file_name );

    if( !path_exists( src_path ) ) {
      std::cerr << "ERROR: Unable to locate the analyzer path." << std::endl;
      //return false;
    }

    if( !get_directory( build_path ) ) {
      std::cerr << "ERROR: Unable to open the analyzer build path." << std::endl;
      //return false;
    }

    Reveal::Core::package_ptr package( new Reveal::Core::package_c( src_path, build_path ) );

    // configure package
    bool cmake_result = package->configure();
    if( !cmake_result ) {
      printf( "ERROR: Failed to configure make for analyzer plugin\nExiting\n" );
      //return false;
    } else {
      printf( "Package configuration succeeded\n" );
    }

    // build package
    bool make_result = package->make( build_products );
    if( !make_result ) {
      printf( "ERROR: Failed to build analyzer plugin\nExiting\n" );
      //return false;
    } else {
      printf( "Built package\n" );
    }
  }


  // open IPC
  Reveal::Core::transport_exchange_c::open();  

  _clientconnection = Reveal::Core::connection_c( port );
  if( _clientconnection.open() != Reveal::Core::connection_c::ERROR_NONE ) {
    printf( "Failed to open clientconnection\n" );
    return false;
  }

  _workerconnection = Reveal::Core::connection_c( Reveal::Core::connection_c::DEALER, _clientconnection.context() );
  if( _workerconnection.open() != Reveal::Core::connection_c::ERROR_NONE ) {
    printf( "Failed to open workerconnection\n" );
    return false; 
  }

  void* context = _clientconnection.context();
  for( unsigned i = 0; i < MAX_CLIENT_WORKERS; i++ ) {
    pthread_t workerthread;
    pthread_create( &workerthread, NULL, server_c::worker_thread, context );
    workers.push_back( workerthread );
  }

  printf( "Server is listening...\n" );

  return true; 
}

//-----------------------------------------------------------------------------
/// Main Loop
void server_c::run( void ) {
  _clientconnection.route( _workerconnection );
}

//-----------------------------------------------------------------------------
void server_c::terminate( void ) {
  // Note: may need to loop on these connection close operations if they don't
  // return ERROR_NONE and do return ERROR_INTERRUPT
  _workerconnection.close();
  _clientconnection.close();

  Reveal::Core::transport_exchange_c::close();

} 

//-----------------------------------------------------------------------------
//TODO : need to properly handle a failure in worker init.
void* server_c::worker_thread( void* context ) {
  boost::shared_ptr<worker_c> worker = boost::shared_ptr<worker_c>( new worker_c(context) );
  if( !worker->init() ) return NULL;
  worker->work();
  worker->terminate();
  return NULL;
}
//-----------------------------------------------------------------------------

} // namespace Server

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------
