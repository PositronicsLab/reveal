#ifndef _REVEAL_DB_MONGO_ANALYZER_H_
#define _REVEAL_DB_MONGO_ANALYZER_H_

#include <vector>

#include "reveal/core/pointers.h"
#include "reveal/core/analyzer.h"
#include "reveal/db/mongo/mongo.h"
#include "reveal/db/database.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------

class analyzer_c {
public:

  /// count the number of analyzers in the database
  /// @param db the reveal/mongo database to count records in
  /// @return the total count of users listed in the user table
  static unsigned count( database_ptr db );

  /// inserts an instance of a analyzer structure into the database
  /// @param db the reveal/mongo database to insert into
  /// @param analyzer the instance to insert
  /// @return true if the record was inserted OR false if insertion failed
  static bool insert( database_ptr db, Reveal::Core::analyzer_ptr analyzer );

  /// fetches the set of all analyzers from the database
  /// @param analyzers the vector of analyzers populated with data on success
  /// @param db the reveal/mongo database to fetch the record from
  /// @return true if the instance was populated OR false if fetch failed
  static bool fetch( std::vector<Reveal::Core::analyzer_ptr>& analyzers, database_ptr db );

  /// fetches an instance of the analyzer specified from the database
  /// @param analyzer instance populated with data on success
  /// @param db the reveal/mongo database to fetch the record from
  /// @param scenario_id the unique identifier of the scenario that owns the
  ///        record to search the analyzer table for
  /// @return true if the instance was populated OR false if fetch failed
  static bool fetch( Reveal::Core::analyzer_ptr& analyzer, database_ptr db, std::string scenario_id );

  /// maps the bson object into a new instance of an analyzer
  /// @param analyzer instance created and populated on success
  /// @param obj the bson object to map data from
  /// @return true if the instance was populated OR false if mapping failed
  static bool map( Reveal::Core::analyzer_ptr& analyzer, mongo::BSONObj obj );

  /// maps the analyzer instance into a bson object 
  /// @param obj the bson object to map data into
  /// @param analyzer the analyzer pointer to map data from
  /// @return true if the instance was populated OR false if mapping failed
  static bool map( mongo::BSONObj& obj, Reveal::Core::analyzer_ptr analyzer );
};

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_MONGO_USER_H_
