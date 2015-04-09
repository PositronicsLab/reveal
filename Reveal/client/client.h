/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

client.h defines the client_c API.  To implement a Reveal client, the client_c
class is the only required interface into the system. 
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CLIENT_CLIENT_H_
#define _REVEAL_CLIENT_CLIENT_H_

//-----------------------------------------------------------------------------

#include <string.h>

#include "Reveal/core/system.h"
#include "Reveal/core/connection.h"
#include "Reveal/core/pointers.h"
#include "Reveal/core/simulator.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Client {
//-----------------------------------------------------------------------------

class client_c;
typedef boost::shared_ptr<client_c> client_ptr;

//-----------------------------------------------------------------------------
class client_c : public boost::enable_shared_from_this<client_c> {
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

  //bool execute( experiment_f );
  bool execute( void );

  client_ptr ptr( void );

  Reveal::Core::simulator_ptr simulator;

private:
  Reveal::Core::user_ptr _user;
  Reveal::Core::authorization_ptr _auth;

  boost::shared_ptr<Reveal::Core::system_c> _system;

  std::string _working_directory;

public:

  bool login( void );

  // API
  error_e request_authorization( Reveal::Core::authorization_ptr& auth );
  error_e request_digest( Reveal::Core::authorization_ptr& auth, Reveal::Core::digest_ptr& digest );
  unsigned ui_select_scenario( Reveal::Core::digest_ptr digest );

  error_e request_experiment( Reveal::Core::authorization_ptr& auth, Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr& experiment );

  bool request_trial( Reveal::Core::authorization_ptr& auth, Reveal::Core::experiment_ptr experiment, Reveal::Core::trial_ptr& trial );
  bool submit_solution( Reveal::Core::authorization_ptr& auth, Reveal::Core::experiment_ptr experiment, Reveal::Core::solution_ptr& solution );

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

