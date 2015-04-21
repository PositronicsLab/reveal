#include "Reveal/db/database.h"

//#include <assert.h>
//#include <stdio.h>

#include <cstdlib>
#include <iostream>
#include <memory>

#include <boost/pointer_cast.hpp>

#include "Reveal/core/system.h"
#include "Reveal/core/pointers.h"
#include "Reveal/core/user.h"
#include "Reveal/core/session.h"
#include "Reveal/core/experiment.h"
#include "Reveal/core/digest.h"
#include "Reveal/core/scenario.h"
#include "Reveal/core/trial.h"
#include "Reveal/core/solution.h"
#include "Reveal/core/solution_set.h"
#include "Reveal/core/model.h"
#include "Reveal/core/link.h"
#include "Reveal/core/joint.h"

#include "Reveal/db/mongo/mongo.h"
#include "Reveal/db/mongo/user.h"
#include "Reveal/db/mongo/session.h"
#include "Reveal/db/mongo/digest.h"
#include "Reveal/db/mongo/scenario.h"
#include "Reveal/db/mongo/experiment.h"
#include "Reveal/db/mongo/trial.h"
#include "Reveal/db/mongo/solution.h"
#include "Reveal/db/mongo/solution_set.h"
#include "Reveal/db/mongo/analyzer.h"
#include "Reveal/db/mongo/analysis.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
database_c::database_c( void ) {

}

//-----------------------------------------------------------------------------
database_c::~database_c( void ) {
  close();
}

//-----------------------------------------------------------------------------
database_ptr database_c::ptr( void ) {
  return shared_from_this();
}

//-----------------------------------------------------------------------------
service_ptr database_c::service( void ) {
  return _service;
}

//-----------------------------------------------------------------------------
bool database_c::open( void ) {

  // if service instantiated call its open method
  if( _service ) return _service->open();
    
  // otherwise, create a mongo instance and open it
  //Reveal::DB::Mongo::mongo_ptr mongo;
  //mongo = Reveal::DB::Mongo::mongo_ptr( new mongo_c() );
  //_service = boost::dynamic_pointer_cast<service_c>( mongo );
  _service = boost::dynamic_pointer_cast<service_c>( Reveal::DB::Mongo::mongo_ptr( new Reveal::DB::Mongo::mongo_c() ) );
  return _service->open();
}

//-----------------------------------------------------------------------------
void database_c::close( void ) {
  if( _service ) _service->close();
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::insert( Reveal::Core::user_ptr user ) {

  // TODO : error handling
  Reveal::DB::Mongo::user_c::insert( ptr(), user );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::query( Reveal::Core::user_ptr& user, const std::string& user_id ) {

  // TODO : error handling
  Reveal::DB::Mongo::user_c::fetch( user, ptr(), user_id );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::insert( Reveal::Core::session_ptr session ) {

  // TODO : error handling
  Reveal::DB::Mongo::session_c::insert( ptr(), session );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::query( Reveal::Core::session_ptr& session, std::string session_id ) {

  // TODO : error handling
  Reveal::DB::Mongo::session_c::fetch( session, ptr(), session_id );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::insert( Reveal::Core::experiment_ptr experiment ) {

  // TODO : error handling
  Reveal::DB::Mongo::experiment_c::insert( ptr(), experiment );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::query( Reveal::Core::experiment_ptr& experiment, std::string experiment_id ) {

  // TODO : error handling
  Reveal::DB::Mongo::experiment_c::fetch( experiment, ptr(), experiment_id );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::update_increment_trial_index( Reveal::Core::experiment_ptr experiment ) {

  // TODO : error handling
  Reveal::DB::Mongo::experiment_c::update_increment_trial_index( ptr(), experiment );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::query( Reveal::Core::digest_ptr& digest ) {
  // TODO : error handling
  Reveal::DB::Mongo::digest_c::fetch( digest, ptr() );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::insert( Reveal::Core::scenario_ptr scenario ) {

  // TODO : error handling
  Reveal::DB::Mongo::scenario_c::insert( ptr(), scenario );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::query( Reveal::Core::scenario_ptr& scenario, const std::string& scenario_id ) {

  // TODO : error handling
  Reveal::DB::Mongo::scenario_c::fetch( scenario, ptr(), scenario_id );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::insert( Reveal::Core::trial_ptr trial ) {

  // TODO : error handling
  Reveal::DB::Mongo::trial_c::insert( ptr(), trial );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::query( Reveal::Core::trial_ptr& trial, const std::string& scenario_id, unsigned trial_id ) {

  // TODO : error handling
  Reveal::DB::Mongo::trial_c::fetch( trial, ptr(), scenario_id, trial_id );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::insert( Reveal::Core::solution_ptr solution ) {

  // TODO : error handling
  Reveal::DB::Mongo::solution_c::insert( ptr(), solution );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::query( Reveal::Core::solution_ptr& solution, Reveal::Core::solution_c::type_e type, const std::string& scenario_id, int trial_id ) {

  // TODO : error handling
  Reveal::DB::Mongo::solution_c::fetch( solution, ptr(), type, scenario_id, trial_id );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::query( Reveal::Core::solution_ptr& solution, const std::string& experiment_id, const std::string& scenario_id, int trial_id ) {

  // TODO : error handling
  Reveal::DB::Mongo::solution_c::fetch( solution, ptr(), experiment_id, scenario_id, trial_id );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::insert( Reveal::Core::analyzer_ptr analyzer ) {

  // TODO : error handling
  Reveal::DB::Mongo::analyzer_c::insert( ptr(), analyzer );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::query( Reveal::Core::analyzer_ptr& analyzer, const std::string& scenario_id ) {

  // TODO : error handling
  Reveal::DB::Mongo::analyzer_c::fetch( analyzer, ptr(), scenario_id );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::insert( Reveal::Core::analysis_ptr analysis ) {

  // TODO : error handling
  Reveal::DB::Mongo::analysis_c::insert( ptr(), analysis );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::query( Reveal::Core::solution_set_ptr& solution_set, const std::string& experiment_id ) {

  // TODO : error handling
  Reveal::DB::Mongo::solution_set_c::fetch( solution_set, ptr(), experiment_id );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
}  // namespace DB
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------
