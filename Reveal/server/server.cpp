#include "Reveal/server/server.h"

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>

#include "Reveal/core/system.h"
#include "Reveal/core/transport_exchange.h"

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
  Reveal::Core::system_c system( Reveal::Core::system_c::SERVER );
  if( !system.open() ) return false;

  unsigned port = system.server_port();

  system.close();

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
