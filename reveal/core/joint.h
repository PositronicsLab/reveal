/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_JOINT_H_
#define _REVEAL_CORE_JOINT_H_

//-----------------------------------------------------------------------------

#include <vector>
#include <string>
#include <assert.h>

#include "reveal/core/component.h"
#include "reveal/core/control.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class joint_c : public component_c {
public:
  std::string id;          //< the unique joint identifier
  control_c control;       //< the control applied to the joint

  /// Default constructor
  joint_c( void ) {}
  /// Destructor
  virtual ~joint_c( void ) {}

  /// Prints the instance data to the console
  void print( void ) {
    printf( "joint[%s]", id.c_str() );
    printf( ", control{" );
    control.print();
    printf( "}" );
  }

  /// Gets the component type.  Fulfills the component_c interface
  /// @return the component type
  virtual component_c::type_e component_type( void ) { return component_c::JOINT; }
};

//-----------------------------------------------------------------------------
}  // namespace Core
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_JOINT_H_
