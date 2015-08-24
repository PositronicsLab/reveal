#include "Reveal/analytics/worker.h"

#include <cmath>

#include "Reveal/analytics/plugin.h"
#include "Reveal/analytics/script.h"
#include "Reveal/core/system.h"
#include "Reveal/core/experiment.h"

#include "Reveal/core/pointers.h"
#include "Reveal/core/scenario.h"

#include "Reveal/core/console.h"
//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Analytics {
//-----------------------------------------------------------------------------
worker_c::worker_c( void ) {

}

//-----------------------------------------------------------------------------
worker_c::~worker_c( void ) {

}

//-----------------------------------------------------------------------------
bool worker_c::execute( boost::shared_ptr<Reveal::DB::database_c> db, const std::string& experiment_id, double solution_time ) {
  Reveal::Core::scenario_ptr scenario;
  Reveal::Core::experiment_ptr experiment;
  Reveal::Core::analyzer_ptr analyzer;
  Reveal::Core::trial_ptr initial_trial;
  boost::shared_ptr<Reveal::Analytics::module_c> module;
  Reveal::Analytics::error_e error;
  Reveal::DB::database_c::error_e db_error;
  Reveal::Core::solution_set_ptr solution_set;
  Reveal::Core::analysis_ptr analysis;

  // sanity check on database reference
  assert( db );
  db->open();

  // fetch the experimental configuration from the database
  if( !fetch( scenario, experiment, initial_trial, analyzer, db, experiment_id) )
    return false;

  // sanity check that the fetch was successful
  assert( scenario && experiment && initial_trial && analyzer );
  //printf( "initial_trial t[%f]\n", initial_trial->t );

  // load the analyzer module
  if( !load( module, analyzer ) )
    return false;

  // sanity check that load was successful
  assert( module );

  double EPSILON, dt, trial_time;

  // compute the differential time between each solution
  if( experiment->time_step < scenario->sample_rate )
    dt = scenario->sample_rate;
  else
    dt = experiment->time_step; 

  // and set other relavent time variables needed for the query
  EPSILON = experiment->epsilon;
  trial_time = solution_time - dt;

  // fetch the solution set for the given experiment and time
  db_error = db->fetch( solution_set, experiment->experiment_id, trial_time, solution_time, EPSILON );

  // sanity check that solution_set query was successful
  assert( solution_set ); 

  // set relevant solution set variables
  solution_set->time_step = scenario->sample_rate;
  solution_set->epsilon = EPSILON;
  solution_set->initial_trial = initial_trial;

  // generate an analysis by passing the solution_set to plugin execute method
  error = module->analyze( solution_set, analysis );
  if( error != Reveal::Analytics::ERROR_NONE ) {
    //printf( "analyzer failed to complete execution\n" ); 
    return false;
  }

  // sanity check that an analysis was generated
  assert( analysis );

  // insert the analysis into the database
  db_error = db->insert( analysis );
  if( db_error != Reveal::DB::database_c::ERROR_NONE ) {
    //printf( "db_error[%d] inserting analysis\n", db_error );
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
bool worker_c::execute( boost::shared_ptr<Reveal::DB::database_c> db, const std::string& experiment_id ) {
  Reveal::Core::scenario_ptr scenario;
  Reveal::Core::experiment_ptr experiment;
  Reveal::Core::analyzer_ptr analyzer;
  Reveal::Core::trial_ptr initial_trial;
  boost::shared_ptr<Reveal::Analytics::module_c> module;
  Reveal::Analytics::error_e error;
  Reveal::DB::database_c::error_e db_error;

  assert( db );
  db->open();

  // fetch the experimental configuration from the database
  if( !fetch( scenario, experiment, initial_trial, analyzer, db, experiment_id) )
    return false;
  assert( scenario && experiment && initial_trial && analyzer );

  // load the analyzer module
  if( !load( module, analyzer ) )
    return false;
  assert( module );

  double t = experiment->start_time;
  double dt = experiment->time_step;
  double EPSILON = experiment->epsilon;
  double tf = experiment->end_time;

  // iterate over all the trials, build a solution_set individually for each 
  // trial, analyze that solution and insert the analysis in the database
  //TODO: FIX

  while( fabs( tf - t ) > EPSILON ) {
    Reveal::Core::solution_set_ptr solution_set;
    Reveal::Core::analysis_ptr analysis;

    db_error = db->fetch( solution_set, experiment->experiment_id, t, EPSILON );

    if( !solution_set ) 
      printf( "failed to select requested solution_set from db\n" );

    solution_set->initial_trial = initial_trial;
        
    error = module->analyze( solution_set, analysis );
    if( error != Reveal::Analytics::ERROR_NONE ) {
      printf( "analyzer failed to complete execution\n" ); 
      return false;
    }
 
    assert( analysis );

    db_error = db->insert( analysis );
    if( db_error != Reveal::DB::database_c::ERROR_NONE )
      printf( "db_error[%d] inserting analysis\n", db_error );

    t += dt;
  }
/*
  for( unsigned i = 0; i < scenario->trials; i++ ) {
    Reveal::Core::solution_set_ptr solution_set;
    Reveal::Core::analysis_ptr analysis;

    db_error = db->fetch( solution_set, experiment->experiment_id, i );

    if( !solution_set ) 
      printf( "failed to select requested solution_set from db\n" );

    solution_set->initial_trial = initial_trial;
        
    error = module->analyze( solution_set, analysis );
    if( error != Reveal::Analytics::ERROR_NONE ) {
      printf( "analyzer failed to complete execution\n" ); 
      return false;
    }
 
    assert( analysis );

    db_error = db->insert( analysis );
    if( db_error != Reveal::DB::database_c::ERROR_NONE )
      printf( "db_error[%d] inserting analysis\n", db_error );
  }
*/
  return true;
}

//-----------------------------------------------------------------------------
bool worker_c::batch_execute( boost::shared_ptr<Reveal::DB::database_c> db, const std::string& experiment_id ) {
  Reveal::Core::scenario_ptr scenario;
  Reveal::Core::experiment_ptr experiment;
  Reveal::Core::analyzer_ptr analyzer;
  Reveal::Core::trial_ptr initial_trial;
  Reveal::Core::solution_set_ptr solution_set;
  Reveal::Core::analysis_ptr analysis;
  boost::shared_ptr<Reveal::Analytics::module_c> module;
  Reveal::Analytics::error_e error;
  Reveal::DB::database_c::error_e db_error;

  assert( db );
  db->open();

  // fetch the experimental configuration from the database
  if( !fetch( scenario, experiment, initial_trial, analyzer, db, experiment_id) )
    return false;
  assert( scenario && experiment && initial_trial && analyzer );

  // load the analyzer module
  if( !load( module, analyzer ) )
    return false;
  assert( module );

  // build a solution set for the entire experiment
  //printf( "fetching solution_set\n" );
  db_error = db->fetch( solution_set, experiment->experiment_id );
  if( db_error != Reveal::DB::database_c::ERROR_NONE )
    printf( "db_error[%d] fetching solution_set\n", db_error );

  if( !solution_set ) 
    printf( "failed to select requested solution_set from db\n" );
  assert( solution_set );

  solution_set->initial_trial = initial_trial;

  // submit the comprehensive solution set to the analyzer
  //printf( "analyzing\n" );
  error = module->analyze( solution_set, analysis );
  if( error != Reveal::Analytics::ERROR_NONE ) {
    printf( "analyzer failed to complete execution\n" ); 
    return false;
  }
  assert( analysis );

  // insert the comprehensive analysis in the database
  //printf( "inserting\n" );
  db_error = db->insert( analysis );
  if( db_error != Reveal::DB::database_c::ERROR_NONE )
    printf( "db_error[%d] inserting analysis\n", db_error );

  return true;
}

//-----------------------------------------------------------------------------
bool worker_c::load( boost::shared_ptr<Reveal::Analytics::module_c>& module, Reveal::Core::analyzer_ptr analyzer ) {

  Reveal::Core::system_c system( Reveal::Core::system_c::SERVER );
  if( !system.open() ) {
    std::string errmsg = "ERROR: Failed to open system.  Make sure the Reveal environment is properly configured";
    Reveal::Core::console_c::error( errmsg );
    return false;
  }
  std::string pkg_path = system.package_path();
  system.close();

  Reveal::Analytics::error_e error;

  if( analyzer->type == Reveal::Core::analyzer_c::PLUGIN ) {
    // load analyzer as plugin
    boost::shared_ptr<Reveal::Analytics::plugin_c> plugin = boost::shared_ptr<Reveal::Analytics::plugin_c>( new Reveal::Analytics::plugin_c() );
    
    module = boost::dynamic_pointer_cast<Reveal::Analytics::module_c>( plugin );
  } else if( analyzer->type == Reveal::Core::analyzer_c::SCRIPT ) {
    // load analyzer as script

    boost::shared_ptr<Reveal::Analytics::script_c> script = boost::shared_ptr<Reveal::Analytics::script_c>( new Reveal::Analytics::script_c() );

    module = boost::dynamic_pointer_cast<Reveal::Analytics::module_c>( script );
  } else {
    return false;
  }

  // compose the filename of the module
  std::string path = combine_path( pkg_path, analyzer->build_path);
  path = combine_path( path, analyzer->build_target );

  // load the module
  error = module->load( path );
  if( error != Reveal::Analytics::ERROR_NONE ) {
    printf( "failed to load requested analyzer\n" );
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
bool worker_c::fetch( Reveal::Core::scenario_ptr& scenario, Reveal::Core::experiment_ptr& experiment, Reveal::Core::trial_ptr& initial_trial, Reveal::Core::analyzer_ptr& analyzer, boost::shared_ptr<Reveal::DB::database_c> db, std::string experiment_id ) {

  Reveal::DB::database_c::error_e error;

  error = db->fetch( experiment, experiment_id );
  if( error != Reveal::DB::database_c::ERROR_NONE ) 
    printf( "db error[%d] fetching experiment\n", error );
  if( !experiment ) return false;

  error = db->fetch( scenario, experiment->scenario_id );
  if( error != Reveal::DB::database_c::ERROR_NONE )
    printf( "db error[%d] fetching scenario\n", error );
  if( !scenario ) return false;

  error = db->fetch( initial_trial, experiment->scenario_id, experiment->start_time, experiment->epsilon );
  if( error != Reveal::DB::database_c::ERROR_NONE ) 
    printf( "db error[%d] fetching initial trial\n", error );
  if( !initial_trial ) return false;

  error = db->fetch( analyzer, experiment->scenario_id );
  if( error != Reveal::DB::database_c::ERROR_NONE )
    printf( "db error[%d] fetching analyzer\n", error );
  if( !analyzer ) return false;

  return true;
}

//-----------------------------------------------------------------------------
} // namespace Analytics
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
