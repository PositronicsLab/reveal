#ifndef _REVEAL_DB_MONGO_TRIAL_H_
#define _REVEAL_DB_MONGO_TRIAL_H_

#include "Reveal/core/pointers.h"
#include "Reveal/core/trial.h"
#include "Reveal/db/mongo/mongo.h"
#include "Reveal/db/database.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------

class trial_c {
public:

  /// count the number of trials in the database
  /// @param db the reveal/mongo database to count records in
  /// @return the total count of trials listed in the trial table
  static unsigned count( database_ptr db );

  /// inserts an instance of a trial structure into the database
  /// @param db the reveal/mongo database to insert into
  /// @param trial the instance to insert
  /// @return true if the record was inserted OR false if insertion failed
  static bool insert( database_ptr db, Reveal::Core::trial_ptr trial );

  /// fetches an instance of the trial specified from the database
  /// @param trial instance populated with data on success
  /// @param db the reveal/mongo database to fetch the record from
  /// @param scenario_id the unique identifier of the scenario that owns the 
  ///        trial to search the table for
  /// @param t the time of the trial to fetch
  /// @param epsilon the acceptable error to use in the time query
  /// @return true if the instance was populated OR false if fetch failed
  static bool fetch( Reveal::Core::trial_ptr& trial, database_ptr db, std::string scenario_id, double t, double epsilon );

  /// maps the bson object into a new instance of a trial
  /// @param trial instance created and populated on success
  /// @param obj the bson object to map data from
  /// @return true if the instance was populated OR false if mapping failed
  static bool map( Reveal::Core::trial_ptr& trial, mongo::BSONObj obj );

  /// maps the trial instance into a bson object 
  /// @param obj the bson object to map data into
  /// @param trial the trial pointer to map data from
  /// @return true if the instance was populated OR false if mapping failed
  static bool map( mongo::BSONObj& obj, Reveal::Core::trial_ptr trial );

};

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_MONGO_TRIAL_H_
