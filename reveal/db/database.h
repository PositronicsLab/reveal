/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

database.h defines the database_c adapter that encapsulates the mongo database
driver.  All interactions with the mongo database are handled through this
interface
------------------------------------------------------------------------------*/

#ifndef _REVEAL_DB_DATABASE_
#define _REVEAL_DB_DATABASE_

//-----------------------------------------------------------------------------
#include <cstdlib>
#include <iostream>
#include <memory>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "reveal/db/service.h"

#include "reveal/core/pointers.h"
#include "reveal/core/analysis.h"
#include "reveal/core/analyzer.h"
#include "reveal/core/solution.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------

class database_c : public boost::enable_shared_from_this<database_c> {
protected:

  service_ptr                  _service;   //< the service the database is using

public:
  /// Enumeration defining the errors the database methods may encounter
  enum error_e {
    ERROR_NONE,            //< indicates that an operation was successful
    ERROR_EMPTYSET         //< indicates a fetch returned an empty set
  };

  /// Default constructor
  database_c( void );
  /// Destructor
  virtual ~database_c( void );

  /// Gets a shared pointer to this instance
  database_ptr ptr( void );
  /// Gets the service used by this instance
  service_ptr service( void );

  /// Opens the database service
  /// @return true if the database service is successfully opened OR false if
  ///         any error occurs
  bool open( void );

  /// Closes the database service
  void close( void );

public:
  /// Inserts a reveal user into the database
  /// @param user the user to insert
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e insert( Reveal::Core::user_ptr user );

  /// Fetches a reveal user from the database given a user identifier
  /// @param user the user data returned on a successful query
  /// @param user_id the unique user identifier to filter user data by
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e fetch( Reveal::Core::user_ptr& user, const std::string& user_id );

  /// Inserts a reveal session into the database
  /// @param session the session to insert
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e insert( Reveal::Core::session_ptr session );

  /// Fetches a reveal session from the database given a session identifier
  /// @param session the session data returned on a successful query
  /// @param session_id the unique session identifier to filter session data by
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e fetch( Reveal::Core::session_ptr& session, std::string session_id );

  /// Fetches a reveal scenario digest from the database containing all sessions
  /// maintained in the database
  /// @param digest the digest data returned on a successful query
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e fetch( Reveal::Core::digest_ptr& digest );

  /// Inserts a reveal experiment into the database
  /// @param experiment the experiment to insert
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e insert( Reveal::Core::experiment_ptr experiment );

  /// Fetches a reveal experiment from the database given an experiment
  /// identifier
  /// @param experiment the experiment data returned on a successful query
  /// @param experiment_id the unique experiment identifier to filter experiment
  ///        data by
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e fetch( Reveal::Core::experiment_ptr& experiment, std::string experiment_id );

  /// Inserts a reveal scenario into the database
  /// @param scenario the scenario to insert
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e insert( Reveal::Core::scenario_ptr scenario );

  /// Fetches a reveal scenario from the database given a scenario identifier
  /// @param scenario the scenario data returned on a successful query
  /// @param scenario_id the unique session identifier to filter scenario data 
  ///        by
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
   error_e fetch( Reveal::Core::scenario_ptr& scenario, const std::string& scenario_id );

  /// Inserts a reveal trial into the database
  /// @param trial the trial to insert
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e insert( Reveal::Core::trial_ptr trial );

  /// Fetches a reveal trial from the database given a scenario identifier, 
  /// a time and an error tolerance
  /// @param trial the trial data returned on a successful query
  /// @param scenario_id the unique session identifier to filter trial data by
  /// @param t the time to filter trial data by
  /// @param epsilon the tolerance to allow time value to vary by
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e fetch( Reveal::Core::trial_ptr& trial, const std::string& scenario_id, double t, double epsilon );

  /// Inserts a reveal solution into the database
  /// @param solution the solution to insert
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e insert( Reveal::Core::solution_ptr solution );

  /// Fetches a reveal model solution from the database given a scenario 
  /// identifier, a time and an error tolerance
  /// @param solution the model solution data returned on a successful query
  /// @param scenario_id the unique session identifier to filter solution data 
  ///        by
  /// @param t the time to filter solution data by
  /// @param epsilon the tolerance to allow time value to vary by
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e fetch( Reveal::Core::solution_ptr& solution, const std::string& experiment_id, const std::string& scenario_id, double t, double epsilon );

  /// Fetches a reveal model or client solution from the database given a 
  /// scenario identifier, a time and an error tolerance
  /// @param solution the solution data returned on a successful query
  /// @param scenario_id the unique session identifier to filter solution data 
  ///        by
  /// @param t the time to filter solution data by
  /// @param epsilon the tolerance to allow time value to vary by
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e fetch( Reveal::Core::solution_ptr& solution, Reveal::Core::solution_c::type_e type, const std::string& scenario_id, double t, double epsilon );

  /// Inserts a reveal analyzer into the database
  /// @param analyzer the analyzer to insert
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e insert( Reveal::Core::analyzer_ptr analyzer );

  /// Fetches the set of all analyzers in the database 
  /// @param analyzers the set of all analyzers returned on a successful query
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e fetch( std::vector<Reveal::Core::analyzer_ptr>& analyzers );

  /// Fetches a reveal model or client solution from the database given a 
  /// scenario identifier, a time and an error tolerance
  /// @param solution the solution data returned on a successful query
  /// @param scenario_id the unique session identifier to filter solution data 
  ///        by
  /// @param t the time to filter solution data by
  /// @param epsilon the tolerance to allow time value to vary by
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e fetch( Reveal::Core::analyzer_ptr& analyzer, const std::string& scenario_id );

  /// Fetches an entire reveal solution set from the database given an 
  /// experiment identifier
  /// @param solution_set the solution set data returned on a successful query
  /// @param experiment_id the unique experiment identifier to filter solution 
  ///        set data by
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e fetch( Reveal::Core::solution_set_ptr& solution_set, const std::string& experiment_id );

  /// Fetches a single reveal solution set from the database given an 
  /// experiment identifier, a time and an error tolerance
  /// @param solution_set the solution set data returned on a successful query
  /// @param experiment_id the unique experiment identifier to filter solution 
  ///        set data by
  /// @param t the time to filter solution data by
  /// @param epsilon the tolerance to allow time value to vary by
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e fetch( Reveal::Core::solution_set_ptr& solution_set, const std::string& experiment_id, double t, double epsilon );

  /// Fetches a single reveal solution set from the database given an 
  /// experiment identifier, a trial time, a solution time and an error 
  /// tolerance
  /// @param solution_set the solution set data returned on a successful query
  /// @param experiment_id the unique experiment identifier to filter solution 
  ///        set data by
  /// @param trial_time the trial time to filter trial data by
  /// @param solution_time the solution time to filter solution data by
  /// @param epsilon the tolerance to allow time value to vary by
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e fetch( Reveal::Core::solution_set_ptr& solution_set, const std::string& experiment_id, double trial_time, double solution_time, double epsilon );

  /// Inserts a reveal analysis into the database
  /// @param analysis the analysis to insert
  /// @return returns ERROR_NONE if the operation was successful OR another
  ///         enumerated value if the operation failed
  error_e insert( Reveal::Core::analysis_ptr analysis );
};

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_DATABASE_
