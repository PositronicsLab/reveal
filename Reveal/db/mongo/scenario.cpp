#include "Reveal/db/mongo/scenario.h"

#include <mongo/client/dbclient.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------
unsigned scenario_c::count( Reveal::DB::database_ptr db ) {
  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return 0;

  // query mongo for count
  return mongo->count( "scenario" );
}
//-----------------------------------------------------------------------------
bool scenario_c::insert( Reveal::DB::database_ptr db, Reveal::Core::scenario_ptr scenario ) {

  mongo::BSONObj obj;
  map( obj, scenario );

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  return mongo->insert( "scenario", obj );
}

//-----------------------------------------------------------------------------
bool scenario_c::fetch( Reveal::Core::scenario_ptr& scenario, Reveal::DB::database_ptr db, std::string scenario_id ) {

  std::auto_ptr<mongo::DBClientCursor> cursor;
  //Reveal::DB::query_c query;
  //Reveal::Core::scenario_ptr ptr;

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  // query mongo for cursor
  mongo->fetch( cursor, "scenario", QUERY( "scenario_id" << scenario_id ) );

  if( !cursor->more() ) return false;

  // TODO:add error handling
  mongo::BSONObj record = cursor->next();

  map( scenario, record );

  return true;
}

//-----------------------------------------------------------------------------
bool scenario_c::map( Reveal::Core::scenario_ptr& scenario, mongo::BSONObj obj ) {
  scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );

  scenario->id = obj.getField( "scenario_id" ).String();
  scenario->description = obj.getField( "description" ).String();
  scenario->sample_rate = obj.getField( "sample_rate" ).Double();
  scenario->sample_start_time = obj.getField( "sample_start_time" ).Double();
  scenario->sample_end_time = obj.getField( "sample_end_time" ).Double();

  mongo::BSONObj bson_uris = obj.getObjectField( "uris" );
  std::vector<mongo::BSONElement> vec_uris;
  bson_uris.elems( vec_uris );

  for( unsigned i = 0; i < vec_uris.size(); i++ )
    scenario->uris.push_back( vec_uris[i].String() );

  return true; 
}

//-----------------------------------------------------------------------------
bool scenario_c::map( mongo::BSONObj& obj, Reveal::Core::scenario_ptr scenario ) {
  mongo::BSONObjBuilder bob;
  mongo::BSONArrayBuilder bab;

  bob.append( "scenario_id", scenario->id );
  bob.append( "description", scenario->description );
  bob.append( "sample_rate", scenario->sample_rate );
  bob.append( "sample_start_time", scenario->sample_start_time );
  bob.append( "sample_end_time", scenario->sample_end_time );

  for( unsigned uri = 0; uri < scenario->uris.size(); uri++ )
    bab.append( scenario->uris[uri] );
  bob.appendArray( "uris", bab.arr() );

  obj = bob.obj();

  return true; 
}

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


