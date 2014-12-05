/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

client.h defines the client_c API.  To implement a Reveal client, the client_c
class is the only required interface into the system. 
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CLIENT_CLIENT_H_
#define _REVEAL_CLIENT_CLIENT_H_

//-----------------------------------------------------------------------------

#include <string.h>

#include "Reveal/core/connection.h"
#include "Reveal/core/pointers.h"

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Client {

//-----------------------------------------------------------------------------

#define REVEAL_SERVER_URI "localhost"

//-----------------------------------------------------------------------------

class client_c {
public:
  enum error_e {
    ERROR_NONE = 0,
    ERROR_READ,                      //< Error attempting to read
    ERROR_WRITE,                     //< Error attempting to write
    ERROR_CONNECTION,                //< Error in the connection
    ERROR_EXCHANGE_RESPONSE,         //< Error in the response
    ERROR_EXCHANGE_BUILD,            //< Unable to build message
    ERROR_EXCHANGE_PARSE,            //< Unable to parse
    ERROR_INVALID_SCENARIO_REQUEST,  //< Requested scenario is invalid
    ERROR_INVALID_TRIAL_REQUEST,     //< Requested trial is invalid
    ERROR_INVALID_SOLUTION           //< Attempted solution sent was malformed
  };

  client_c( void );
  virtual ~client_c( void );

  bool init( void );
  void terminate( void );
  bool connect( void );

  // API
  error_e request_authorization( Reveal::Core::authorization_ptr& auth );
  error_e request_digest( Reveal::Core::authorization_ptr& auth, Reveal::Core::digest_ptr& digest );
  error_e request_experiment( Reveal::Core::authorization_ptr& auth, Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr& experiment );
  error_e request_trial( Reveal::Core::authorization_ptr& auth, Reveal::Core::experiment_ptr experiment, Reveal::Core::trial_ptr& trial );
  error_e submit_solution( Reveal::Core::authorization_ptr& auth, Reveal::Core::experiment_ptr experiment, Reveal::Core::solution_ptr& solution );

  Reveal::Core::connection_c _connection;

private:
  error_e request_reply( const std::string& request, std::string& reply );
};

//-----------------------------------------------------------------------------

} // namespace Client

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CLIENT_CLIENT_H_

