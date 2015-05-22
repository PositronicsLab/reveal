/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

solution_set.h defines the solution_set_c data-structure that contains the set
of data produced for all trials attempted by a client and the model solution
for a given scenario. The solution_set is passed to the analyzer.
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_SOLUTION_SET_H_
#define _REVEAL_CORE_SOLUTION_SET_H_

//-----------------------------------------------------------------------------

#include <vector>

#include "Reveal/core/state.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class solution_set_c {
public:

  double time_step;
  double epsilon;

  solution_set_c( void ) {}
  virtual ~solution_set_c( void ) {}


  //Reveal::Core::scenario_ptr 	            scenario;  // scenario data
  Reveal::Core::experiment_ptr              experiment;// identifiers

  Reveal::Core::trial_ptr                   initial_trial;
  std::vector< Reveal::Core::trial_ptr >    trials;    // set of trials
  std::vector< Reveal::Core::solution_ptr > solutions; // set of solutions
  std::vector< Reveal::Core::solution_ptr > models;    // set of model solutions

  void add_trial( Reveal::Core::trial_ptr trial ) {
    trials.push_back( trial );
  }
  
  void add_solution( Reveal::Core::solution_ptr solution ) {
    solutions.push_back( solution );
  }
  
  void add_model( Reveal::Core::solution_ptr model ) {
    models.push_back( model );
  }
  
};

//-----------------------------------------------------------------------------
}  // namespace Core
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_SOLUTION_SET_H_
