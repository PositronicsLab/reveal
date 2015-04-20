#ifndef _REVEAL_DB_MONGO_ANALYZER_H_
#define _REVEAL_DB_MONGO_ANALYZER_H_

#include "Reveal/core/pointers.h"
#include "Reveal/core/analyzer.h"
#include "Reveal/db/mongo/mongo.h"
#include "Reveal/db/database.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
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

  /// fetches an instance of the analyzer specified from the database
  /// @param analyzer instance populated with data on success
  /// @param db the reveal/mongo database to fetch the record from
  /// @param scenario_id the unique identifier of the scenario that owns the
  ///        record to search the analyzer table for
  /// @return true if the instance was populated OR false if fetch failed
  static bool fetch( Reveal::Core::analyzer_ptr& analyzer, database_ptr db, std::string scenario_id );

};

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_MONGO_USER_H_
