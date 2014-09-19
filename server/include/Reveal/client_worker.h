/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

client_worker.h defines the worker_c thread that handles the client connections
to the server
------------------------------------------------------------------------------*/

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
    ERROR_NONE = 0,
    ERROR_QUERY,
    ERROR_CREATE
  };

  worker_c( void* context );
  virtual ~worker_c( void );

  bool init( void );
  void work( void );
  void terminate( void );

private:
  error_e send_valid_handshake_response( Reveal::Core::authorization_ptr auth );
  error_e send_invalid_handshake_response( Reveal::Core::authorization_ptr auth );

  error_e authorize( Reveal::Core::authorization_ptr auth );
  error_e service_failed_authorization( Reveal::Core::authorization_ptr auth );
  error_e service_handshake_request( Reveal::Core::authorization_ptr auth );

  error_e service_digest_request( Reveal::Core::authorization_ptr auth );
  error_e service_experiment_request( Reveal::Core::authorization_ptr auth, std::string scenario_id );
  //error_e service_scenario_request( int scenario_id );
  error_e service_trial_request( Reveal::Core::authorization_ptr auth, Reveal::Core::experiment_ptr experiment, int trial_id );
  error_e service_solution_submission( Reveal::Core::authorization_ptr auth, Reveal::Core::experiment_ptr, Reveal::Core::solution_ptr solution );
//  error_e analyze_solution( void );

  boost::shared_ptr<Reveal::DB::database_c> _db;
  void* _context;
  Reveal::Core::connection_c _connection;

  bool is_user_valid( Reveal::Core::authorization_ptr auth, Reveal::Core::user_ptr& user );
  bool is_session_valid( Reveal::Core::authorization_ptr auth, Reveal::Core::session_ptr& session );
  bool is_experiment_valid( Reveal::Core::authorization_ptr auth, Reveal::Core::experiment_ptr experiment_request, Reveal::Core::experiment_ptr& experiment_record );

  bool create_session( Reveal::Core::authorization_ptr auth, Reveal::Core::session_ptr& session );
  bool create_experiment( Reveal::Core::authorization_ptr auth, Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr& experiment );
};

//-----------------------------------------------------------------------------

}  // namespace Server

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_SERVER_WORKER_H_
