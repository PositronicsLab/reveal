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

#include "Reveal/core/connection.h"
#include "Reveal/db/database.h"

#include "Reveal/analytics/types.h"
#include "Reveal/analytics/module.h"
#include "Reveal/core/analysis.h"
#include "Reveal/core/pointers.h"
#include "Reveal/core/solution_set.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Analytics {
//-----------------------------------------------------------------------------

class worker_c {
public:

  worker_c( void );
  virtual ~worker_c( void );

  bool execute( boost::shared_ptr<Reveal::DB::database_c> db, const std::string& experiment_id );
  bool execute( boost::shared_ptr<Reveal::DB::database_c> db, const std::string& experiment_id, double trial_time );

  bool batch_execute( boost::shared_ptr<Reveal::DB::database_c> db, const std::string& experiment_id );

private:
  bool load( boost::shared_ptr<Reveal::Analytics::module_c>& module, Reveal::Core::analyzer_ptr analyzer );

  bool fetch( Reveal::Core::scenario_ptr& scenario, Reveal::Core::experiment_ptr& experiment, Reveal::Core::trial_ptr& initial_trial, Reveal::Core::analyzer_ptr& analyzer, boost::shared_ptr<Reveal::DB::database_c> db, std::string experiment_id );
};

//-----------------------------------------------------------------------------
} // namespace Analytics
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_ANALYTICS_WORKER_H_
