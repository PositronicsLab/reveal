#include "Reveal/db/mongo/solution_set.h"

#include <mongo/client/dbclient.h>

#include "Reveal/core/experiment.h"
#include "Reveal/core/scenario.h"
#include "Reveal/core/trial.h"
#include "Reveal/core/solution.h"
#include "Reveal/core/model.h"

#include "Reveal/db/mongo/scenario.h"
#include "Reveal/db/mongo/experiment.h"
#include "Reveal/db/mongo/trial.h"
#include "Reveal/db/mongo/solution.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------

bool solution_set_c::fetch( Reveal::Core::solution_set_ptr& solution_set, Reveal::DB::database_ptr db, std::string experiment_id ) {

  // NOTE: may need to order queries such that indices match up in each query

  Reveal::Core::experiment_ptr experiment;
  Reveal::Core::scenario_ptr scenario;
  Reveal::Core::trial_ptr trial;
  Reveal::Core::solution_ptr solution, model;
  bool result;

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  solution_set = Reveal::Core::solution_set_ptr( new Reveal::Core::solution_set_c() );

  result = experiment_c::fetch( experiment, db, experiment_id );
  if( !result ) return false;

  std::string scenario_id = experiment->scenario_id;

  result = scenario_c::fetch( scenario, db, scenario_id );
  if( !result ) return false;

  //solution_set->scenario = scenario;
  solution_set->experiment = experiment;

  for( unsigned i = 0; i < scenario->trials; i++ ) {
    result = trial_c::fetch( trial, db, scenario_id, i );
    if( !result ) return false;
    solution_set->add_trial( trial );

    result = solution_c::fetch( solution, db, experiment->experiment_id, scenario_id, i );
    if( !result ) return false;
    solution_set->add_solution( solution );

    result = solution_c::fetch( model, db, Reveal::Core::solution_c::MODEL, scenario_id, i );
    if( !result ) return false;
    solution_set->add_model( model );
  }

  return true;
}

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


