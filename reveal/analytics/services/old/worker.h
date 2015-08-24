/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

worker.h defines the worker_c thread that handles execution of an analyzer on
a given solution_set submitted by a client

Note: analyze is currently directly called by a client worker rather than run
as a separate thread.
------------------------------------------------------------------------------*/

#ifndef _REVEAL_ANALYTICS_SERVICES_NODE_WORKER_H_
#define _REVEAL_ANALYTICS_SERVICES_NODE_WORKER_H_

//-----------------------------------------------------------------------------

#include <string>

#include "reveal/core/connection.h"
#include "reveal/db/database.h"

#include "reveal/analytics/types.h"
#include "reveal/analytics/module.h"
#include "reveal/core/analysis.h"
#include "reveal/core/pointers.h"
#include "reveal/core/solution_set.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Analytics {
//-----------------------------------------------------------------------------

class worker_c {
private:
  boost::shared_ptr<Reveal::DB::database_c> _db; //< the reveal database
  void* _context;          //< the parent server process' communication context
  Reveal::Core::connection_c _connection;  //< the connection to the client

public:

  /// Default constructor
  worker_c( void* context );
  /// Destructor
  virtual ~worker_c( void );

  bool init( void );
  void work( void );
  void terminate( void );
private:
  /// Encapsulates replying to a client authorization request with a valid 
  /// authorization
  /// @param auth the authorization granted to the client
  /// @return returns ERROR_NONE if the operation succeeded OR another 
  ///         enumerated value if the operation failed
  error_e send_valid_handshake_response( Reveal::Core::authorization_ptr auth );

public:
  /// executes the analyzer for a single solution and a given experiment
  /// @param experiment_id the experiment to analyze
  /// @param solution_time the time slice the analyzer is to analyze
  /// @return true on success OR false on any failure 
  bool execute( const std::string& experiment_id, double solution_time );

  /// *Deprecated* executes the analyzer on an entire experiment data set
  /// @param experiment_id the experiment to analyze
  /// @return true on success OR false on any failure 
  bool execute( const std::string& experiment_id );

  /// *Deprecated* executes the analyzer on an entire experiment data set
  /// @param experiment_id the experiment to analyze
  /// @return true on success OR false on any failure 
  bool batch_execute( const std::string& experiment_id );

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
  /// @param experiment_id the experiment to request
  /// @return true on success OR false on any failure
  bool fetch( Reveal::Core::scenario_ptr& scenario, Reveal::Core::experiment_ptr& experiment, Reveal::Core::trial_ptr& initial_trial, Reveal::Core::analyzer_ptr& analyzer, std::string experiment_id );
};

//-----------------------------------------------------------------------------
} // namespace Analytics
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_ANALYTICS_SERVICES_NODE_WORKER_H_
