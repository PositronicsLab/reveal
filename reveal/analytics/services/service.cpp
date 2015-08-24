#include "service.h"
#include "worker.h"

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>

#include "reveal/core/console.h"
#include "reveal/core/system.h"
#include "reveal/core/pointers.h"
#include "reveal/core/analyzer.h"
#include "reveal/core/package.h"
#include "reveal/analytics/exchange.h"

#include "reveal/db/database.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Analytics {
//-----------------------------------------------------------------------------
service_c::service_c( void ) {

}

//-----------------------------------------------------------------------------
service_c::~service_c( void ) {

}

//-----------------------------------------------------------------------------
bool service_c::init( void ) {
  // open the system
  Reveal::Core::system_c system( Reveal::Core::system_c::SERVER );
  if( !system.open() ) {
    std::string errmsg = "ERROR: Failed to open system.  Make sure the Reveal environment is properly configured";
    Reveal::Core::console_c::error( errmsg );
    return false;
  }

  // get system environmental parameters
  unsigned port = system.analytics_port();
  std::string pkg_path = system.packages_path();

  printf( "REVEAL_ANALYZER_PORT=%u\n", port );

  // close the system
  system.close();

  // open the database
  boost::shared_ptr<Reveal::DB::database_c> db( new Reveal::DB::database_c() );
  if( !db->open() ) {
    std::string errmsg = "ERROR: Failed to open database.";
    Reveal::Core::console_c::error( errmsg );
    return false;
  }
 
  // open IPC
  Reveal::Analytics::exchange_c exchange;
  exchange.open();  

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

  // spawn worker threads and set them to work
  void* context = _clientconnection.context();
  for( unsigned i = 0; i < MAX_CLIENT_WORKERS; i++ ) {
    pthread_t workerthread;
    pthread_create( &workerthread, NULL, service_c::worker_thread, context );
    workers.push_back( workerthread );
  }

  printf( "Server is listening...\n" );

  return true; 
}

//-----------------------------------------------------------------------------
void service_c::run( void ) {
  _clientconnection.route( _workerconnection );
}

//-----------------------------------------------------------------------------
void service_c::terminate( void ) {
  // Note: may need to loop on these connection close operations if they don't
  // return ERROR_NONE and do return ERROR_INTERRUPT
  _workerconnection.close();
  _clientconnection.close();

  Reveal::Analytics::exchange_c exchange;
  exchange.close();  

} 

//-----------------------------------------------------------------------------
//TODO : need to properly handle a failure in worker init.
void* service_c::worker_thread( void* context ) {
  boost::shared_ptr<worker_c> worker = boost::shared_ptr<worker_c>( new worker_c(context) );
  if( !worker->init() ) return NULL;
  worker->work();
  worker->terminate();
  return NULL;
}

//-----------------------------------------------------------------------------
} // namespace Analytics
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
