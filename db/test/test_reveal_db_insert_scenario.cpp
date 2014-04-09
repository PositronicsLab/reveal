#include <stdio.h>
#include <stdlib.h>

#ifndef _REVEAL_SERVER_SERVER_H_
#define _REVEAL_SERVER_SERVER_H_
#endif

#include <Reveal/database.h>
#include <Reveal/pendulum.h>
#include <Reveal/pointers.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/model_solution.h>

int main( int argc, char* argv[] ) {
  Reveal::DB::database_c db( "localhost" );

  db.open();

  std::string scenario_doc = "scenario";
  std::string trial_doc = "trial";
  std::string expected_solution_doc = "model_solution";

  // generate the pendulum scenario entry
  Reveal::Core::scenario_ptr scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );

  scenario->name = "pendulum";
  scenario->trials = 10;
  scenario->uris.push_back( "http://www.gazebosim.org/" );
  scenario->uris.push_back( "http://www.osrffoundation.org/" );
  db.insert( scenario );

  // generate the entries for all pendulum trials 
  for( unsigned i = 0; i < 10; i++ ) {
    double q, dq, t, dt;
    pendulum_c::sample_trial( i, q, dq, t, dt ); 

    Reveal::Core::trial_ptr trial = Reveal::Core::trial_ptr( new Reveal::Core::trial_c() );

    trial->scenario = scenario->name;
    trial->index = i;
    trial->t = t;
    trial->dt = dt;
    trial->state.append_q( q );
    trial->state.append_dq( dq );
    // TODO: add controls
    db.insert( trial );
  }

  // generate the entries for all pendulum expected solutions 
  pendulum_c pendulum( EXPERIMENTAL_PENDULUM_L );

  for( unsigned i = 0; i < 10; i++ ) {
    std::vector<double> x, epsilon_q, epsilon_dq;
    double q, dq, ti, dt, tf;
    pendulum_c::sample_trial( i, q, dq, ti, dt );     

    x.push_back( q );
    x.push_back( dq );
    tf = ti + dt;
    boost::numeric::odeint::integrate_adaptive( stepper_type(), pendulum, x, ti, tf, dt );

    Reveal::Core::model_solution_ptr solution = Reveal::Core::model_solution_ptr( new Reveal::Core::model_solution_c() );

    solution->scenario = scenario->name;
    solution->index = i;
    solution->t = tf;
    solution->state.append_q( x[0] );
    solution->state.append_dq( x[1] );
    solution->epsilon.append_q( PENDULUM_VALIDATION_EPSILON );
    solution->epsilon.append_dq( PENDULUM_VALIDATION_EPSILON );
    db.insert( solution );
  }

  return 0;
}
