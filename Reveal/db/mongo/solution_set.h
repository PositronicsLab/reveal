#ifndef _REVEAL_DB_MONGO_SOLUTIONSET_H_
#define _REVEAL_DB_MONGO_SOLUTIONSET_H_

#include "Reveal/core/pointers.h"
#include "Reveal/core/solution_set.h"
#include "Reveal/db/mongo/mongo.h"
#include "Reveal/db/database.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------

class solution_set_c {
public:

  /// fetches the set of all solutions for a single experiment from the database
  /// @param solution_set instance populated with data on success
  /// @param db the reveal/mongo database to fetch the record from
  /// @param experiment_id the unique experiment identifier to search over all
  ///        solutions
  /// @return true if the instance was populated OR false if fetch failed
  static bool fetch( Reveal::Core::solution_set_ptr& solution_set, database_ptr db, std::string experiment_id );

  /// fetches a set of solutions for a single experiment for a single trial 
  /// from the database
  /// @param solution_set instance populated with data on success
  /// @param db the reveal/mongo database to fetch the record from
  /// @param experiment_id the unique experiment identifier to search over all
  ///        solutions
  /// @param trial_id the trial index to search over all solutions
  /// @return true if the instance was populated OR false if fetch failed
  static bool fetch( Reveal::Core::solution_set_ptr& solution_set, database_ptr db, std::string experiment_id, unsigned trial_id );

};

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_MONGO_SOLUTIONSET_H_
