/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

experiment.h defines the experiment_c class that encapsulates and manages the 
prescription and exchange of data with regard to the execution of an attempt 
by a client to solve a scenario.
-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_EXPERIMENT_
#define _REVEAL_CORE_EXPERIMENT_

//----------------------------------------------------------------------------

#include <stdio.h>
#include <string>
#include "Reveal/core/pointers.h"
#include "Reveal/core/scenario.h"

//----------------------------------------------------------------------------
namespace Reveal {
//----------------------------------------------------------------------------
namespace Core {
//----------------------------------------------------------------------------

class experiment_c {
public:

  enum error_e {
    ERROR_NONE = 0,
    ERROR_INVALID_EXPERIMENT,
    ERROR_INVALID_SESSION,
    ERROR_INVALID_SCENARIO,
    ERROR_INVALID_TRIAL
  };

  experiment_c( void ) {}
  experiment_c( Reveal::Core::scenario_ptr scenario ) {
    scenario_id = scenario->id;
    start_time = scenario->sample_start_time;
    end_time = scenario->sample_end_time;
    time_step = scenario->sample_rate;
  }
  virtual ~experiment_c( void ) {}

  error_e _error;

  std::string       experiment_id;
  std::string       session_id;
  std::string       scenario_id;
  double            start_time;
  double            end_time;
  double            time_step;
  double            epsilon;
  unsigned          intermediate_trials_to_ignore;
  
  //                creation_timestamp;
/*
  //------
  time_c submission_date;
  unsigned number_of_trials;
  unsigned steps_per_trial;
  unsigned current_trial_index;
  unsigned start_trial;
  unsigned end_trial;
  double sim_start_time;
  double sim_end_time;
  double time_step;
  //------
*/
  void print( void ) const {
    printf( "experiment_id[%s]", experiment_id.c_str() );
    printf( ", session_id[%s]", session_id.c_str() );
    printf( ", scenario_id[%s]", scenario_id.c_str() );
    printf( ", start_time[%f]", start_time );
    printf( ", end_time[%f]", end_time );
    printf( ", time_step[%f]", time_step );
    printf( ", epsilon[%f]\n", epsilon );
  }
};

//----------------------------------------------------------------------------
} // namespace Core
//----------------------------------------------------------------------------
} // namespace Reveal
//----------------------------------------------------------------------------

#endif // _REVEAL_CORE_EXPERIMENT_
