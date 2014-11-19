#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include <Reveal/database.h>
#include <Reveal/pointers.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>

#include <Reveal/query.h>

//-----------------------------------------------------------------------------
int main( int argc, char* argv[] ) {
  Reveal::DB::database_c db( "localhost" );

  db.open();

  Reveal::DB::query_c query;
  std::string scenario_id = "test";

  Reveal::Core::scenario_ptr scenario;
  db.query( scenario, scenario_id );
  scenario->print();

  for( unsigned i = 0; i < scenario->trials; i++ ) {
    Reveal::Core::trial_ptr trial;
    db.query( trial, scenario->id, i );
    trial->print();
  }

  for( unsigned i = 0; i < scenario->trials; i++ ) {
    Reveal::Core::solution_ptr solution;
    db.query( solution, Reveal::Core::solution_c::MODEL, scenario->id, i );
    solution->print();
  }

  db.close();

  return 0;
}
