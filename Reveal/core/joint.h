/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_JOINT_H_
#define _REVEAL_CORE_JOINT_H_

//-----------------------------------------------------------------------------

#include <vector>
#include <string>
#include <assert.h>

#include "Reveal/core/component.h"
#include "Reveal/core/control.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class joint_c : public component_c {
public:
  std::string id;
  control_c control;

  joint_c( void ) {}
  virtual ~joint_c( void ) {}

  void print( void ) {
    printf( "joint[%s]", id.c_str() );
    printf( ", control{" );
    control.print();
    printf( "}" );
  }

  virtual component_c::type_e component_type( void ) { return component_c::JOINT; }
};

//-----------------------------------------------------------------------------
}  // namespace Core
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_JOINT_H_
