#include "Reveal/db/mongo/session.h"

#include <mongo/client/dbclient.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------
unsigned session_c::count( Reveal::DB::database_ptr db ) {
  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return 0;

  // query mongo for count
  return mongo->count( "session" );
}
//-----------------------------------------------------------------------------
bool session_c::insert( Reveal::DB::database_ptr db, Reveal::Core::session_ptr session ) {
  mongo::BSONObjBuilder bob;

  bob.append( "session_id", session->session_id );
  if( session->user_type == Reveal::Core::session_c::ANONYMOUS ) {
    bob.append( "user_type", 0 );
  } else {
    bob.append( "user_type", 1 );
    // may want sanity check that user_id is not empty here.
    bob.append( "user_id", session->user_id );
  }

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  return mongo->insert( "session", bob.obj() );
}

//-----------------------------------------------------------------------------
bool session_c::fetch( Reveal::Core::session_ptr& session, Reveal::DB::database_ptr db, std::string session_id ) {
  std::auto_ptr<mongo::DBClientCursor> cursor;

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  mongo->fetch( cursor, "session", QUERY( "session_id" << session_id ) );

  // if EMPTYSET returned, then session is not in the database
  if( !cursor->more() ) return false;

  // TODO:add error handling
  mongo::BSONObj record = cursor->next();

  session = Reveal::Core::session_ptr( new Reveal::Core::session_c() );

  // TODO: TODO: TODO: determine best datatype for A) storage and B) passing

  session->session_id = record.getField( "session_id" ).String();
  int user_type = record.getField( "user_type" ).Int();
  if( user_type == 0 ) {
    session->user_type = Reveal::Core::session_c::ANONYMOUS;
  } else {
    session->user_type = Reveal::Core::session_c::IDENTIFIED;
    session->user_id = record.getField( "user_id" ).String();
  }
 
  // TODO expand as needed

  return true;
}

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


