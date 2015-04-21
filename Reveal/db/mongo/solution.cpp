#include "Reveal/db/mongo/solution.h"

#include <mongo/client/dbclient.h>

#include "Reveal/db/mongo/model.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------
bool solution_c::insert( Reveal::DB::database_ptr db, Reveal::Core::solution_ptr solution ) {
  std::string table;

  mongo::BSONObjBuilder bob;
  if( solution->type == Reveal::Core::solution_c::CLIENT )
    bob.append( "experiment_id", solution->experiment_id );
  bob.append( "scenario_id", solution->scenario_id );
  bob.append( "trial_id", solution->trial_id );
  bob.append( "t", solution->t );
  bob.append( "dt", solution->dt );

  model_c::insert( bob, solution->models, false );

  if( solution->type == Reveal::Core::solution_c::CLIENT )
    table = "solution";
  else if( solution->type == Reveal::Core::solution_c::MODEL )
    table = "model";

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  return mongo->insert( table, bob.obj() );
}

//-----------------------------------------------------------------------------
bool solution_c::fetch( Reveal::Core::solution_ptr& solution, Reveal::DB::database_ptr db, Reveal::Core::solution_c::type_e type, std::string scenario_id, unsigned trial_id ) {
  std::auto_ptr<mongo::DBClientCursor> cursor;
  Reveal::Core::scenario_ptr ptr;
  std::string table;

  if( type == Reveal::Core::solution_c::CLIENT )
    table = "solution";
  else if( type == Reveal::Core::solution_c::MODEL )
    table = "model";

  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  mongo->fetch( cursor, table, QUERY( "scenario_id" << scenario_id << "trial_id" << trial_id ) );

  if( !cursor->more() ) return false;

  // add error handling
  mongo::BSONObj record = cursor->next();

  solution = Reveal::Core::solution_ptr( new Reveal::Core::solution_c( type ) );

  if( type == Reveal::Core::solution_c::CLIENT )
    solution->experiment_id = record.getField( "experiment_id" ).String();
  solution->scenario_id = record.getField( "scenario_id" ).String();
  solution->trial_id = record.getField( "trial_id" ).Int();
  solution->t = record.getField( "t" ).Double();
  solution->dt = record.getField( "dt" ).Double();

  model_c::fetch( solution, record );

  return true;
}

//-----------------------------------------------------------------------------
bool solution_c::fetch( Reveal::Core::solution_ptr& solution, Reveal::DB::database_ptr db, std::string experiment_id, std::string scenario_id, unsigned trial_id ) {
  std::auto_ptr<mongo::DBClientCursor> cursor;
  Reveal::Core::scenario_ptr ptr;
  std::string table = "solution";

  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  mongo->fetch( cursor, table, QUERY( "experiment_id" << experiment_id << "scenario_id" << scenario_id << "trial_id" << trial_id ) );

  if( !cursor->more() ) return false;

  // add error handling
  mongo::BSONObj record = cursor->next();

  solution = Reveal::Core::solution_ptr( new Reveal::Core::solution_c( Reveal::Core::solution_c::CLIENT ) );

  solution->experiment_id = record.getField( "experiment_id" ).String();
  solution->scenario_id = record.getField( "scenario_id" ).String();
  solution->trial_id = record.getField( "trial_id" ).Int();
  solution->t = record.getField( "t" ).Double();
  solution->dt = record.getField( "dt" ).Double();

  model_c::fetch( solution, record );

  return true;
}

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


