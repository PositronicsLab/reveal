/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_SERVER_WORKER_H_
#define _REVEAL_SERVER_WORKER_H_

//-----------------------------------------------------------------------------

#include <string.h>
#include <vector>
#include <pthread.h>

#include <Reveal/connection.h>
#include <Reveal/database.h>
#include <Reveal/pointers.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Server {

//-----------------------------------------------------------------------------

class worker_c {
public:
  enum error_e {
    ERROR_NONE = 0
  };

  worker_c( void* context );
  virtual ~worker_c( void );

  bool init( void );
  void work( void );
  void terminate( void );

private:

  error_e service_digest_request( void );
  error_e service_scenario_request( int scenario_id );
  error_e service_trial_request( int scenario_id, int trial_id );
  error_e service_solution_submission( Reveal::Core::solution_ptr solution );
//  error_e analyze_solution( void );

  boost::shared_ptr<Reveal::DB::database_c> _db;
  void* _context;
  Reveal::Core::connection_c _connection;
};

//-----------------------------------------------------------------------------

}  // namespace Server

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_SERVER_WORKER_H_
