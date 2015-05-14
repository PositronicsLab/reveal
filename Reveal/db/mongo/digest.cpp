#include "Reveal/db/mongo/digest.h"

#include <mongo/client/dbclient.h>

#include "Reveal/core/scenario.h"
#include "Reveal/db/mongo/scenario.h"

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

    if( Reveal::DB::Mongo::scenario_c::map( scenario, record ) ) {
      assert( scenario );  // sanity check

      // add the scenario to the digest
      digest->add_scenario( scenario );
    }
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


