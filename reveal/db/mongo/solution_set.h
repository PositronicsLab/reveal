#ifndef _REVEAL_DB_MONGO_SOLUTIONSET_H_
#define _REVEAL_DB_MONGO_SOLUTIONSET_H_

#include "reveal/core/pointers.h"
#include "reveal/core/solution_set.h"
#include "reveal/db/mongo/mongo.h"
#include "reveal/db/database.h"

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
  /// @param t the time to search over all solutions
  /// @param epsilon the error allowed in the temporal search
  /// @return true if the instance was populated OR false if fetch failed
  static bool fetch( Reveal::Core::solution_set_ptr& solution_set, database_ptr db, std::string experiment_id, double t, double epsilon );

  static bool fetch( Reveal::Core::solution_set_ptr& solution_set, database_ptr db, std::string experiment_id, double trial_time, double solution_time, double epsilon );

};

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_MONGO_SOLUTIONSET_H_
