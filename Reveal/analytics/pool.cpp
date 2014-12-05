#include "Reveal/analytics/pool.h"

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Analytics {

//-----------------------------------------------------------------------------
pool_c::pool_c( void ) {

}

//-----------------------------------------------------------------------------
pool_c::~pool_c( void ) {

}

//-----------------------------------------------------------------------------
Reveal::Analytics::worker_ptr pool_c::spawn_worker( void ) {
  worker_ptr worker;
  worker = worker_ptr( new worker_c() );

  // todo call the worker initialization

  return worker;
}

//-----------------------------------------------------------------------------
Reveal::Analytics::error_e pool_c::init( void ) {
  //if( !allocate() ) return false;

  return ERROR_NONE;
}

/*
//-----------------------------------------------------------------------------
bool pool_c::allocate( void ) {
  for( unsigned i = 0; i < MAX_ANALYTIC_WORKERS; i++ ) {
    _workers.push_back( spawn_worker() );
  }
  return true;
}
*/
//-----------------------------------------------------------------------------
Reveal::Analytics::error_e pool_c::cycle( void ) {

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
Reveal::Analytics::error_e pool_c::shutdown( void ) {

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
Reveal::Analytics::error_e pool_c::poll_workers( void ) {

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
Reveal::Analytics::error_e pool_c::poll_server( void ) {

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
Reveal::Analytics::error_e pool_c::process_messages( void ) {

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
Reveal::Analytics::error_e pool_c::dispatch( void ) {


  return ERROR_NONE;
}

//-----------------------------------------------------------------------------

} // namespace Analytics

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------
