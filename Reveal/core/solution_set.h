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

  double time_step;            //< the time step this solution set uses
  double epsilon;              //< the error allowed in time comparison

  /// Default constructor
  solution_set_c( void ) {}
  /// Destructor
  virtual ~solution_set_c( void ) {}


  Reveal::Core::experiment_ptr experiment; //< associated experiment
  Reveal::Core::trial_ptr initial_trial;   //< initial trial data
  std::vector<Reveal::Core::trial_ptr>    trials;   //< current trial data
  std::vector<Reveal::Core::solution_ptr> solutions;//< current solution data
  std::vector<Reveal::Core::solution_ptr> models;   //< current model solutions

  /// Appends a trial into this instance's trial data
  /// @param trial the trial data to append
  void add_trial( Reveal::Core::trial_ptr trial ) {
    trials.push_back( trial );
  }
  
  /// Appends a solution into this instance's solution data
  /// @param solution the solution data to append
  void add_solution( Reveal::Core::solution_ptr solution ) {
    solutions.push_back( solution );
  }
  
  /// Appends a model solution into this instance's model solution data
  /// @param model the model solution data to append
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
