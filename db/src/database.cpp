#include <Reveal/database.h>

//#include <assert.h>
//#include <stdio.h>

#include <cstdlib>
#include <iostream>
#include <memory>

#include <mongo/client/dbclient.h>

#include <Reveal/query.h>
#include <Reveal/digest.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>
#include <Reveal/solution_set.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace DB {

//-----------------------------------------------------------------------------
database_c::database_c( void ) {
  _open = false;
  _host = "localhost";
  _port = 27017;
  //_dbname = "revealdb";
  _dbname = "test";
}

//-----------------------------------------------------------------------------
database_c::database_c( const std::string& host ) {
  _open = false;
  _host = host;
  _port = 27017;
//  _dbname = "revealdb";
  _dbname = "test";
}

//-----------------------------------------------------------------------------
database_c::~database_c( void ) {
  if( _open ) close();
}

//-----------------------------------------------------------------------------
bool database_c::open( void ) {
  if( _open ) return false;

  try {
    _connection.connect( _host );
  } catch( const mongo::DBException &ex ) {
    std::cout << "ERROR: Failed to open connection to database host: " << ex.what() << std::endl;
    return false;
  }

  _open = true;
  return _open;
}

//-----------------------------------------------------------------------------
void database_c::close( void ) {

  if( !_open ) return;
  
/*
  if( _connection.isStillConnected() )
  
  mongo::BSONObj info;
  _connection.logout(  )
*/

  _open = false;
}

//-----------------------------------------------------------------------------
bool database_c::insert( const std::string& table, const mongo::BSONObj& query ) {
  std::string document;

  document = _dbname + "." + table;

  _connection.insert( document, query );

  return true;
}

