#include <stdio.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------

#ifndef _REVEAL_SERVER_SERVER_H_
#define _REVEAL_SERVER_SERVER_H_
#endif

//-----------------------------------------------------------------------------

#include <Reveal/database.h>
#include <Reveal/pointers.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>

//-----------------------------------------------------------------------------

std::string scenario_doc = "scenario";
std::string trial_doc = "trial";
std::string expected_solution_doc = "model";

//-----------------------------------------------------------------------------

boost::shared_ptr<Reveal::DB::database_c> db;

//-----------------------------------------------------------------------------
void insert_test1( void ) {
  // generate the scenario entry
  Reveal::Core::scenario_ptr scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );

  int scenario_id = 1;
  int trials = 10;

  scenario->id = scenario_id;
  scenario->name = "test1";
  scenario->description = "test case 1";
  scenario->trials = trials;
  scenario->uris.push_back( "http://www.gazebosim.org/" );
  scenario->uris.push_back( "http://www.osrffoundation.org/" );
  db->insert( scenario );

  std::vector<Reveal::Core::trial_ptr> trial_set;

  // generate the trial entries 
  for( int i = 0; i < trials; i++ ) {
    double q, dq, u, t, dt;

    Reveal::Core::trial_ptr trial = Reveal::Core::trial_ptr( new Reveal::Core::trial_c() );

    t = (double)rand() / (double)RAND_MAX * 5.0;
    dt = 0.001;

    q = 0.0;
    dq = 0.0;
    u = 0.0;

    trial->scenario_id = scenario_id;
    trial->trial_id = i;
    trial->t = t;
    trial->dt = dt;
    trial->state.append_q( q );
    trial->state.append_dq( dq );
    trial->control.append_u( u );
    trial_set.push_back( trial );

    db->insert( trial );
  }

  // generate the solution entries
  for( int i = 0; i < trials; i++ ) {
    double q, dq, ti, dt, tf;

    q = trial_set[i]->state.q(0);
    dq = trial_set[i]->state.dq(0);
    ti = trial_set[i]->t;
    dt = trial_set[i]->dt;

    tf = ti + dt;

    Reveal::Core::solution_ptr solution = Reveal::Core::solution_ptr( new Reveal::Core::solution_c( Reveal::Core::solution_c::MODEL ) );

    solution->scenario_id = scenario_id;
    solution->trial_id = i;
    solution->t = tf;
    solution->state.append_q( q );
    solution->state.append_dq( dq );
    db->insert( solution );
  }

}

//-----------------------------------------------------------------------------
void insert_test2( void ) {
  // generate the scenario entry
  Reveal::Core::scenario_ptr scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );

  int scenario_id = 2;
  int trials = 20;

  scenario->id = scenario_id;
  scenario->name = "test2";
  scenario->description = "test case 2";
  scenario->trials = (int)trials;
  scenario->uris.push_back( "http://www.osrffoundation.org/" );
  scenario->uris.push_back( "http://www.gazebosim.org/" );
  scenario->uris.push_back( "https://bitbucket.org/osrf/gazebo" );
  db->insert( scenario );

  std::vector<Reveal::Core::trial_ptr> trial_set;

  // generate the trial entries 
  for( int i = 0; i < trials; i++ ) {
    double q, dq, u, t, dt;

    Reveal::Core::trial_ptr trial = Reveal::Core::trial_ptr( new Reveal::Core::trial_c() );

    t = (double)rand() / (double)RAND_MAX * 5.0;
    dt = 0.001;

    q = 0.0;
    dq = 0.0;
    u = 0.0;

    trial->scenario_id = scenario_id;
    trial->trial_id = i;
    trial->t = t;
    trial->dt = dt;
    trial->state.append_q( q );
    trial->state.append_dq( dq );
    trial->control.append_u( u );
    trial_set.push_back( trial );

    db->insert( trial );
  }

  // generate the solution entries
  for( int i = 0; i < trials; i++ ) {
    double q, dq, ti, dt, tf;

    q = trial_set[i]->state.q(0);
    dq = trial_set[i]->state.dq(0);
    ti = trial_set[i]->t;
    dt = trial_set[i]->dt;

    tf = ti + dt;

    Reveal::Core::solution_ptr solution = Reveal::Core::solution_ptr( new Reveal::Core::solution_c( Reveal::Core::solution_c::MODEL ) );

    solution->scenario_id = scenario_id;
    solution->trial_id = i;
    solution->t = tf;
    solution->state.append_q( q );
    solution->state.append_dq( dq );
    db->insert( solution );
  }
}

//-----------------------------------------------------------------------------
int main( int argc, char* argv[] ) {

  db = boost::shared_ptr<Reveal::DB::database_c>( new Reveal::DB::database_c( "localhost" ) );
  //db( "localhost" );

  db->open();

  insert_test1();
  insert_test2();

  db->close();

  return 0;
}

//-----------------------------------------------------------------------------
