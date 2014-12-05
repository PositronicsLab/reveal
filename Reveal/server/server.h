/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

server.h defines the server_c interface.  A Reveal server implementation
uses the server interface to handle client and database interactions
------------------------------------------------------------------------------*/

#ifndef _REVEAL_SERVER_SERVER_H_
#define _REVEAL_SERVER_SERVER_H_

//-----------------------------------------------------------------------------

#include <string.h>
#include <vector>
#include <pthread.h>

#include "Reveal/core/connection.h"
#include "Reveal/server/client_worker.h"

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Server {

//-----------------------------------------------------------------------------

#define MAX_CLIENT_WORKERS 4
//#define MAX_ANALYTIC_WORKERS 4

//-----------------------------------------------------------------------------

class server_c {
public:

  server_c( void );
  virtual ~server_c( void );

  bool init( void );
  void run( void );
  void terminate( void );

  static void* worker_thread( void* context );


private:
  Reveal::Core::connection_c _clientconnection;
  Reveal::Core::connection_c _workerconnection;

  std::vector<pthread_t> workers;

};

//-----------------------------------------------------------------------------

}  // namespace Server

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_SERVER_SERVER_H_
