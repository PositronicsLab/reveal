/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

identity.h defines identity functions.  
-----------------------------------------------------------------------------*/

#ifndef _REVEAL_SERVER_IDENTITY_H_
#define _REVEAL_SERVER_IDENTITY_H_

//-----------------------------------------------------------------------------

#include <string>
#include <sstream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

// NOTE: generate_uuid added to core/system.h
// TODO: remove from here and refactor references to core

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Server {
//-----------------------------------------------------------------------------
std::string generate_uuid( void ) {

  std::stringstream ss;
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  ss << uuid;
  return ss.str();
}

//-----------------------------------------------------------------------------
}  // namespace Server
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_SERVER_IDENTITY_H_
