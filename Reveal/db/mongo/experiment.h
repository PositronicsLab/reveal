#ifndef _REVEAL_DB_MONGO_EXPERIMENT_H_
#define _REVEAL_DB_MONGO_EXPERIMENT_H_

#include "Reveal/core/pointers.h"
#include "Reveal/core/experiment.h"
#include "Reveal/db/mongo/mongo.h"
#include "Reveal/db/database.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
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

  /// issues an update command and increments the current_trial_index for a 
  ///        given experiment
  /// @param db the reveal/mongo database to update
  /// @param experiment the instance of the experiment to update
  static bool update_increment_trial_index( database_ptr db, Reveal::Core::experiment_ptr experiment );
};

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_MONGO_EXPERIMENT_H_
