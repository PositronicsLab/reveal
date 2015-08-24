/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

trial.h defines the trial_c data-structure that contains data provided to a 
client for a given scenario.  For a single scenario, there may be many trials.
A client evaluates a given trial within a scenario and submits its solution
to the server for analysis
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_TRIAL_H_
#define _REVEAL_CORE_TRIAL_H_

//-----------------------------------------------------------------------------

#include <vector>
#include <stdio.h>
#include <string>

#include "reveal/core/pointers.h"
#include "reveal/core/component.h"
#include "reveal/core/model.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class trial_c : public component_c {
public:
  /// Default constructor
  trial_c( void ) {}
  /// Destructor
  virtual ~trial_c( void ) {}

  std::string scenario_id;        //< the scenario that owns this trial data
  double t;                       //< the time of this trial data
  std::vector<model_ptr> models;  //< the set of model data for this trial

  /// Prints the instance data to the console
  void print( void ) {
    printf( "scenario_id[%s]", scenario_id.c_str() );
    printf( ", t[%f]", t );

    printf( ", models { " );
    for( unsigned i = 0; i < models.size(); i++ ) {
      if( i > 0 ) printf( ", " );
      models[i]->print();
    }
    printf( " }\n" );
  }

  /// Gets the component type.  Fulfills the component_c interface
  /// @return the component type
  virtual component_c::type_e component_type( void ) { return component_c::TRIAL; }
};

//-----------------------------------------------------------------------------
}  // namespace Core
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_TRIAL_H_