//-----------------------------------------------------------------------------
bool database_c::fetch( std::auto_ptr<mongo::DBClientCursor>& cursor, const std::string& table, mongo::Query query ) {
  std::string document;

  document = _dbname + "." + table; 

  cursor = _connection.query( document, query );

  return true;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::query( Reveal::Core::digest_ptr& digest ) {

  std::auto_ptr<mongo::DBClientCursor> cursor;
  Reveal::DB::query_c query;
  Reveal::Core::scenario_ptr scenario;

  query.digest( );
  fetch( cursor, "scenario", query() );

  digest = Reveal::Core::digest_ptr( new Reveal::Core::digest_c() );

  if( !cursor->more() ) return ERROR_EMPTYSET;

  while( cursor->more() ) {
    // add error handling
    mongo::BSONObj record = cursor->next();

    // create a new scenario from the database record
    // Note: at summarization level not detail
    scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );
   
    scenario->id = record.getField( "id" ).Int();
    scenario->name = record.getField( "name" ).String();
    scenario->trials = record.getField( "trials" ).Int();
    scenario->description = record.getField( "description" ).String();
    // number of resources?

    // add the scenario to the digest
    digest->add_scenario( scenario );
  }

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::insert( Reveal::Core::scenario_ptr scenario ) {

  mongo::BSONObjBuilder bob_scenario;
  mongo::BSONArrayBuilder bab_scenario_uris;

  bob_scenario.append( "id", scenario->id );
  bob_scenario.append( "name", scenario->name );
  bob_scenario.append( "description", scenario->description );
  bob_scenario.append( "trials", scenario->trials );
  for( unsigned uri = 0; uri < scenario->uris.size(); uri++ )
    bab_scenario_uris.append( scenario->uris[uri] );
  bob_scenario.appendArray( "uris", bab_scenario_uris.arr() );

  mongo::BSONObj scenario_query = bob_scenario.obj();

  insert( "scenario", scenario_query );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::query( Reveal::Core::scenario_ptr& scenario, const std::string& name ) {
/*
  std::auto_ptr<mongo::DBClientCursor> cursor;
  Reveal::DB::query_c query;
  //Reveal::Core::scenario_ptr ptr;

  query.scenario( name );
  fetch( cursor, "scenario", query() );

  // add error handling
  mongo::BSONObj record = cursor->next();

  scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );
  scenario->name = record.getField( "name" ).String();
  scenario->trials = record.getField( "trials" ).Int();

  mongo::BSONObj bson_uris = record.getObjectField( "uris" );
  std::vector<mongo::BSONElement> vec_uris;
  bson_uris.elems( vec_uris );

  for( unsigned i = 0; i < vec_uris.size(); i++ )
    scenario->uris.push_back( vec_uris[i].String() );
*/
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::query( Reveal::Core::scenario_ptr& scenario, int scenario_id ) {

  std::auto_ptr<mongo::DBClientCursor> cursor;
  Reveal::DB::query_c query;
  //Reveal::Core::scenario_ptr ptr;

  query.scenario( scenario_id );
  fetch( cursor, "scenario", query() );

  if( !cursor->more() ) return ERROR_EMPTYSET;

  // TODO:add error handling
  mongo::BSONObj record = cursor->next();

  scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );
  scenario->id = record.getField( "id" ).Int();
  scenario->name = record.getField( "name" ).String();
  scenario->description = record.getField( "description" ).String();
  scenario->trials = record.getField( "trials" ).Int();

  mongo::BSONObj bson_uris = record.getObjectField( "uris" );
  std::vector<mongo::BSONElement> vec_uris;
  bson_uris.elems( vec_uris );

  for( unsigned i = 0; i < vec_uris.size(); i++ )
    scenario->uris.push_back( vec_uris[i].String() );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::insert( Reveal::Core::trial_ptr trial ) {

  mongo::BSONObjBuilder bob_trial;
  bob_trial.append( "scenario_id", trial->scenario_id );
  bob_trial.append( "trial_id", trial->trial_id );
  bob_trial.append( "t", trial->t );
  bob_trial.append( "dt", trial->dt );
  mongo::BSONArrayBuilder bab_trial_state_q;
  for( unsigned iq = 0; iq < trial->state.size_q(); iq++ ) 
    bab_trial_state_q.append( trial->state.q(iq) );
  bob_trial.appendArray( "state_q", bab_trial_state_q.arr() );
  mongo::BSONArrayBuilder bab_trial_state_dq;
  for( unsigned idq = 0; idq < trial->state.size_dq(); idq++ ) 
    bab_trial_state_dq.append( trial->state.dq(idq) );
  bob_trial.appendArray( "state_dq", bab_trial_state_dq.arr() );

  mongo::BSONObj trial_query = bob_trial.obj();
  
  insert( "trial", trial_query );  

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::query( Reveal::Core::trial_ptr& trial, int scenario_id, int trial_id ) {

  std::auto_ptr<mongo::DBClientCursor> cursor;
  Reveal::DB::query_c query;
  Reveal::Core::scenario_ptr ptr;

  query.trial( scenario_id, trial_id );
  fetch( cursor, "trial", query() );

  if( !cursor->more() ) return ERROR_EMPTYSET;

  // add error handling
  mongo::BSONObj record = cursor->next();

  trial = Reveal::Core::trial_ptr( new Reveal::Core::trial_c() );
  trial->scenario_id = record.getField( "scenario_id" ).Int();
  trial->trial_id = record.getField( "trial_id" ).Int();
  trial->t = record.getField( "t" ).Double();
  trial->dt = record.getField( "dt" ).Double();

  mongo::BSONObj bson_state_q = record.getObjectField( "state_q" );
  std::vector<mongo::BSONElement> vec_state_q;
  bson_state_q.elems( vec_state_q );
  for( unsigned i = 0; i < vec_state_q.size(); i++ )
    trial->state.append_q( vec_state_q[i].Double() );

  mongo::BSONObj bson_state_dq = record.getObjectField( "state_dq" );
  std::vector<mongo::BSONElement> vec_state_dq;
  bson_state_dq.elems( vec_state_dq );
  for( unsigned i = 0; i < vec_state_dq.size(); i++ )
    trial->state.append_dq( vec_state_dq[i].Double() );

  mongo::BSONObj bson_control_u = record.getObjectField( "control_u" );
  std::vector<mongo::BSONElement> vec_control_u;
  bson_control_u.elems( vec_control_u );
  for( unsigned i = 0; i < vec_control_u.size(); i++ )
    trial->control.append_u( vec_control_u[i].Double() );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::insert( Reveal::Core::solution_ptr solution ) {

  std::string table;

  mongo::BSONObjBuilder bob_solution;
  bob_solution.append( "scenario_id", solution->scenario_id );
  bob_solution.append( "trial_id", solution->trial_id );
  bob_solution.append( "t", solution->t );
  mongo::BSONArrayBuilder bab_solution_state_q;
  for( unsigned iq = 0; iq < solution->state.size_q(); iq++ ) 
    bab_solution_state_q.append( solution->state.q(iq) );
  bob_solution.appendArray( "state_q", bab_solution_state_q.arr() );
  mongo::BSONArrayBuilder bab_solution_state_dq;
  for( unsigned idq = 0; idq < solution->state.size_dq(); idq++ ) 
    bab_solution_state_dq.append( solution->state.dq(idq) );
  bob_solution.appendArray( "state_dq", bab_solution_state_dq.arr() );

  mongo::BSONObj solution_query = bob_solution.obj();
  
  if( solution->type == Reveal::Core::solution_c::CLIENT )
    table = "solution";
  else if( solution->type == Reveal::Core::solution_c::MODEL )
    table = "model";

  insert( table, solution_query );  

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::query( Reveal::Core::solution_ptr& solution, Reveal::Core::solution_c::type_e type, int scenario_id, int trial_id ) {

  std::auto_ptr<mongo::DBClientCursor> cursor;
  Reveal::DB::query_c query;
  Reveal::Core::scenario_ptr ptr;
  std::string table;

  if( type == Reveal::Core::solution_c::CLIENT )
    table = "solution";
  else if( type == Reveal::Core::solution_c::MODEL )
    table = "model";

  query.solution( scenario_id, trial_id );
  fetch( cursor, table, query() );

  if( !cursor->more() ) return ERROR_EMPTYSET;

  // add error handling
  mongo::BSONObj record = cursor->next();

  solution = Reveal::Core::solution_ptr( new Reveal::Core::solution_c( type ) );
  solution->scenario_id = record.getField( "scenario_id" ).Int();
  solution->trial_id = record.getField( "trial_id" ).Int();
  solution->t = record.getField( "t" ).Double();

  mongo::BSONObj bson_state_q = record.getObjectField( "state_q" );
  std::vector<mongo::BSONElement> vec_state_q;
  bson_state_q.elems( vec_state_q );
  for( unsigned i = 0; i < vec_state_q.size(); i++ )
    solution->state.append_q( vec_state_q[i].Double() );

  mongo::BSONObj bson_state_dq = record.getObjectField( "state_dq" );
  std::vector<mongo::BSONElement> vec_state_dq;
  bson_state_dq.elems( vec_state_dq );
  for( unsigned i = 0; i < vec_state_dq.size(); i++ )
    solution->state.append_dq( vec_state_dq[i].Double() );

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::insert( Reveal::Core::analyzer_ptr analyzer ) {

  mongo::BSONObjBuilder bob_analyzer;
  bob_analyzer.append( "scenario_id", analyzer->scenario_id );
  bob_analyzer.append( "filename", analyzer->filename );
  bob_analyzer.append( "type", (int) analyzer->type );

  mongo::BSONObj analyzer_query = bob_analyzer.obj();
  
  insert( "analyzer", analyzer_query );  

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::insert( Reveal::Core::analysis_ptr analysis ) {

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::query( Reveal::Core::analyzer_ptr& analyzer, int scenario_id ) {

  std::auto_ptr<mongo::DBClientCursor> cursor;
  Reveal::DB::query_c query;
  //Reveal::Core::scenario_ptr ptr;

  query.analyzer( scenario_id );
  fetch( cursor, "analyzer", query() );

  if( !cursor->more() ) return ERROR_EMPTYSET;

  // add error handling
  mongo::BSONObj record = cursor->next();

  analyzer = Reveal::Core::analyzer_ptr( new Reveal::Core::analyzer_c() );
  analyzer->scenario_id = record.getField( "scenario_id" ).Int();
  analyzer->filename = record.getField( "filename" ).String();
  analyzer->type = (Reveal::Core::analyzer_c::type_e) record.getField( "type" ).Int();

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
database_c::error_e database_c::query( Reveal::Core::solution_set_ptr& solution_set, int scenario_id, int session_id ) {

  // NOTE: may need to order queries such that indices match up in each query

  Reveal::Core::scenario_ptr scenario;
  Reveal::Core::trial_ptr trial;
  Reveal::Core::solution_ptr solution, model;
  error_e error;

  solution_set = Reveal::Core::solution_set_ptr( new Reveal::Core::solution_set_c() );

  error = query( scenario, scenario_id );
  // TODO : error check and handle
  // If above succeeds or recovers then proceed else bomb out

  solution_set->scenario = scenario;

  for( unsigned i = 0; i < scenario->trials; i++ ) {
    error = query( trial, scenario_id, i );
    // TODO : error check and handle
    // contingent on above success
    solution_set->add_trial( trial );

    // TODO: add in session data 
    error = query( solution, Reveal::Core::solution_c::CLIENT, scenario_id, i );
    // TODO : error check and handle
    // contingent on above success
    solution_set->add_solution( solution );

    error = query( model, Reveal::Core::solution_c::MODEL, scenario_id, i );
    // TODO : error check and handle
    // contingent on above success
    solution_set->add_model( model );
  }

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------

}  // namespace DB

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------
