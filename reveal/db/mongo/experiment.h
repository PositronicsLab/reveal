#ifndef _REVEAL_DB_MONGO_EXPERIMENT_H_
#define _REVEAL_DB_MONGO_EXPERIMENT_H_

#include "reveal/core/pointers.h"
#include "reveal/core/experiment.h"
#include "reveal/db/mongo/mongo.h"
#include "reveal/db/database.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------

class experiment_c {
public:

  /// count the number of experiments in the database
  /// @param db the reveal/mongo database to count records in
  /// @return the total count of experiments listed in the experiment table
  static unsigned count( database_ptr db );

  /// inserts an instance of an experiment structure into the database
  /// @param db the reveal/mongo database to insert into
  /// @param experiment the instance to insert
  /// @return true if the record was inserted OR false if insertion failed
  static bool insert( database_ptr db, Reveal::Core::experiment_ptr experiment );

  /// fetches an instance of the experiment specified from the database
  /// @param experiment instance populated with data on success
  /// @param db the reveal/mongo database to fetch the record from
  /// @param experiment_id the unique identifier to search the experiment table
  ///        for
  /// @return true if the instance was populated OR false if fetch failed
  static bool fetch( Reveal::Core::experiment_ptr& experiment, database_ptr db, std::string experiment_id );

  /// maps the bson object into a new instance of an experiment
  /// @param experiment instance created and populated on success
  /// @param obj the bson object to map data from
  /// @return true if the instance was populated OR false if mapping failed
  static bool map( Reveal::Core::experiment_ptr& experiment, mongo::BSONObj obj );

  /// maps the experiment instance into a bson object 
  /// @param obj the bson object to map data into
  /// @param experiment the experiment pointer to map data from
  /// @return true if the instance was populated OR false if mapping failed
  static bool map( mongo::BSONObj& obj, Reveal::Core::experiment_ptr experiment );
};

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_MONGO_EXPERIMENT_H_
