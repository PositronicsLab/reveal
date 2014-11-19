/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

identity.h defines identity functions.  
-----------------------------------------------------------------------------*/

#ifndef _REVEAL_SERVER_IDENTITY_H_
#define _REVEAL_SERVER_IDENTITY_H_

//-----------------------------------------------------------------------------

#include <string>
#include <uuid/uuid.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Server {

//-----------------------------------------------------------------------------
std::string generate_uuid( void ) {
  uuid_t uuid;
  uuid_generate( uuid );

  char buffer[16];
  //sprintf( buffer, "%X", uuid );
  sprintf( buffer, "%s", uuid );
  
  std::string result = buffer;
  return result;
}

//-----------------------------------------------------------------------------

}  // namespace Server

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_SERVER_IDENTITY_H_
