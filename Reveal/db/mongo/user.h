#ifndef _REVEAL_DB_MONGO_USER_H_
#define _REVEAL_DB_MONGO_USER_H_

#include "Reveal/core/pointers.h"
#include "Reveal/core/user.h"
#include "Reveal/db/mongo/mongo.h"
#include "Reveal/db/database.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------

class user_c {
public:

  /// count the number of users in the database
  /// @param db the reveal/mongo database to count records in
  /// @return the total count of users listed in the user table
  static unsigned count( database_ptr db );

  /// inserts an instance of a user structure into the database
  /// @param db the reveal/mongo database to insert into
  /// @param user the instance to insert
  /// @return true if the record was inserted OR false if insertion failed
  static bool insert( database_ptr db, Reveal::Core::user_ptr user );

  /// fetches an instance of the user specified from the database
  /// @param user instance populated with data on success
  /// @param db the reveal/mongo database to fetch the record from
  /// @param user_id the unique identifier to search the user table for
  /// @return true if the instance was populated OR false if fetch failed
  static bool fetch( Reveal::Core::user_ptr& user, database_ptr db, std::string user_id );

  /// maps the bson object into a new instance of a user
  /// @param user instance created and populated on success
  /// @param obj the bson object to map data from
  /// @return true if the instance was populated OR false if mapping failed
  static bool map( Reveal::Core::user_ptr& user, mongo::BSONObj obj );

  /// maps the user instance into a bson object 
  /// @param obj the bson object to map data into
  /// @param user the user pointer to map data from
  /// @return true if the instance was populated OR false if mapping failed
  static bool map( mongo::BSONObj& obj, Reveal::Core::user_ptr user );

};

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_MONGO_USER_H_
