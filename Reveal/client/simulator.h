#ifndef _REVEAL_CLIENT_SIMULATOR_H_
#define _REVEAL_CLIENT_SIMULATOR_H_
//-----------------------------------------------------------------------------

#include "Reveal/core/authorization.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Client {
//-----------------------------------------------------------------------------

typedef bool (*experiment_f)( Reveal::Core::authorization_ptr auth );

//-----------------------------------------------------------------------------
class simulator_c {
public:
  virtual bool experiment( Reveal::Core::authorization_ptr auth ) = 0;
};

//-----------------------------------------------------------------------------
} // namespace Client
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
#endif // _REVEAL_CLIENT_SIMULATOR_H_
