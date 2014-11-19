#define ODEINT_V1                  // odeint versioning
#define ODEINT_RUNGEKUTTA_STEPPER  // which stepper to use

//-----------------------------------------------------------------------------

//#include "pendulum.h"

#include <Reveal/database.h>
#include <Reveal/scenario.h>
#include <Reveal/pointers.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>

//-----------------------------------------------------------------------------

boost::shared_ptr<Reveal::DB::database_c> db;

//-----------------------------------------------------------------------------
// Note: this is really generating a model solution not a solution
int main( void ) {
  db = boost::shared_ptr<Reveal::DB::database_c>( new Reveal::DB::database_c( "localhost" ) );

  db->open();

  Reveal::Core::scenario_ptr scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );

  int trials = 10;

  scenario->id = "pendulum";
  scenario->description = "simple pendulum";
  scenario->trials = trials;
  db->insert( scenario );

  //std::vector<Reveal::Core::trial_ptr> trial_set;

  double t, dt, tf;
  //state_type x( 2 );
/*
  pendulum_c pendulum( EXPERIMENTAL_PENDULUM_L );
#if defined( ODEINT_V1 )
  stepper_type stepper;
  stepper.adjust_size( x );
#endif
*/

  for( unsigned i = 0; i < trials; i++ ) {
    //pendulum.sample_trial( i, x[0], x[1], t, dt );

    Reveal::Core::trial_ptr trial = Reveal::Core::trial_ptr( new Reveal::Core::trial_c() );

    // insert the trial data
    trial->scenario_id = scenario->id;
    trial->trial_id = i;
    trial->t = t;
    trial->dt = dt;
//    trial->state.append_q( x[0] );
//    trial->state.append_dq( x[1] );
    //trial->control.append_u( u );
    //trial_set.push_back( trial );

    db->insert( trial );

#if defined( ODEINT_V1 )
//    stepper.do_step( pendulum, x, t, dt );
#elif defined( ODEINT_V2 )
    tf = t + dt;
    //boost::numeric::odeint::integrate_adaptive( stepper_type(), pendulum, x, t, tf, dt );
#endif

    // insert the model solution data
    Reveal::Core::solution_ptr solution = Reveal::Core::solution_ptr( new Reveal::Core::solution_c( Reveal::Core::solution_c::MODEL ) );

    solution->scenario_id = scenario->id;
    solution->trial_id = i;
    solution->t = t + dt;
//    solution->state.append_q( x[0] );
//    solution->state.append_dq( x[1] );
    db->insert( solution );

    //printf( "%d: t[%f], dt[%f] x[%.9f, %.9f]\n", i, t, dt, x[0], x[1] );
  }

  Reveal::Core::analyzer_ptr analyzer = Reveal::Core::analyzer_ptr( new Reveal::Core::analyzer_c() );
  analyzer->scenario_id = scenario->id;
  analyzer->filename = "/home/james/osrf/Reveal/build/scenarios/libtest-pendulum-analyzer-plugin.so";
  //analyzer->filename = "test-pendulum-analytics.so";
  analyzer->type = Reveal::Core::analyzer_c::PLUGIN;
  db->insert( analyzer );

  db->close();

  return 0;
}

//-----------------------------------------------------------------------------
