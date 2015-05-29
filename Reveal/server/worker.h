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

#include "Reveal/core/connection.h"
#include "Reveal/db/database.h"
#include "Reveal/core/pointers.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Server {
//-----------------------------------------------------------------------------

class worker_c {
private:
  boost::shared_ptr<Reveal::DB::database_c> _db; //< the reveal database
  void* _context;          //< the parent server process' communication context
  Reveal::Core::connection_c _connection;  //< the connection to the client
public:
  /// Enumerated error codes used by the worker
  enum error_e {
    ERROR_NONE = 0,        //< indicates that no error has occurred
    ERROR_QUERY,           //< an error related to a db query
    ERROR_CREATE           //< an error related to a creating a record in the db
  };

  /// Constructor requiring the parent process' communication context
  /// @param context parent process' communication context
  worker_c( void* context );
  /// Destructor
  virtual ~worker_c( void );

  /// Encapsulates the worker's full initialization steps
  /// @return true if the worker has properly initialized OR false if the worker
  ///         was unable to be initialize for any reason
  bool init( void );

  /// Encapsulates the worker's main loop
  void work( void );
  
  /// Encapsulates cleanup and shutdown of all the worker's resources
  void terminate( void );

private:
  /// Encapsulates replying to a client authorization request with a valid 
  /// authorization
  /// @param auth the authorization granted to the client
  /// @return returns ERROR_NONE if the operation succeeded OR another 
  ///         enumerated value if the operation failed
  error_e send_valid_handshake_response( Reveal::Core::authorization_ptr auth );

  /// Encapsulates replying to a client authorization request with an invalid 
  /// authorization
  /// @param auth the invalidated client authorization
  /// @return returns ERROR_NONE if the operation succeeded OR another 
  ///         enumerated value if the operation failed
  error_e send_invalid_handshake_response( Reveal::Core::authorization_ptr auth );

  /// Encapsulates validating a client's authorization
  /// @param auth the authorization submitted by the client
  /// @return returns ERROR_NONE if the operation succeeded OR another 
  ///         enumerated value if the operation failed
  error_e authorize( Reveal::Core::authorization_ptr auth );

  /// Encapsulates handling of a failed authorization
  /// @param auth the authorization submitted by the client
  /// @return returns ERROR_NONE if the operation succeeded OR another 
  ///         enumerated value if the operation failed 
  error_e service_failed_authorization( Reveal::Core::authorization_ptr auth );

  /// Encapsulates servicing of a handshake attempt
  /// @param auth the authorization submitted by the client
  /// @return returns ERROR_NONE if the operation succeeded OR another 
  ///         enumerated value if the operation failed 
  error_e service_handshake_request( Reveal::Core::authorization_ptr auth );

  /// Encapsulates servicing of a client request for a digest
  /// @param auth the authorization submitted by the client
  /// @return returns ERROR_NONE if the operation succeeded OR another 
  ///         enumerated value if the operation failed 
  error_e service_digest_request( Reveal::Core::authorization_ptr auth );

  /// Encapsulates servicing of a client request for an experiment
  /// @param auth the authorization submitted by the client
  /// @param scenario_id the uuid of the scenario for which to create an 
  ///        experiment
  /// @param experiment the client side parameters to assign to the experiment
  /// @return returns ERROR_NONE if the operation succeeded OR another 
  ///         enumerated value if the operation failed 
  error_e service_experiment_request( Reveal::Core::authorization_ptr auth, std::string scenario_id, Reveal::Core::experiment_ptr experiment );

  /// Encapsulates servicing of a client request for a trial
  /// @param auth the authorization submitted by the client
  /// @param experiment the experiment for which to fetch the trial
  /// @param t the time of the trial
  /// @param epsilon the allowable error in the time query 
  /// @return returns ERROR_NONE if the operation succeeded OR another 
  ///         enumerated value if the operation failed 
  error_e service_trial_request( Reveal::Core::authorization_ptr auth, Reveal::Core::experiment_ptr experiment, double t, double epsilon );

  /// Encapsulates servicing of a client submission of a solution
  /// @param auth the authorization submitted by the client
  /// @param experiment the experiment for which to submit the solution
  /// @param solution the solution submitted by the client
  /// @return returns ERROR_NONE if the operation succeeded OR another 
  ///         enumerated value if the operation failed 
  error_e service_solution_submission( Reveal::Core::authorization_ptr auth, Reveal::Core::experiment_ptr, Reveal::Core::solution_ptr solution );

  /// Validates whether a user is valid
  /// @param auth the authorization submitted by the client
  /// @param user the user to validate
  /// @return returns true if the user is valid OR false if the validation 
  ///         failed for any reason
  bool is_user_valid( Reveal::Core::authorization_ptr auth, Reveal::Core::user_ptr& user );

  /// Validates whether a session is valid
  /// @param auth the authorization submitted by the client
  /// @param session the session to validate
  /// @return returns true if the session is valid OR false if the validation 
  ///         failed for any reason
  bool is_session_valid( Reveal::Core::authorization_ptr auth, Reveal::Core::session_ptr& session );

  /// Validates whether an experiment is valid
  /// @param auth the authorization submitted by the client
  /// @param experiment the experiment to validate
  /// @return returns true if the experiment is valid OR false if the 
  ///         validation failed for any reason
  bool is_experiment_valid( Reveal::Core::authorization_ptr auth, Reveal::Core::experiment_ptr experiment_request, Reveal::Core::experiment_ptr& experiment_record );

  /// Encapsulates the operations to create a new session
  /// @param auth the authorization submitted by the client
  /// @param session the session created if the operation was successful
  /// @return true if the session was created and the session pointer is valid
  ///         OR false if the operation failed for any reason
  bool create_session( Reveal::Core::authorization_ptr auth, Reveal::Core::session_ptr& session );

  /// Encapsulates the operations to create a new experiment
  /// @param auth the authorization submitted by the client
  /// @param scenario the scenario for which to create the experiment
  /// @param experiment the input includes the experimental parameters requested
  ///        and the output includes modifications assigned by the service
  /// @return true if the experiment was created and the experiment pointer 
  ///         contains valid additional data generated by the service OR false 
  ///         if the operation failed for any reason
  bool create_experiment( Reveal::Core::authorization_ptr auth, Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr& experiment );
};

//-----------------------------------------------------------------------------
}  // namespace Server
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_SERVER_WORKER_H_
