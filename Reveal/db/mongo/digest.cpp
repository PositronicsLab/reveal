#include "Reveal/db/mongo/digest.h"

#include <mongo/client/dbclient.h>

#include "Reveal/core/scenario.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------
bool digest_c::fetch( Reveal::Core::digest_ptr& digest, Reveal::DB::database_ptr db ) {

  std::auto_ptr<mongo::DBClientCursor> cursor;
  Reveal::Core::scenario_ptr scenario;

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  mongo->fetch( cursor, "scenario", mongo::Query() );

  digest = Reveal::Core::digest_ptr( new Reveal::Core::digest_c() );

  if( !cursor->more() ) return false;

  while( cursor->more() ) {
    // add error handling
    mongo::BSONObj record = cursor->next();

    // create a new scenario from the database record
    // Note: at summarization level not detail
    scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );
   
    scenario->id = record.getField( "scenario_id" ).String();
    //scenario->trials = record.getField( "trials" ).Int();
    scenario->steps_per_trial = record.getField( "steps_per_trial" ).Int();
    scenario->description = record.getField( "description" ).String();
    // number of resources?

    scenario->trials = mongo->count( "trial", BSON( "scenario_id" << scenario->id ) );

    // add the scenario to the digest
    digest->add_scenario( scenario );
  }

  return true;
}

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


