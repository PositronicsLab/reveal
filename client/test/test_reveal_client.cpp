#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include <Reveal/client.h>
#include <Reveal/digest.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>

//-----------------------------------------------------------------------------

int main( int argc, char* argv[] ) {
  Reveal::Client::client_c client;

  if( !client.init() ) {
    printf( "ERROR: Client failed to initialize properly.\nExiting.\n" );
    client.terminate();
    exit(1);
  }

  //client.go();

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
  // TODO: error handling

  // for testing purposes, pick a random scenario
  assert( digest->scenarios() );
  scenario = digest->get_scenario( 0 );
  scenario->print();

  // request the scenario details from the server
  client.request_scenario( scenario );
  // TODO: error handling

  // for each trial in the scenario
  for( unsigned i = 0; i < scenario->trials; i++ ) {
    // request the trial from the server

    // create a trial
    trial = scenario->get_trial( i );

    client.request_trial( trial );
    // TODO: error handling

    // **SIMULATION WOULD BE RUN HERE**
    double ti = trial->t;
    double dt = trial->dt;
    double tf = ti + dt;

    // build a solution
    solution = scenario->get_solution( Reveal::Core::solution_c::CLIENT, trial, tf );

    // finish building the solution by inserting the state data
    solution->state.append_q( 0.0 );
    solution->state.append_dq( 0.0 );
    solution->print();

    // submit the solution to the server
    client.submit_solution( solution );
  }

  client.terminate();

  return 0;
}

//-----------------------------------------------------------------------------
