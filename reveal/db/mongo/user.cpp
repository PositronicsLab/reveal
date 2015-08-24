#include "reveal/db/mongo/user.h"

#include <mongo/client/dbclient.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
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
  mongo::BSONObj obj;

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  map( obj, user );

  return mongo->insert( "user", obj );
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

  map( user, record );

  return true;
}

//-----------------------------------------------------------------------------
bool user_c::map( Reveal::Core::user_ptr& user, mongo::BSONObj obj ) { 

  user = Reveal::Core::user_ptr( new Reveal::Core::user_c() );
  user->id = obj.getField( "user_id" ).String();

  return true;
}

//-----------------------------------------------------------------------------
bool user_c::map( mongo::BSONObj& obj, Reveal::Core::user_ptr user ) {

  mongo::BSONObjBuilder bob;

  bob.append( "user_id", user->id );

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


