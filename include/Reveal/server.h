/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_SERVER_H_
#define _REVEAL_SERVER_H_

//-----------------------------------------------------------------------------

#include <string.h>
#include <vector>
#include <pthread.h>

#include <Reveal/connection.h>

#include "net.pb.h"

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

class Server {
public:
  Server( void );
  virtual ~Server( void );

  bool Init( void );
  void Run( void );
  void Terminate( void );

  //static void* AnalyticWorker( void* arg );

private:
  Connection _connection;
  //std::vector<pthread_t> workers;
};

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_SERVER_H_
