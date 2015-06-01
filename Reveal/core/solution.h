/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

solution.h defines the solution_c data-structure that contains data produced by
a client upon completion of a trial within a scenario
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_SOLUTION_H_
#define _REVEAL_CORE_SOLUTION_H_

//-----------------------------------------------------------------------------

#include <string>
#include <vector>
#include <stdio.h>

#include "Reveal/core/pointers.h"
#include "Reveal/core/component.h"
#include "Reveal/core/model.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class solution_c : public component_c {
public:
  /// The enumerated set of valid solution types
  enum type_e {
    MODEL,        //< identifies the solution as model data
    CLIENT        //< identifies the solution as a client generated solution
  };  

  /// Parameterized constructor
  /// @param type the type of this solution
  solution_c( type_e type ) { this->type = type; }
  /// Destructor
  virtual ~solution_c( void ) {}

  std::string experiment_id;     //< the solution's unique experiment identifier
  std::string scenario_id;       //< the solution's unique scenario identifier 
  double t;                      //< the virtual time of this solution
  double real_time;              //< the real time of this solution

  std::vector<model_ptr> models; //< the set of models in this solution
  type_e type;                   //< the type of this solution

  /// Appends a model to this solution
  /// @param model the model to append to this solution
  void insert( model_ptr model ) {  
    models.push_back( model );
  }

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
  virtual component_c::type_e component_type( void ) { return component_c::SOLUTION; }
};

//-----------------------------------------------------------------------------
}  // namespace Core
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_SOLUTION_H_
