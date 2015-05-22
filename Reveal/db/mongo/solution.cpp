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
  mongo::BSONObj obj;

  if( solution->type == Reveal::Core::solution_c::CLIENT )
    table = "solution";
  else if( solution->type == Reveal::Core::solution_c::MODEL )
    table = "model";

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  map( obj, solution );

  return mongo->insert( table, obj );
}

//-----------------------------------------------------------------------------
bool solution_c::fetch( Reveal::Core::solution_ptr& solution, Reveal::DB::database_ptr db, Reveal::Core::solution_c::type_e type, std::string scenario_id, double t, double epsilon ) {
  std::auto_ptr<mongo::DBClientCursor> cursor;
  Reveal::Core::scenario_ptr ptr;
  std::string table;

  if( type == Reveal::Core::solution_c::CLIENT )
    table = "solution";
  else if( type == Reveal::Core::solution_c::MODEL )
    table = "model";

  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  double lbound = t - epsilon;
  double ubound = t + epsilon;

  mongo->fetch( cursor, table, QUERY( "scenario_id" << scenario_id << "t" << mongo::GTE << lbound << mongo::LTE << ubound ) );

  if( !cursor->more() ) return false;

  // TODO: add error handling
  mongo::BSONObj record = cursor->next();

  map( solution, record, type );

  return true;
}

//-----------------------------------------------------------------------------
bool solution_c::fetch( Reveal::Core::solution_ptr& solution, Reveal::DB::database_ptr db, std::string experiment_id, std::string scenario_id, double t, double epsilon ) {
  std::auto_ptr<mongo::DBClientCursor> cursor;
  Reveal::Core::scenario_ptr ptr;
  std::string table = "solution";

  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  double lbound = t - epsilon;
  double ubound = t + epsilon;

  mongo->fetch( cursor, table, QUERY( "experiment_id" << experiment_id << "scenario_id" << scenario_id << "t" << mongo::GTE << lbound << mongo::LTE << ubound ) );

  if( !cursor->more() ) return false;

  // TODO: add error handling
  mongo::BSONObj record = cursor->next();

  map( solution, record, Reveal::Core::solution_c::CLIENT );

  return true;
}

//-----------------------------------------------------------------------------
bool solution_c::map( Reveal::Core::solution_ptr& solution, mongo::BSONObj obj, Reveal::Core::solution_c::type_e type ) { 
  solution = Reveal::Core::solution_ptr( new Reveal::Core::solution_c( type ) );

  if( type == Reveal::Core::solution_c::CLIENT )
    solution->experiment_id = obj.getField( "experiment_id" ).String();
  solution->scenario_id = obj.getField( "scenario_id" ).String();
  solution->t = obj.getField( "t" ).Double();
  if( type == Reveal::Core::solution_c::CLIENT )
    solution->real_time = obj.getField( "real_time" ).Double();

  model_c::fetch( solution, obj );

  return true;
}

//-----------------------------------------------------------------------------
bool solution_c::map( mongo::BSONObj& obj, Reveal::Core::solution_ptr solution ) {
  mongo::BSONObjBuilder bob;
  if( solution->type == Reveal::Core::solution_c::CLIENT )
    bob.append( "experiment_id", solution->experiment_id );
  bob.append( "scenario_id", solution->scenario_id );
  bob.append( "t", solution->t );
  if( solution->type == Reveal::Core::solution_c::CLIENT )
    bob.append( "real_time", solution->real_time );

  model_c::insert( bob, solution->models, false );

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


