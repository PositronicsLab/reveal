#ifndef _REVEAL_DB_MONGO_SCENARIO_H_
#define _REVEAL_DB_MONGO_SCENARIO_H_

#include "Reveal/core/pointers.h"
#include "Reveal/core/scenario.h"
#include "Reveal/db/mongo/mongo.h"
#include "Reveal/db/database.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------

class scenario_c {
public:

  /// count the number of scenarios in the database
  /// @param db the reveal/mongo database to count records in
  /// @return the total count of scenarios listed in the scenario table
  static unsigned count( database_ptr db );

  /// inserts an instance of a scenario structure into the database
  /// @param db the reveal/mongo database to insert into
  /// @param scenario the instance to insert
  /// @return true if the record was inserted OR false if insertion failed
  static bool insert( database_ptr db, Reveal::Core::scenario_ptr scenario );

  /// fetches an instance of the scenario specified from the database
  /// @param scenario instance populated with data on success
  /// @param db the reveal/mongo database to fetch the record from
  /// @param scenario_id the unique identifier to search the scenario table for
  /// @return true if the instance was populated OR false if fetch failed
  static bool fetch( Reveal::Core::scenario_ptr& scenario, database_ptr db, std::string scenario_id );

};

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_MONGO_SCENARIO_H_
