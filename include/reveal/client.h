/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CLIENT_H_
#define _REVEAL_CLIENT_H_

//-----------------------------------------------------------------------------

#include <zmq.h>
#include <string.h>

#include "protocols.pb.h"

//-----------------------------------------------------------------------------

namespace reveal {

//-----------------------------------------------------------------------------

#define PORT 20700
#define SENDER_CHUNK_SZ    64

//-----------------------------------------------------------------------------

class Client {
public:
  Client( void );
  virtual ~Client( void );

  bool Init( void );
  bool Connect( void );
  void Terminate( void );

private:
  void* context;
  void* socket;

  std::string getConnectionString( void );

  ip::Models genTestMessage( void );
};

//-----------------------------------------------------------------------------

} // namespace reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CLIENT_H_

