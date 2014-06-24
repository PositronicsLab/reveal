/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CLIENT_CLIENT_H_
#define _REVEAL_CLIENT_CLIENT_H_

//-----------------------------------------------------------------------------

#include <string.h>

#include <Reveal/connection.h>
#include <Reveal/pointers.h>

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
  error_e request_digest( Reveal::Core::digest_ptr& digest );
  error_e request_scenario( Reveal::Core::scenario_ptr& scenario );
  error_e request_trial( Reveal::Core::trial_ptr& trial );
  error_e submit_solution( Reveal::Core::solution_ptr& solution );

private:
  Reveal::Core::connection_c _connection;

  error_e request_reply( const std::string& request, std::string& reply );
};

//-----------------------------------------------------------------------------

} // namespace Client

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CLIENT_CLIENT_H_

