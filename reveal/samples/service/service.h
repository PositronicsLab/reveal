/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

server.h defines the server_c interface.  A Reveal server implementation
uses the server interface to handle client and database interactions
------------------------------------------------------------------------------*/

#ifndef _REVEAL_SAMPLES_SERVICE_H_
#define _REVEAL_SAMPLES_SERVICE_H_

//-----------------------------------------------------------------------------

#include <string.h>
#include <vector>
#include <pthread.h>

#include "reveal/core/connection.h"
#include "worker.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Samples {
//-----------------------------------------------------------------------------

#define MAX_CLIENT_WORKERS 4

//-----------------------------------------------------------------------------

class service_c {
public:

  /// Default constructor
  service_c( void );
  /// Destructor
  virtual ~service_c( void );

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
}  // namespace Samples
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_SAMPLES_SERVICE_H_
