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
#include "reveal/core/pointers.h"
#include "reveal/core/scenario.h"

//----------------------------------------------------------------------------
namespace Reveal {
//----------------------------------------------------------------------------
namespace Core {
//----------------------------------------------------------------------------

class experiment_c {
public:
  /// The enumerated errors produced by experiment operations
  enum error_e {
    ERROR_NONE = 0,
    ERROR_INVALID_EXPERIMENT,
    ERROR_INVALID_SESSION,
    ERROR_INVALID_SCENARIO,
    ERROR_INVALID_TRIAL
  };

  /// Default constructor
  experiment_c( void ) {}
  /// Parameterized constructor
  /// @param scenario the scenario to model this experiment on
  experiment_c( Reveal::Core::scenario_ptr scenario ) {
    scenario_id = scenario->id;
    start_time = scenario->sample_start_time;
    end_time = scenario->sample_end_time;
    time_step = scenario->sample_rate;
  }
  /// Destructor 
  virtual ~experiment_c( void ) {}

  std::string       experiment_id;  //< the unique experiment identifier 
  std::string       session_id;     //< the unique session identifier
  std::string       scenario_id;    //< the unique scenario identifier
  double            start_time;     //< the start time of the experiment
  double            end_time;       //< the end time of the experiment
  double            time_step;      //< the time step used by the experiment 
  double            epsilon;        //< the error allowed in time queries
  unsigned          intermediate_trials_to_ignore; //< the number of trials that will be ignored before resetting state to trial data
  
  /// Prints the experiment data to the console
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
