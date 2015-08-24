/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_LINK_H_
#define _REVEAL_CORE_LINK_H_

//-----------------------------------------------------------------------------

#include <vector>
#include <string>
#include <assert.h>

#include "reveal/core/component.h"
#include "reveal/core/state.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class link_c : public component_c {
public:
  std::string id;         //< the unique link identifier
  state_c state;          //< the link state

  /// Default constructor
  link_c( void ) {}
  /// Destructor
  virtual ~link_c( void ) {}

  /// Prints the instance data to the console
  void print( void ) {
    printf( "link[%s]", id.c_str() );
    printf( ", state{" );
    state.print();
    printf( "}" );
  }

  /// Gets the component type.  Fulfills the component_c interface
  /// @return the component type
  virtual component_c::type_e component_type( void ) { return component_c::LINK; }

};

//-----------------------------------------------------------------------------
}  // namespace Core
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_LINK_H_
