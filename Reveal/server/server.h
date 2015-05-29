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
#include "Reveal/server/worker.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Server {
//-----------------------------------------------------------------------------

#define MAX_CLIENT_WORKERS 4

//-----------------------------------------------------------------------------

class server_c {
public:

  /// Default constructor
  server_c( void );
  /// Destructor
  virtual ~server_c( void );

  /// Encapsulates the initialization of the server system.
  /// @return true if the server was able to successfully initialize and is 
  ///         fully prepared to operate OR false if initialization failed for
  ///         any reason
  bool init( void );

  /// Encapsulates the server main loop which is entirely routing connections to
  /// worker threads
  void run( void );

  /// Encapsulates the clean shutdown of the server
  void terminate( void );

  /// The client connection worker thread function
  /// @param context the ipc connection context shared by all threads in the
  ///        server process
  /// @return NULL
  static void* worker_thread( void* context );

private:
  Reveal::Core::connection_c _clientconnection; //< Central outgoing connection
  Reveal::Core::connection_c _workerconnection; //< Internal routing connection
  std::vector<pthread_t> workers;  //< Pool of all worker threads spawned

};

//-----------------------------------------------------------------------------
}  // namespace Server
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_SERVER_SERVER_H_
