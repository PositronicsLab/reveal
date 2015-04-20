#include "Reveal/db/mongo/analyzer.h"

#include <mongo/client/dbclient.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
unsigned analyzer_c::count( Reveal::DB::database_ptr db ) {
  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return 0;

  // query mongo for count
  return mongo->count( "analyzer" );
}
//-----------------------------------------------------------------------------
bool analyzer_c::insert( Reveal::DB::database_ptr db, Reveal::Core::analyzer_ptr analyzer ) {

  mongo::BSONObjBuilder bob;
  bob.append( "scenario_id", analyzer->scenario_id );
  bob.append( "filename", analyzer->filename );
  bob.append( "type", (int) analyzer->type );

  mongo::BSONArrayBuilder bab_keys, bab_labels;
  for( unsigned i = 0; i < analyzer->keys.size(); i++ ) {
    bab_keys.append( analyzer->keys[i] );
  }
  bob.appendArray( "keys", bab_keys.arr() );

  for( unsigned i = 0; i < analyzer->labels.size(); i++ ) {
    bab_labels.append( analyzer->labels[i] );
  }
  bob.appendArray( "labels", bab_labels.arr() );

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  return mongo->insert( "analyzer", bob.obj() );
}

//-----------------------------------------------------------------------------
bool analyzer_c::fetch( Reveal::Core::analyzer_ptr& analyzer, Reveal::DB::database_ptr db, std::string scenario_id ) {

  std::auto_ptr<mongo::DBClientCursor> cursor;

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  mongo->fetch( cursor, "analyzer", QUERY( "scenario_id" << scenario_id ) );

  if( !cursor->more() ) return false;

  // add error handling
  mongo::BSONObj record = cursor->next();

  analyzer = Reveal::Core::analyzer_ptr( new Reveal::Core::analyzer_c() );
  analyzer->scenario_id = record.getField( "scenario_id" ).String();
  analyzer->filename = record.getField( "filename" ).String();
  analyzer->type = (Reveal::Core::analyzer_c::type_e) record.getField( "type" ).Int();

  return true;
}

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


