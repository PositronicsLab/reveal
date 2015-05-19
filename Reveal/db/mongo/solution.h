#ifndef _REVEAL_DB_MONGO_SOLUTION_H_
#define _REVEAL_DB_MONGO_SOLUTION_H_

#include "Reveal/core/pointers.h"
#include "Reveal/core/solution.h"
#include "Reveal/db/mongo/mongo.h"
#include "Reveal/db/database.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------

class solution_c {
public:

  /// inserts an instance of a solution structure into the database
  /// @param db the reveal/mongo database to insert into
  /// @param solution the instance to insert
  /// @return true if the record was inserted OR false if insertion failed
  static bool insert( database_ptr db, Reveal::Core::solution_ptr solution );

  /// fetches an instance of the solution specified from the database
  /// @param solution instance populated with data on success
  /// @param db the reveal/mongo database to fetch the record from
  /// @param type the type of solution CLIENT or MODEL to fetch
  /// @param scenario_id the unique identifier of the scenario that owns the 
  ///        solution to search the table for
  /// @param t the time of the trial associated with the solution to fetch
  /// @param epsilon the error allowed in the time computation
  /// @return true if the instance was populated OR false if fetch failed
  static bool fetch( Reveal::Core::solution_ptr& solution, database_ptr db, Reveal::Core::solution_c::type_e type, std::string scenario_id, double t, double epsilon );

  /// fetches an instance of a CLIENT solution specified from the database
  /// @param solution instance populated with data on success
  /// @param db the reveal/mongo database to fetch the record from
  /// @param experiment_id the unique identifier of the experiment that owns
  ///        the solution to search the table for
  /// @param scenario_id the unique identifier of the scenario that owns the 
  ///        solution to search the table for
  /// @param t the time of the trial associated with the solution to fetch
  /// @param epsilon the error allowed in the time computation
  /// @return true if the instance was populated OR false if fetch failed
  static bool fetch( Reveal::Core::solution_ptr& solution, database_ptr db, std::string experiment_id, std::string scenario_id, double t, double epsilon );

  /// maps the bson object into a new instance of a solution
  /// @param solution instance created and populated on success
  /// @param obj the bson object to map data from
  /// @return true if the instance was populated OR false if mapping failed
  static bool map( Reveal::Core::solution_ptr& solution, mongo::BSONObj obj, Reveal::Core::solution_c::type_e type );

  /// maps the solution instance into a bson object 
  /// @param obj the bson object to map data into
  /// @param solution the solution pointer to map data from
  /// @return true if the instance was populated OR false if mapping failed
  static bool map( mongo::BSONObj& obj, Reveal::Core::solution_ptr solution );

};

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_MONGO_SOLUTION_H_
