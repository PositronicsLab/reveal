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
void query_c::scenario( const std::string& name ) {
  _query = QUERY( "name" << name );
}

//-----------------------------------------------------------------------------
void query_c::trial( const std::string& scenario ) {
  _query = QUERY( "scenario" << scenario );
}

//-----------------------------------------------------------------------------
void query_c::trial( const std::string& scenario, const unsigned& index ) {
  _query = QUERY( "scenario" << scenario << "index" << index);
}

//-----------------------------------------------------------------------------
void query_c::solution( const std::string& scenario ) {
  _query = QUERY( "scenario" << scenario );
}

//-----------------------------------------------------------------------------
void query_c::solution( const std::string& scenario, const unsigned& index ) {
  _query = QUERY( "scenario" << scenario << "index" << index );
}

//-----------------------------------------------------------------------------
void query_c::model_solution( const std::string& scenario ) {
  _query = QUERY( "scenario" << scenario );
}

//-----------------------------------------------------------------------------
void query_c::model_solution( const std::string& scenario, const unsigned& index ) {
  _query = QUERY( "scenario" << scenario << "index" << index );
}

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
