/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

experiment.h defines the experiment_c class that encapsulates and manages the 
prescription and exchange of data with regard to the execution of an attempt 
by a client to solve a scenario.
-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_EXPERIMENT_
#define _REVEAL_CORE_EXPERIMENT_

//----------------------------------------------------------------------------

#include <string>
#include <Reveal/pointers.h>

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
  virtual ~experiment_c( void ) {}

  error_e _error;

  std::string       experiment_id;
  std::string       session_id;
  std::string       scenario_id;
  int               number_of_trials;
  std::vector<int>  trial_prescription;
  int               current_trial_index;
  
  //                creation_timestamp;
};

//----------------------------------------------------------------------------

} // namespace Core

//----------------------------------------------------------------------------

} // namespace Reveal

//----------------------------------------------------------------------------

#endif // _REVEAL_CORE_EXPERIMENT_
