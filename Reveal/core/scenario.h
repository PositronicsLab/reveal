/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

scenario.h defines the scenario_c data-structure that contains scenario data.
A scenario is the priniciple set of data required to interact between client
and server
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_SCENARIO_H_
#define _REVEAL_CORE_SCENARIO_H_

//-----------------------------------------------------------------------------

#include <stdio.h>
#include <iostream>
#include <fstream>

#include "Reveal/core/pointers.h"
#include "Reveal/core/trial.h"
#include "Reveal/core/solution.h"
#include "Reveal/core/xml.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class scenario_c {
public:
  scenario_c( void ) {}
  virtual ~scenario_c( void ) {}

  // database mapped fields 
  std::string id;
  std::string description;
  std::vector<std::string> uris;

  // virtual fields
  //unsigned trials;  // counted from the number of trials stored in database
  double sample_rate;
  double sample_start_time;
  double sample_end_time;

/*
  //------
  enum continuity_e {
    INTERVAL,
    RANDOM
  };

  std::string scenario_uuid;
  std::string package_name;
  std::string short_description;
  std::string long_description;
  std::string submitted_by;          //user_c?
  time_c submission_date;
  unsigned number_of_trials;         // fetched by query of trials with uuid
  double sim_start_time;
  double sim_end_time;
  bool timestep_is_constant;
  double time_step;
  continuity_e continuity;
  //------
*/
///*
  void print( void ) const {
    printf( "id[%s]", id.c_str() );
    printf( ", sample_rate[%f]", sample_rate );
    printf( ", sample_start_time[%f]", sample_start_time );
    printf( ", sample_end_time[%f]", sample_end_time );
    printf( ", uris[" );
    for( unsigned i = 0; i < uris.size(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%s", uris.at(i).c_str() );
    }
    printf( "]\n" );
  }
//*/
/*
  solution_ptr get_solution( Reveal::Core::solution_c::type_e type, trial_ptr trial, double t ) {
    assert( trial->scenario_id == id );
    //assert( trial->trial_id < trials );

    // TODO : temporary change of adding type into solution constructor.
    solution_ptr solution = solution_ptr( new solution_c( type ) );
    // !!

    solution->scenario_id = id;
    //solution->trial_id = trial->trial_id;
    solution->t = t;

    return solution;
  }
*/
};

//-----------------------------------------------------------------------------
}  // namespace Core
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_SCENARIO_H_
