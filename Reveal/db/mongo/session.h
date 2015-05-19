#ifndef _REVEAL_DB_MONGO_SESSION_H_
#define _REVEAL_DB_MONGO_SESSION_H_

#include "Reveal/core/pointers.h"
#include "Reveal/core/session.h"
#include "Reveal/db/mongo/mongo.h"
#include "Reveal/db/database.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------

class session_c {
public:

  /// count the number of sessions in the database
  /// @param db the reveal/mongo database to count records in
  /// @return the total count of sessions listed in the session table
  static unsigned count( database_ptr db );

  /// inserts an instance of a session structure into the database
  /// @param db the reveal/mongo database to insert into
  /// @param session the instance to insert
  /// @return true if the record was inserted OR false if insertion failed
  static bool insert( database_ptr db, Reveal::Core::session_ptr session );

  /// fetches an instance of the session specified from the database
  /// @param session instance populated with data on success
  /// @param db the reveal/mongo database to fetch the record from
  /// @param session_id the unique identifier to search the session table for
  /// @return true if the instance was populated OR false if fetch failed
  static bool fetch( Reveal::Core::session_ptr& session, database_ptr db, std::string session_id );

  /// maps the bson object into a new instance of a session
  /// @param session instance created and populated on success
  /// @param obj the bson object to map data from
  /// @return true if the instance was populated OR false if mapping failed
  static bool map( Reveal::Core::session_ptr& session, mongo::BSONObj obj );

  /// maps the session instance into a bson object 
  /// @param obj the bson object to map data into
  /// @param session the session pointer to map data from
  /// @return true if the instance was populated OR false if mapping failed
  static bool map( mongo::BSONObj& obj, Reveal::Core::session_ptr session );
};

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_MONGO_SESSION_H_
