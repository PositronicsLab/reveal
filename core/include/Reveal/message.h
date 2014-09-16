/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

message.h defines the message_c wrapper that encapsulates data passed via 
socket.  
-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_MESSAGE_H_
#define _REVEAL_CORE_MESSAGE_H_

//-----------------------------------------------------------------------------

#include <string>
#include <vector>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Core {

//-----------------------------------------------------------------------------

class message_c {
public:
  std::string sender;
  std::string body;

  int end_of_header;
  std::vector< std::string > data;

  message_c( void ) {
    end_of_header = -1;
  }
  virtual ~message_c( void ) { }

};

//-----------------------------------------------------------------------------

}  // namespace Core

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_MESSAGE_H_
