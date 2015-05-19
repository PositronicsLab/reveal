#include "Reveal/db/mongo/experiment.h"

#include <mongo/client/dbclient.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------
unsigned experiment_c::count( Reveal::DB::database_ptr db ) {
  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return 0;

  // query mongo for count
  return mongo->count( "experiment" );
}
//-----------------------------------------------------------------------------
bool experiment_c::insert( Reveal::DB::database_ptr db, Reveal::Core::experiment_ptr experiment ) {
  mongo::BSONObj obj;

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  map( obj, experiment );

  return mongo->insert( "experiment", obj );
}

//-----------------------------------------------------------------------------
bool experiment_c::fetch( Reveal::Core::experiment_ptr& experiment, Reveal::DB::database_ptr db, std::string experiment_id ) {
  std::auto_ptr<mongo::DBClientCursor> cursor;

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  mongo->fetch( cursor, "experiment", QUERY( "experiment_id" << experiment_id ) );

  // if EMPTYSET returned, then session is not in the database
  if( !cursor->more() ) return false;

  // TODO:add error handling
  mongo::BSONObj record = cursor->next();

  map( experiment, record );

  return true;
}

//-----------------------------------------------------------------------------
bool experiment_c::update_increment_trial_index( database_ptr db, Reveal::Core::experiment_ptr experiment ) {
/*
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  mongo->update( "experiment", BSON( "experiment_id" << experiment->experiment_id ), BSON( "$inc" << BSON( "current_trial_index" << 1 ) ) );
*/
  return true;
}

//-----------------------------------------------------------------------------
bool experiment_c::map( Reveal::Core::experiment_ptr& experiment, mongo::BSONObj obj ) { 
  experiment = Reveal::Core::experiment_ptr( new Reveal::Core::experiment_c() );

  experiment->experiment_id = obj.getField( "experiment_id" ).String();
  experiment->session_id = obj.getField( "session_id" ).String();
  experiment->scenario_id = obj.getField( "scenario_id" ).String();
  experiment->start_time = obj.getField( "start_time" ).Double();
  experiment->end_time = obj.getField( "end_time" ).Double();
  experiment->time_step = obj.getField( "time_step" ).Double();
  experiment->epsilon = obj.getField( "epsilon" ).Double();
//  experiment->current_trial_index = obj.getField( "current_trial_index" ).Int();

  return true;
}

//-----------------------------------------------------------------------------
bool experiment_c::map( mongo::BSONObj& obj, Reveal::Core::experiment_ptr experiment ) {
  mongo::BSONObjBuilder bob;

  bob.append( "experiment_id", experiment->experiment_id );
  bob.append( "session_id", experiment->session_id );
  bob.append( "scenario_id", experiment->scenario_id );
  bob.append( "start_time", experiment->start_time );
  bob.append( "end_time", experiment->end_time );
  bob.append( "time_step", experiment->time_step );
  bob.append( "epsilon", experiment->epsilon );

//  bob.append( "current_trial_index", experiment->current_trial_index ); 

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


