/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_PROTOCOL_MANAGER_H_
#define _REVEAL_PROTOCOL_MANAGER_H_

//-----------------------------------------------------------------------------

#include "net.pb.h"

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

class protocol_manager_c {
public:
  static void start( void ) { GOOGLE_PROTOBUF_VERIFY_VERSION; }
  static void shutdown( void ) { google::protobuf::ShutdownProtobufLibrary(); }
};

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_PROTOCOL_MANAGER_H_
