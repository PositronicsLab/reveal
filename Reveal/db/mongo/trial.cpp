#include "Reveal/db/mongo/trial.h"

#include <mongo/client/dbclient.h>

#include "Reveal/db/mongo/model.h"
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

  mongo::BSONObjBuilder bob;
  bob.append( "scenario_id", trial->scenario_id );
  bob.append( "trial_id", trial->trial_id );
  bob.append( "t", trial->t );
  bob.append( "dt", trial->dt );

  model_c::insert( bob, trial->models, true );

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  return mongo->insert( "trial", bob.obj() );
}

//-----------------------------------------------------------------------------
bool trial_c::fetch( Reveal::Core::trial_ptr& trial, Reveal::DB::database_ptr db, std::string scenario_id, double t ) {
  std::auto_ptr<mongo::DBClientCursor> cursor;
//  Reveal::DB::query_c query;
  Reveal::Core::scenario_ptr ptr;

//  query.trial( scenario_id, trial_id );
//  fetch( cursor, "trial", query() );

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  mongo->fetch( cursor, "trial", QUERY( "scenario_id" << scenario_id << "t" << t ) );

  if( !cursor->more() ) return false;

  // add error handling
  mongo::BSONObj record = cursor->next();

  trial = Reveal::Core::trial_ptr( new Reveal::Core::trial_c() );
  trial->scenario_id = record.getField( "scenario_id" ).String();
  trial->trial_id = record.getField( "trial_id" ).Int();
  trial->t = record.getField( "t" ).Double();
  trial->dt = record.getField( "dt" ).Double();

  model_c::fetch( trial, record );

  trial->print();

  return true;
}
/*
//-----------------------------------------------------------------------------
bool trial_c::fetch( Reveal::Core::trial_ptr& trial, Reveal::DB::database_ptr db, std::string scenario_id, unsigned trial_id ) {
  std::auto_ptr<mongo::DBClientCursor> cursor;
//  Reveal::DB::query_c query;
  Reveal::Core::scenario_ptr ptr;

//  query.trial( scenario_id, trial_id );
//  fetch( cursor, "trial", query() );

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  mongo->fetch( cursor, "trial", QUERY( "scenario_id" << scenario_id << "trial_id" << trial_id ) );

  if( !cursor->more() ) return false;

  // add error handling
  mongo::BSONObj record = cursor->next();

  trial = Reveal::Core::trial_ptr( new Reveal::Core::trial_c() );
  trial->scenario_id = record.getField( "scenario_id" ).String();
  trial->trial_id = record.getField( "trial_id" ).Int();
  trial->t = record.getField( "t" ).Double();
  trial->dt = record.getField( "dt" ).Double();

  model_c::fetch( trial, record );

  return true;
}
*/
//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


