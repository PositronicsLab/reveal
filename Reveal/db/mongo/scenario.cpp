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
  mongo::BSONObjBuilder bob;
  mongo::BSONArrayBuilder bab;

  bob.append( "scenario_id", scenario->id );
  bob.append( "description", scenario->description );
  //bob.append( "trials", scenario->trials );
  bob.append( "steps_per_trial", scenario->steps_per_trial );

  for( unsigned uri = 0; uri < scenario->uris.size(); uri++ )
    bab.append( scenario->uris[uri] );
  bob.appendArray( "uris", bab.arr() );

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  // ask mongo to insert
  return mongo->insert( "scenario", bob.obj() );
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

  scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );
  scenario->id = record.getField( "scenario_id" ).String();
  scenario->description = record.getField( "description" ).String();
//  scenario->trials = record.getField( "trials" ).Int();
  scenario->steps_per_trial = record.getField( "steps_per_trial" ).Int();

  mongo::BSONObj bson_uris = record.getObjectField( "uris" );
  std::vector<mongo::BSONElement> vec_uris;
  bson_uris.elems( vec_uris );

  for( unsigned i = 0; i < vec_uris.size(); i++ )
    scenario->uris.push_back( vec_uris[i].String() );

  scenario->trials = mongo->count( "trial", BSON( "scenario_id" << scenario_id ) );

  //printf( "number_of_trials: %u\n", scenario->trials );

  return true;
}

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


