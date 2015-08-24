#include "reveal/db/mongo/analyzer.h"

#include <mongo/client/dbclient.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
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
  mongo::BSONObj obj;

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  map( obj, analyzer );

  return mongo->insert( "analyzer", obj );
}

//-----------------------------------------------------------------------------
bool analyzer_c::fetch( std::vector<Reveal::Core::analyzer_ptr>& analyzers, Reveal::DB::database_ptr db ) {

  std::auto_ptr<mongo::DBClientCursor> cursor;

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  mongo->fetch( cursor, "analyzer", mongo::BSONObj( ) );
  //mongo->fetch( cursor, "analyzer", QUERY( "scenario_id" << scenario_id ) );

  if( !cursor->more() ) return false;

  do {
    Reveal::Core::analyzer_ptr analyzer;

    // add error handling
    mongo::BSONObj record = cursor->next();

    map( analyzer, record );

    analyzers.push_back( analyzer );
  } while( cursor->more() );

  return true;
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

  map( analyzer, record );

  return true;
}

//-----------------------------------------------------------------------------
bool analyzer_c::map( Reveal::Core::analyzer_ptr& analyzer, mongo::BSONObj obj ) { 
  analyzer = Reveal::Core::analyzer_ptr( new Reveal::Core::analyzer_c() );

  analyzer->scenario_id = obj.getField( "scenario_id" ).String();
  analyzer->source_path = obj.getField( "source_path" ).String();
  analyzer->build_path = obj.getField( "build_path" ).String();
  analyzer->build_target = obj.getField( "build_target" ).String();
  analyzer->type = (Reveal::Core::analyzer_c::type_e) obj.getField( "type" ).Int();

  // TODO: map keys and labels

  return true;
}

//-----------------------------------------------------------------------------
bool analyzer_c::map( mongo::BSONObj& obj, Reveal::Core::analyzer_ptr analyzer ) {
  mongo::BSONObjBuilder bob;
  bob.append( "scenario_id", analyzer->scenario_id );
  bob.append( "source_path", analyzer->source_path );
  bob.append( "build_path", analyzer->build_path );
  bob.append( "build_target", analyzer->build_target );
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


