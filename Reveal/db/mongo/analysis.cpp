#include "Reveal/db/mongo/analysis.h"

#include <mongo/client/dbclient.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
bool analysis_c::insert( Reveal::DB::database_ptr db, Reveal::Core::analysis_ptr analysis ) {

  for( unsigned i = 0; i < analysis->count_rows(); i++ ) {

    mongo::BSONObjBuilder bob;

    bob.append( "session_id", analysis->experiment->session_id );
    bob.append( "experiment_id", analysis->experiment->experiment_id );
    bob.append( "scenario_id", analysis->experiment->scenario_id );

    mongo::BSONArrayBuilder bab_values;
    mongo::BSONObjBuilder bob_value;
    for( unsigned j = 0; j < analysis->count_keys(); j++ ) {
      bob_value.append( analysis->key( j ), analysis->value( i, j ) );
    }
    bab_values.append( bob_value.obj() );
    bob.appendArray( "values", bab_values.arr() );

    // get mongo service and verify
    mongo_ptr mongo = mongo_c::service( db );
    if( !mongo ) return false;

    return mongo->insert( "analysis", bob.obj() );
  }

  return true;
}

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


