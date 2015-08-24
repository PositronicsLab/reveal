/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

component.h defines the component_c interface that allows class self 
identification for xml writable components.
------------------------------------------------------------------------------*/
#ifndef _REVEAL_CORE_COMPONENT_H_
#define _REVEAL_CORE_COMPONENT_H_

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class component_c {
public:
  /// The enumeration of available component types
  enum type_e {
    TRIAL,
    SOLUTION,
    LINK,
    JOINT
  };

  /// The pure virtual method components must provide to self identify
  virtual type_e component_type( void ) = 0;
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_COMPONENT_H_
