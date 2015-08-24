#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include <Reveal/pointers.h>
#include <Reveal/authorization.h>
#include <Reveal/user.h>
#include <Reveal/client.h>
#include <Reveal/digest.h>
#include <Reveal/experiment.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>

//-----------------------------------------------------------------------------
Reveal::Client::client_c client;

//-----------------------------------------------------------------------------
void test_identified_user( void ) {
  Reveal::Core::user_ptr user = Reveal::Core::user_ptr(new Reveal::Core::user_c() );
  user->id = "alice";

  Reveal::Core::authorization_ptr auth = Reveal::Core::authorization_ptr(new Reveal::Core::authorization_c() );
  auth->set_type( Reveal::Core::authorization_c::TYPE_IDENTIFIED );
  auth->set_user( user->id );

  Reveal::Client::client_c::error_e client_error;
  client_error = client.request_authorization( auth );
  if( client_error != Reveal::Client::client_c::ERROR_NONE ) {
    printf( "ERROR: client failed to gain identified authorization\n" );
  } else {
    printf( "SUCCESS: identified client gained authorization: session[%s]\n", auth->get_session().c_str() );
  }
}

//-----------------------------------------------------------------------------
void test_anonymous_user( void ) {

  Reveal::Core::authorization_ptr auth = Reveal::Core::authorization_ptr(new Reveal::Core::authorization_c() );
  auth->set_type( Reveal::Core::authorization_c::TYPE_ANONYMOUS );

  Reveal::Client::client_c::error_e client_error;
  client_error = client.request_authorization( auth );
  if( client_error != Reveal::Client::client_c::ERROR_NONE ) {
    printf( "ERROR: client failed to gain anonymous authorization\n" );
  } else {
    printf( "SUCCESS: anonymous client gained authorization: session[%s]\n", auth->get_session().c_str() );
  }
}

//-----------------------------------------------------------------------------
int main( int argc, char* argv[] ) {

  if( !client.init() ) {
    printf( "ERROR: Client failed to initialize properly.\nExiting.\n" );
    client.terminate();
    exit(1);
  }

  Reveal::Core::digest_ptr digest;
  Reveal::Core::scenario_ptr scenario;
  Reveal::Core::experiment_ptr experiment;
  Reveal::Core::trial_ptr trial;
  Reveal::Core::solution_ptr solution;

  if( !client.connect() ) {
    printf( "ERROR: Client failed to connect properly.\nExiting.\n" );
    client.terminate();
    exit(1);
  }

  Reveal::Core::user_ptr user = Reveal::Core::user_ptr(new Reveal::Core::user_c() );
  user->id = "alice";

  Reveal::Core::authorization_ptr auth = Reveal::Core::authorization_ptr(new Reveal::Core::authorization_c() );
  auth->set_type( Reveal::Core::authorization_c::TYPE_IDENTIFIED );
  auth->set_user( user->id );

  Reveal::Client::client_c::error_e client_error;
  client_error = client.request_authorization( auth );
  if( client_error != Reveal::Client::client_c::ERROR_NONE ) {
    printf( "ERROR: client failed to gain identified authorization\n" );
  } else {
    printf( "SUCCESS: identified client gained authorization: session[%s]\n", auth->get_session().c_str() );
  }

  // request the digest
  client.request_digest( auth, digest );
  // TODO: error handling

  // for testing purposes, pick a random scenario
  assert( digest->scenarios() );
  scenario = digest->get_scenario( 0 );
  scenario->print();

  client_error = client.request_experiment( auth, scenario, experiment );
  if( client_error != Reveal::Client::client_c::ERROR_NONE ) {
    printf( "ERROR: client failed to receive experiment\n" );
  } else {
    printf( "SUCCESS: client received: experiment[%s]\n", experiment->experiment_id.c_str() );
    //experiment->print();
    scenario->print();
  }

  // for each trial in the scenario
  for( unsigned i = 0; i < scenario->trials; i++ ) {
    // request the trial from the server

    // create a trial
    trial = scenario->get_trial( i );

    client.request_trial( auth, experiment, trial );
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
    client.submit_solution( auth, experiment, solution );
  }

  client.terminate();

  return 0;
}

//-----------------------------------------------------------------------------
