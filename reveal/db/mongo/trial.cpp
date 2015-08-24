#include "reveal/db/mongo/trial.h"

#include <mongo/client/dbclient.h>

#include "reveal/db/mongo/model.h"
//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------
unsigned trial_c::count( Reveal::DB::database_ptr db ) {
  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return 0;

  // query mongo for count
  return mongo->count( "trial" );
}
//-----------------------------------------------------------------------------
bool trial_c::insert( Reveal::DB::database_ptr db, Reveal::Core::trial_ptr trial ) {
  mongo::BSONObj obj;

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  map( obj, trial );

  return mongo->insert( "trial", obj );
}

//-----------------------------------------------------------------------------
bool trial_c::fetch( Reveal::Core::trial_ptr& trial, Reveal::DB::database_ptr db, std::string scenario_id, double t, double epsilon ) {
  std::auto_ptr<mongo::DBClientCursor> cursor;
//  Reveal::DB::query_c query;
  Reveal::Core::scenario_ptr ptr;

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  double lbound = t - epsilon;
  double ubound = t + epsilon;

  //mongo->fetch( cursor, "trial", QUERY( "scenario_id" << scenario_id << "t" << t ) );
  mongo->fetch( cursor, "trial", QUERY( "scenario_id" << scenario_id << "t" << mongo::GTE << lbound << mongo::LTE << ubound) );

  if( !cursor->more() ) return false;

  // add error handling
  mongo::BSONObj record = cursor->next();

  map( trial, record );

  return true;
}

//-----------------------------------------------------------------------------
bool trial_c::map( Reveal::Core::trial_ptr& trial, mongo::BSONObj obj ) { 

  trial = Reveal::Core::trial_ptr( new Reveal::Core::trial_c() );

  trial->scenario_id = obj.getField( "scenario_id" ).String();
  trial->t = obj.getField( "t" ).Double();

  model_c::fetch( trial, obj );

  return true;
}

//-----------------------------------------------------------------------------
bool trial_c::map( mongo::BSONObj& obj, Reveal::Core::trial_ptr trial ) {

  mongo::BSONObjBuilder bob;
  bob.append( "scenario_id", trial->scenario_id );
  bob.append( "t", trial->t );

  model_c::insert( bob, trial->models, true );

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


