#include "Reveal/db/mongo/experiment.h"

#include <mongo/client/dbclient.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
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
  mongo::BSONObjBuilder bob;

  bob.append( "experiment_id", experiment->experiment_id );
  bob.append( "session_id", experiment->session_id );
  bob.append( "scenario_id", experiment->scenario_id );
  bob.append( "trials", experiment->number_of_trials );
  bob.append( "steps_per_trial", experiment->steps_per_trial );

  // TODO: need a loop to serialize or a subdocument
  //bob_experiment.append( "trial_prescription", experiment->trial_prescription );
  bob.append( "current_trial_index", experiment->current_trial_index ); 

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  return mongo->insert( "experiment", bob.obj() );
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

  experiment = Reveal::Core::experiment_ptr( new Reveal::Core::experiment_c() );

  // TODO: TODO: TODO: determine best datatype for A) storage and B) passing

  experiment->experiment_id = record.getField( "experiment_id" ).String();
  experiment->session_id = record.getField( "session_id" ).String();
  experiment->scenario_id = record.getField( "scenario_id" ).String();
  experiment->number_of_trials = record.getField( "trials" ).Int();
  experiment->steps_per_trial = record.getField( "steps_per_trial" ).Int();
  // TODO : subdocument for trial prescription
  experiment->current_trial_index = record.getField( "current_trial_index" ).Int();
 
  // TODO expand as needed

  return true;
}

//-----------------------------------------------------------------------------
bool experiment_c::update_increment_trial_index( database_ptr db, Reveal::Core::experiment_ptr experiment ) {

  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  mongo->update( "experiment", BSON( "experiment_id" << experiment->experiment_id ), BSON( "$inc" << BSON( "current_trial_index" << 1 ) ) );

  return true;
}

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


