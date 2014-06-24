#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include <Reveal/client.h>
#include <Reveal/digest.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>

//-----------------------------------------------------------------------------
static Reveal::Client::client_c client;

//-----------------------------------------------------------------------------
void report_client_error( Reveal::Client::client_c::error_e error ) {
  if( error == Reveal::Client::client_c::ERROR_WRITE ) {
    printf( "ERROR: the Reveal Client reported an error while writing to the server connection\n" );
  } else if( error == Reveal::Client::client_c::ERROR_READ ) {
    printf( "ERROR: the Reveal Client reported an error while reading from the server connection\n" );
  } else if( error == Reveal::Client::client_c::ERROR_CONNECTION ) {
    printf( "ERROR: the Reveal Client reported an error while attempting to connect to the server\n" );
  } else if( error == Reveal::Client::client_c::ERROR_EXCHANGE_BUILD ) {
    printf( "ERROR: the Reveal Client reported an error while attempting to build the message exchange request protocol\n" );
  } else if( error == Reveal::Client::client_c::ERROR_EXCHANGE_PARSE ) {
    printf( "ERROR: the Reveal Client reported an error while attempting to parse the message exchange response protocol\n" );
  } else if( error == Reveal::Client::client_c::ERROR_EXCHANGE_RESPONSE ) {
    printf( "ERROR: the Reveal Client reported a general response error detected in the exchange protocol\n" );
  } else if( error == Reveal::Client::client_c::ERROR_INVALID_SCENARIO_REQUEST ) {
    printf( "ERROR: the Reveal Client received an error from the server indicating that the requested scenario is invalid\n" );
  } else if( error == Reveal::Client::client_c::ERROR_INVALID_TRIAL_REQUEST ) {
    printf( "ERROR: the Reveal Client received an error from the server indicating that the requested trial is invalid\n" );
  } else if( error == Reveal::Client::client_c::ERROR_INVALID_SOLUTION ) {
    printf( "ERROR: the Reveal Client received an error from the server indicating that the solution submitted was malformed and could not be accepted\n" );
  }
}

//-----------------------------------------------------------------------------
void test_bad_scenario_query( void ) {
  Reveal::Core::digest_ptr digest;
  Reveal::Core::scenario_ptr scenario;
  Reveal::Core::trial_ptr trial;
  Reveal::Core::solution_ptr solution;
  
  Reveal::Client::client_c::error_e error;

  // request the digest
  error = client.request_digest( digest );
  if( error != Reveal::Client::client_c::ERROR_NONE ) {
    report_client_error( error );
  }

  // for testing purposes, pick a bad scenario
  assert( digest->scenarios() );
  scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );
  scenario->id = digest->scenarios()+100;

  // request the scenario details from the server
  error = client.request_scenario( scenario );
  if( error != Reveal::Client::client_c::ERROR_NONE ) {
    report_client_error( error );
  }
}

//-----------------------------------------------------------------------------
void test_bad_trial_query( void ) {
  Reveal::Core::digest_ptr digest;
  Reveal::Core::scenario_ptr scenario;
  Reveal::Core::trial_ptr trial;
  Reveal::Core::solution_ptr solution;
  
  Reveal::Client::client_c::error_e error;

  // request the digest
  error = client.request_digest( digest );
  if( error != Reveal::Client::client_c::ERROR_NONE ) {
    report_client_error( error );
  }

  // for testing purposes, pick a bad scenario
  assert( digest->scenarios() );
  scenario = digest->get_scenario( 0 );
  scenario->print();

  // request the scenario details from the server
  error = client.request_scenario( scenario );
  if( error != Reveal::Client::client_c::ERROR_NONE ) {
    report_client_error( error );
  }

  // create a trial
  trial = scenario->get_trial( scenario->trials - 1 );
  trial->trial_id = scenario->trials;

  error = client.request_trial( trial );
  if( error != Reveal::Client::client_c::ERROR_NONE ) {
    report_client_error( error );
  }
/*
  // **SIMULATION WOULD BE RUN HERE**
  double ti = trial->t;
  double dt = trial->dt;
  double tf = ti + dt;

  // build a solution
  solution = scenario->get_solution( trial, tf );

  // finish building the solution by inserting the state data
  solution->state.append_q( 0.0 );
  solution->state.append_dq( 0.0 );
  solution->print();

  // submit the solution to the server
  client.submit_solution( solution );
  // TODO : error handling for bad transport
*/
}

//-----------------------------------------------------------------------------

int main( int argc, char* argv[] ) {

  if( !client.init() ) {
    printf( "ERROR: Client failed to initialize properly.\nExiting.\n" );
    client.terminate();
    exit(1);
  }

  if( !client.connect() ) {
    printf( "ERROR: Client failed to connect properly.\nExiting.\n" );
    client.terminate();
    exit(1);
  }

  test_bad_scenario_query();
  test_bad_trial_query();

  client.terminate();

  return 0;
}

//-----------------------------------------------------------------------------
