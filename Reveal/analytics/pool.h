/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

pool.h defines the pool_c thread pool for analyzers distributed to remote 
processors.  

Note: this class is for future expansion as prototype development was decided
to roll analytics processing directly into server client worker threads
------------------------------------------------------------------------------*/

#ifndef _REVEAL_ANALYTICS_POOL_H_
#define _REVEAL_ANALYTICS_POOL_H_

//-----------------------------------------------------------------------------

#include <vector>

#include "Reveal/analytics/types.h"
#include "Reveal/analytics/worker.h"

//-----------------------------------------------------------------------------

#define MAX_ANALYTIC_WORKERS 4

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Analytics {

//-----------------------------------------------------------------------------

class pool_c {
public:
  pool_c( void );
  virtual ~pool_c( void );

private:
  // master list of workers.  Necessary?
  std::vector< worker_ptr > _workers;

  std::vector< void* > message_queue;  // (template type is protocol message)
  
  std::vector< worker_ptr > _idle;
  std::vector< worker_ptr > _busy;
  std::vector< worker_ptr > _errored;

  Reveal::Core::connection_c _receiver;

public:
  worker_ptr spawn_worker( void );

  error_e init( void );
  error_e cycle( void );
  error_e shutdown( void );

  error_e poll_workers( void );
  error_e poll_server( void );
  error_e process_messages( void );
  error_e dispatch( void );
};

//-----------------------------------------------------------------------------

} // namespace Analytics

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_ANALYTICS_POOL_H_
