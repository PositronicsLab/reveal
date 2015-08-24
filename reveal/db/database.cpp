#include "reveal/db/database.h"

//#include <assert.h>
//#include <stdio.h>

#include <cstdlib>
#include <iostream>
#include <memory>

#include <boost/pointer_cast.hpp>

#include "reveal/core/system.h"
#include "reveal/core/pointers.h"
#include "reveal/core/user.h"
#include "reveal/core/session.h"
#include "reveal/core/experiment.h"
#include "reveal/core/digest.h"
#include "reveal/core/scenario.h"
#include "reveal/core/trial.h"
#include "reveal/core/solution.h"
#include "reveal/core/solution_set.h"
#include "reveal/core/model.h"
#include "reveal/core/link.h"
#include "reveal/core/joint.h"

#include "reveal/db/mongo/mongo.h"
#include "reveal/db/mongo/user.h"
#include "reveal/db/mongo/session.h"
#include "reveal/db/mongo/digest.h"
#include "reveal/db/mongo/scenario.h"
#include "reveal/db/mongo/experiment.h"
#include "reveal/db/mongo/trial.h"
#include "reveal/db/mongo/solution.h"
#include "reveal/db/mongo/solution_set.h"
#include "reveal/db/mongo/analyzer.h"
#include "reveal/db/mongo/analysis.h"

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
database_c::error_e database_c::fetch( Reveal::Core::user_ptr& user, const std::string& user_id ) {

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
database_c::error_e database_c::fetch( Reveal::Core::session_ptr& session, std::string session_id ) {

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
database_c::error_e database_c::fetch( Reveal::Core::experiment_ptr& experiment, std::string experiment_id ) {

  // TODO : error handling
  Reveal::DB::Mongo::experiment_c::fetch( experiment, ptr(), experiment_id );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::fetch( Reveal::Core::digest_ptr& digest ) {
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
database_c::error_e database_c::fetch( Reveal::Core::scenario_ptr& scenario, const std::string& scenario_id ) {

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
database_c::error_e database_c::fetch( Reveal::Core::trial_ptr& trial, const std::string& scenario_id, double t, double epsilon ) {

//  printf( "t: %f\n", t );

  // TODO : error handling
  if( !Reveal::DB::Mongo::trial_c::fetch( trial, ptr(), scenario_id, t, epsilon ) )
    return ERROR_EMPTYSET;

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::insert( Reveal::Core::solution_ptr solution ) {

  // TODO : error handling
  Reveal::DB::Mongo::solution_c::insert( ptr(), solution );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::fetch( Reveal::Core::solution_ptr& solution, Reveal::Core::solution_c::type_e type, const std::string& scenario_id, double t, double epsilon ) {

  // TODO : error handling
  Reveal::DB::Mongo::solution_c::fetch( solution, ptr(), type, scenario_id, t, epsilon );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::fetch( Reveal::Core::solution_ptr& solution, const std::string& experiment_id, const std::string& scenario_id, double t, double epsilon ) {

  // TODO : error handling
  Reveal::DB::Mongo::solution_c::fetch( solution, ptr(), experiment_id, scenario_id, t, epsilon );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::insert( Reveal::Core::analyzer_ptr analyzer ) {

  // TODO : error handling
  Reveal::DB::Mongo::analyzer_c::insert( ptr(), analyzer );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::fetch( std::vector<Reveal::Core::analyzer_ptr>& analyzers ) {

  // TODO : error handling
  Reveal::DB::Mongo::analyzer_c::fetch( analyzers, ptr() );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::fetch( Reveal::Core::analyzer_ptr& analyzer, const std::string& scenario_id ) {

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
database_c::error_e database_c::fetch( Reveal::Core::solution_set_ptr& solution_set, const std::string& experiment_id ) {

  // TODO : error handling
  Reveal::DB::Mongo::solution_set_c::fetch( solution_set, ptr(), experiment_id );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::fetch( Reveal::Core::solution_set_ptr& solution_set, const std::string& experiment_id, double t, double epsilon ) {

  // TODO : error handling
  Reveal::DB::Mongo::solution_set_c::fetch( solution_set, ptr(), experiment_id, t, epsilon );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::fetch( Reveal::Core::solution_set_ptr& solution_set, const std::string& experiment_id, double trial_time, double solution_time, double epsilon ) {

  // TODO : error handling
  Reveal::DB::Mongo::solution_set_c::fetch( solution_set, ptr(), experiment_id, trial_time, solution_time, epsilon );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
}  // namespace DB
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------
