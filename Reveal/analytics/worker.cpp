#include "Reveal/analytics/worker.h"

#include "Reveal/analytics/plugin.h"
#include "Reveal/analytics/script.h"
#include "Reveal/core/experiment.h"

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Analytics {

//-----------------------------------------------------------------------------
worker_c::worker_c( void ) {

}

//-----------------------------------------------------------------------------
worker_c::worker_c( boost::shared_ptr<Reveal::DB::database_c> db, const std::string& experiment_id ) {
  _db = db;
  _experiment_id = experiment_id;
}

//-----------------------------------------------------------------------------
worker_c::~worker_c( void ) {

}

//-----------------------------------------------------------------------------
Reveal::Analytics::error_e worker_c::init( void ) {
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
Reveal::Analytics::error_e worker_c::cycle( void ) {
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
Reveal::Analytics::error_e worker_c::shutdown( void ) {
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
Reveal::Analytics::error_e worker_c::read( void ) {
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
Reveal::Analytics::error_e worker_c::query( void ) {
  // query the database for trial data
  // TODO : better error handling

  Reveal::DB::database_c::error_e db_error;

  db_error = _db->query( _experiment, _experiment_id );
  if( db_error != Reveal::DB::database_c::ERROR_NONE )
    printf( "db_error[%d] fetching experiment\n", db_error );

  db_error = _db->query( _analyzer, _experiment->scenario_id );
  if( db_error != Reveal::DB::database_c::ERROR_NONE )
    printf( "db_error[%d] fetching analyzer\n", db_error );

  if( !_analyzer ) 
    printf( "failed to select requested analyzer from db\n" );

  db_error = _db->query( _solution_set, _experiment->experiment_id );
  if( db_error != Reveal::DB::database_c::ERROR_NONE )
    printf( "db_error[%d] fetching solution_set\n", db_error );

  if( !_solution_set ) 
    printf( "failed to select requested solution_set from db\n" );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
Reveal::Analytics::error_e worker_c::load( void ) {
  if( _analyzer->type == Reveal::Core::analyzer_c::PLUGIN ) {
    // load analyzer as plugin
    boost::shared_ptr<Reveal::Analytics::plugin_c> plugin = boost::shared_ptr<Reveal::Analytics::plugin_c>( new Reveal::Analytics::plugin_c() );
    
    _module = boost::dynamic_pointer_cast<Reveal::Analytics::module_c>( plugin );
  } else if( _analyzer->type == Reveal::Core::analyzer_c::SCRIPT ) {
    // load analyzer as script

    boost::shared_ptr<Reveal::Analytics::script_c> script = boost::shared_ptr<Reveal::Analytics::script_c>( new Reveal::Analytics::script_c() );

    _module = boost::dynamic_pointer_cast<Reveal::Analytics::module_c>( script );
  }

  // get the filename of the module and load it
  Reveal::Analytics::error_e error = _module->load( _analyzer->filename );
  if( error != Reveal::Analytics::ERROR_NONE ) {
    printf( "failed to load requested analyzer\n" );
    return error;
  }

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
Reveal::Analytics::error_e worker_c::analyze( void ) {
  Reveal::Analytics::error_e error = _module->analyze( _solution_set, _analysis );
  if( error != Reveal::Analytics::ERROR_NONE ) {
    printf( "analyzer failed to complete execution\n" ); 
    return error;
  }
 
  assert( _analysis );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
Reveal::Analytics::error_e worker_c::insert( void ) {
  Reveal::DB::database_c::error_e db_error;

  db_error = _db->insert( _analysis );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
Reveal::Analytics::error_e worker_c::idle( void ) {
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
Reveal::Analytics::error_e worker_c::send( void ) {
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
Reveal::Analytics::error_e worker_c::receive( void ) {
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------

} // namespace Analytics

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------
