/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_SERVER_H_
#define _REVEAL_SERVER_H_

//-----------------------------------------------------------------------------

#include <zmq.h>
#include <string.h>
#include <vector>

#include "protocols.pb.h"

//-----------------------------------------------------------------------------

namespace reveal {

//-----------------------------------------------------------------------------

#define PORT 20700
#define RECEIVE_BUFFER_SZ  65

//-----------------------------------------------------------------------------

class Server {
public:
  Server( void );
  virtual ~Server( void );

  bool Init( void );
  void Run( void );
  void Terminate( void );

  static void* Worker( void* arg );

private:
  void* context;
  void* socket;
  std::vector<void*> connections;

  std::string getConnectionString( void );

  ip::Models genTestMessage( void );
};

//-----------------------------------------------------------------------------

}  // namespace reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_SERVER_H_
