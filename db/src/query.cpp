#include <Reveal/query.h>

#include <assert.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
query_c::query_c( void ) {

}

//-----------------------------------------------------------------------------
query_c::~query_c( void ) {

}

//-----------------------------------------------------------------------------
mongo::Query query_c::operator()( void ) {
  return _query;
}

//-----------------------------------------------------------------------------
void query_c::operator()( mongo::Query query ) {
  _query = query;
}

//-----------------------------------------------------------------------------
void query_c::digest( void ) {
  //_query = QUERY( "name" << name );
  // TODO: SQL equivalent 'scenario *'
}

//-----------------------------------------------------------------------------
void query_c::scenario( int scenario_id ) {
  _query = QUERY( "id" << scenario_id );
}

//-----------------------------------------------------------------------------
void query_c::scenario( const std::string& name ) {
  _query = QUERY( "name" << name );
}

//-----------------------------------------------------------------------------
void query_c::trial( int scenario_id, int trial_id ) {
  _query = QUERY( "scenario_id" << scenario_id << "trial_id" << trial_id);
}
/*
//-----------------------------------------------------------------------------
void query_c::solution( const std::string& scenario ) {
  _query = QUERY( "scenario" << scenario );
}
*/
//-----------------------------------------------------------------------------
void query_c::solution( int scenario_id, int trial_id ) {
  _query = QUERY( "scenario_id" << scenario_id << "trial_id" << trial_id );
}
/*
//-----------------------------------------------------------------------------
void query_c::model_solution( unsigned scenario_id ) {
  _query = QUERY( "scenario_id" << scenario_id );
}
*/
//-----------------------------------------------------------------------------
void query_c::model_solution( int scenario_id, int trial_id ) {
  _query = QUERY( "scenario_id" << scenario_id << "trial_id" << trial_id );
}

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
