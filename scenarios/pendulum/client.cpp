// odeint versioning
#define ODEINT_V1
//#define ODEINT_V2

// which stepper to use
#define ODEINT_EULER_STEPPER
//#define ODEINT_RUNGEKUTTA_STEPPER

//-----------------------------------------------------------------------------

#include "pendulum.h"

#include <Reveal/client.h>
#include <Reveal/digest.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>

//-----------------------------------------------------------------------------
int main( void ) {
  Reveal::Client::client_c client;

  if( !client.init() ) {
    printf( "ERROR: Client failed to initialize properly.\nExiting.\n" );
    client.terminate();
    exit(1);
  }

  Reveal::Core::digest_ptr digest;
  Reveal::Core::scenario_ptr scenario;
  Reveal::Core::trial_ptr trial;
  Reveal::Core::solution_ptr solution;

  if( !client.connect() ) {
    printf( "ERROR: Client failed to connect properly.\nExiting.\n" );
    client.terminate();
    exit(1);
  }
  
  // request the digest
  client.request_digest( digest );

  bool found = false;
  for( unsigned i = 0; i < digest->scenarios(); i++ ) {
    scenario = digest->get_scenario( i );
    if( strcmp( scenario->name.c_str(), "pendulum" ) == 0 ) {
      found = true;
      break;
    }
  }
  if( !found ) {
    printf( "ERROR: Failed to find pendulum in the scenario digest\nExiting.\n" );
    client.terminate();
    exit(1);
  }
  //printf( "Found the pendulum scenario in the scenario digest\n" );
  scenario->print();

  // request the scenario details from the server
  client.request_scenario( scenario );
  // TODO: error handling

  // Pendulum integrator
  pendulum_c pendulum( EXPERIMENTAL_PENDULUM_L );
  state_type x( 2 );
  double t, dt, tf;
#if defined( ODEINT_V1 )
  stepper_type stepper;
  stepper.adjust_size( x );
#endif

  // for each trial in the scenario
  for( unsigned i = 0; i < scenario->trials; i++ ) {
    // request the trial from the server

    // create a trial
    trial = scenario->get_trial( i );

    client.request_trial( trial );
    // TODO: error handling

    trial->print();
    // **SIMULATION WOULD BE RUN HERE**
/*
    double ti = trial->t;
    double dt = trial->dt;
    double tf = ti + dt;
*/
    t = trial->t; 
    dt = trial->dt; 
    x[0] = trial->state.q( 0 );
    x[1] = trial->state.dq( 0 );

    //printf( "%d: t[%f], dt[%f] x[%.9f, %.9f]\n", i, t, dt, x[0], x[1] );
    // integrate the pendulum
    tf = t + dt;
#if defined( ODEINT_V1 )
    stepper.do_step( pendulum, x, t, dt );
#elif defined( ODEINT_V2 )
    boost::numeric::odeint::integrate_adaptive( stepper_type(), pendulum, x, t, tf, dt );
#endif

    //printf( "%d: t[%f], dt[%f] x[%.9f, %.9f]\n", i, t, dt, x[0], x[1] );

    // build a solution
    solution = scenario->get_solution( Reveal::Core::solution_c::CLIENT, trial, tf );

    // finish building the solution by inserting the state data
    solution->t = tf;
    //solution->state.append_q( x[0] );
    solution->state.append_q( x[0] );
    solution->state.append_dq( x[1] );
    solution->print();

    // submit the solution to the server
    client.submit_solution( solution );
  }
 
/*
  pendulum_c pendulum( EXPERIMENTAL_PENDULUM_L );
  state_type x( 2 );
  double t, dt;
#if defined( ODEINT_V1 )
  stepper_type stepper;
  stepper.adjust_size( x );
#endif
 
  for( unsigned i = 0; i < 10; i++ ) {
    pendulum.sample_trial( i, x[0], x[1], t, dt );

#if defined( ODEINT_V1 )
    stepper.do_step( pendulum, x, t, dt );
#elif defined( ODEINT_V2 )
    boost::numeric::odeint::integrate_adaptive( stepper_type(), pendulum, x, ti, tf, dt );
#endif

    printf( "%d: t[%f], dt[%f] x[%.9f, %.9f]\n", i, t, dt, x[0], x[1] );
  }
*/

  client.terminate();

  return 0;
}

//-----------------------------------------------------------------------------
