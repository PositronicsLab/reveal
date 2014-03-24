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

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

#define MAX_CLIENT_WORKERS 4
#define MAX_ANALYTIC_WORKERS 4

//-----------------------------------------------------------------------------
// Experimental Values
//-----------------------------------------------------------------------------

#define DB_SIMULATED_WORK_TIME 2

//-----------------------------------------------------------------------------

class server_c {
public:
  server_c( void );
  virtual ~server_c( void );

  bool init( void );
  void run( void );
  void terminate( void );

  static void* client_worker( void* context );

private:
  connection_c _clientconnection;
  connection_c _workerconnection;

  std::vector<pthread_t> workers;

};

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_SERVER_H_
