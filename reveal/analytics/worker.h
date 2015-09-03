/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

worker.h defines the worker_c thread that handles execution of an analyzer on
a given solution_set submitted by a client

Note: analyze is currently directly called by a client worker rather than run
as a separate thread.
------------------------------------------------------------------------------*/

#ifndef _REVEAL_ANALYTICS_WORKER_H_
#define _REVEAL_ANALYTICS_WORKER_H_

//-----------------------------------------------------------------------------

#include <string>

#include "reveal/core/connection.h"
#include "reveal/db/database.h"

#include "reveal/core/analysis.h"
#include "reveal/core/pointers.h"
#include "reveal/core/solution_set.h"

#include "reveal/analytics/types.h"
#include "reveal/analytics/module.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Analytics {
//-----------------------------------------------------------------------------

class worker_c {
public:

  /// Default constructor
  worker_c( void );
  /// Destructor
  virtual ~worker_c( void );

  /// executes the analyzer for a single solution and a given experiment
  /// @param db the database to read data from and write analysis to
  /// @param experiment_id the experiment to analyze
  /// @param solution_time the time slice the analyzer is to analyze
  /// @return true on success OR false on any failure 
  bool execute( boost::shared_ptr<Reveal::DB::database_c> db, const std::string& experiment_id, double solution_time );

  /// *Deprecated* executes the analyzer on an entire experiment data set
  /// @param db the database to read data from and write analysis to
  /// @param experiment_id the experiment to analyze
  /// @return true on success OR false on any failure 
  bool execute( boost::shared_ptr<Reveal::DB::database_c> db, const std::string& experiment_id );

  /// *Deprecated* executes the analyzer on an entire experiment data set
  /// @param db the database to read data from and write analysis to
  /// @param experiment_id the experiment to analyze
  /// @return true on success OR false on any failure 
  bool batch_execute( boost::shared_ptr<Reveal::DB::database_c> db, const std::string& experiment_id );

private:
  /// loads the analyzer module
  /// @param module the returned module instance of the analyzer on success
  /// @param analyzer the record indicating which analyzer to load
  /// @return true on success OR false on any failure
  bool load( boost::shared_ptr<Reveal::Analytics::module_c>& module, Reveal::Core::analyzer_ptr analyzer );

  /// *Deprecated* queries the database and fetches a comprehensive set of data
  /// for an entire experiment
  /// @param scenario the scenario record fetched on success
  /// @param experiment the experiment record fetched on success
  /// @param initial_trial the trial record for the initial state on success
  /// @param analyzer the analyzer record fetched on success
  /// @param db the database to query
  /// @param experiment_id the experiment to request
  /// @return true on success OR false on any failure
  bool fetch( Reveal::Core::scenario_ptr& scenario, Reveal::Core::experiment_ptr& experiment, Reveal::Core::solution_ptr& initial_state, Reveal::Core::analyzer_ptr& analyzer, boost::shared_ptr<Reveal::DB::database_c> db, std::string experiment_id );
};

//-----------------------------------------------------------------------------
} // namespace Analytics
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_ANALYTICS_WORKER_H_
