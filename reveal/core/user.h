/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

user.h defines the user_c data structure that maintains user specific 
information for a given Reveal session
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_USER_H_
#define _REVEAL_CORE_USER_H_

//-----------------------------------------------------------------------------

#include <string>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class user_c {
public:

  /// Default constructor
  user_c( void ) { }
  /// Destructor
  virtual ~user_c( void ) { }

  std::string            id;   //< unique user identifier
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_USER_H_
