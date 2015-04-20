#include "Reveal/db/mongo/user.h"

#include <mongo/client/dbclient.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
unsigned user_c::count( Reveal::DB::database_ptr db ) {
  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return 0;

  // query mongo for count
  return mongo->count( "user" );
}
//-----------------------------------------------------------------------------
bool user_c::insert( Reveal::DB::database_ptr db, Reveal::Core::user_ptr user ) {
  mongo::BSONObjBuilder bob;

  bob.append( "user_id", user->id );

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  return mongo->insert( "user", bob.obj() );
}

//-----------------------------------------------------------------------------
bool user_c::fetch( Reveal::Core::user_ptr& user, Reveal::DB::database_ptr db, std::string user_id ) {
  std::auto_ptr<mongo::DBClientCursor> cursor;

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  mongo->fetch( cursor, "user", QUERY( "user_id" << user_id ) );

  // if EMPTYSET returned, then user is not in the database
  if( !cursor->more() ) return false;

  // TODO:add error handling
  mongo::BSONObj record = cursor->next();

  user = Reveal::Core::user_ptr( new Reveal::Core::user_c() );
  user->id = record.getField( "user_id" ).String();
  // TODO expand as needed

  return true;
}

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


