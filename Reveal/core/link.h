/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_LINK_H_
#define _REVEAL_CORE_LINK_H_

//-----------------------------------------------------------------------------

#include <vector>
#include <string>
#include <assert.h>

#include "Reveal/core/component.h"
#include "Reveal/core/state.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class link_c : public component_c {
public:
  std::string id;
  state_c state;

  link_c( void ) {}
  virtual ~link_c( void ) {}

  void print( void ) {
    printf( "link[%s]", id.c_str() );
    printf( ", state{" );
    state.print();
    printf( "}" );
  }

  virtual component_c::type_e component_type( void ) { return component_c::LINK; }

};

//-----------------------------------------------------------------------------
}  // namespace Core
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_LINK_H_
