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
  /// Set of error codes that are used as return values in the client interface
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

  /// Default Constructor
  client_c( void );
  /// Destructor
  virtual ~client_c( void );

  /// Encapsulates the whole of the client operation as far as external API is 
  /// concerned
  /// @return true if execution succeeded OR false is any error occurred
  bool execute( void );

  /// Gets a pointer to the this class's instance 
  /// @return a pointer to this client instance
  client_ptr ptr( void );

private:
  Reveal::Core::connection_c _connection;//< the reveal client/server connection
  Reveal::Core::simulator_ptr _simulator;//< the simulator used by the client
  Reveal::Core::user_ptr _user;          //< the user logged into the server
  Reveal::Core::authorization_ptr _auth; //< the authorization granted by server
  boost::shared_ptr<Reveal::Core::system_c> _system;  //< the reveal system
  std::string _working_directory;        //< the client's working directory

  /// Encapsulates the initialization steps required to start a client
  /// @return true if the client successfully initialed OR false if any error
  ///         occurred
  bool init( void );

  /// Encapsulates the shutdown and cleanup of all resources when the client 
  /// is to shutdown
  void terminate( void );

  /// Opens the transport layer and connects the client to the Reveal server
  /// @return true if a connection was successfully made to the server OR false
  ///         if the operation failed for any reason
  bool connect( void );

  /// Manages the request-reply exchanges between the client and the server
  /// @param request the serialized request message to be sent to the server
  /// @param reply the serialized reply returned by the server
  /// @return returns ERROR_NONE if the request was properly serviced by the
  ///         connection and a reply was properly received in response OR 
  ///         another enumerated error value if the exchange failed
  error_e request_reply( const std::string& request, std::string& reply );

  /// Encapsulates the login sequence necessary to gain authorization to the 
  /// reveal service
  /// @return true if authorization was granted OR false if authorization was
  /// rejected
  bool login( void );

  /// Encapsulates the authorization request exchange between client and server
  /// @param auth the authorization data returned by the server if a request
  ///        has been successfully serviced
  /// @return returns ERROR_NONE if the authorization request succeeded and
  ///         signals a valid authorization pointer has been set but does not
  ///         necessarily imply the authorization is granted OR another 
  ///         enumerated error value if the request failed
  error_e request_authorization( Reveal::Core::authorization_ptr& auth );

  /// Encapsulates the digest request exchange between client and server
  /// @param auth the authorization necessary to make requests to the server
  /// @param digest the digest data returned by the server if a request has 
  ///        been successfully serviced
  /// @return returns ERROR_NONE if the digest request succeeded and signals
  ///         a valid digest pointer has been set OR another enumerated error
  ///         value if the request failed
  error_e request_digest( Reveal::Core::authorization_ptr& auth, Reveal::Core::digest_ptr& digest );

  /// Encapsulates the experiment request exchange between client and server
  /// @param auth the authorization necessary to make requests to the server
  /// @param scenario the scenario for which the experiment is being requested
  /// @param experiment the experiment that is returned if the request has been
  ///        successfully been serviced
  /// @return returns ERROR_NONE if the experiment request succeeded and signals
  ///         a valid experiment pointer has been set OR another enumerated 
  ///         error value if the request failed
  error_e request_experiment( Reveal::Core::authorization_ptr& auth, Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr& experiment );

  /// Encapsulates the trial request exchange between client and server
  /// @param auth the authorization necessary to make requests to the server
  /// @param experiment the experiment for which the trial is being requested
  /// @param trial the trial that is returned if the request has successfully 
  ///        been serviced
  /// @return true if the trial request succeeded and signals a valid trial 
  ///         pointer has been set OR false if the request failed for any reason
  bool request_trial( Reveal::Core::authorization_ptr& auth, Reveal::Core::experiment_ptr experiment, Reveal::Core::trial_ptr& trial );

  /// Encapsulates the solution submission exchange between client and server
  /// @param auth the authorization necessary to make requests to the server
  /// @param experiment the experiment for which the solution is being submitted
  /// @param solution the solution that is being submitted to the server
  /// @return true if the trial request succeeded and signals a valid trial 
  ///         pointer has been set OR false if the request failed for any reason
  bool submit_solution( Reveal::Core::authorization_ptr& auth, Reveal::Core::experiment_ptr experiment, Reveal::Core::solution_ptr& solution );

  /// Encapsulates user interface elements required to select a scenario from 
  /// the digest
  /// @param digest the digest composed of the set of scenarios to select from
  /// @return the index of one element in the digest that was selected by the 
  ///         user
  unsigned ui_select_scenario( Reveal::Core::digest_ptr digest );

  /// Prompt interface for requesting the user to set the time step for the 
  /// experiment provided
  /// @param scenario the scenario for which the experiment is designed
  /// @param experiment the experiment to which to assign the time step as set 
  ///        by the user
  void prompt_time_step( Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr experiment );

  /// Prompt interface for requesting the user to set the intermediate trials to
  /// ignore for the experiment provided
  /// @param scenario the scenario for which the experiment is designed
  /// @param experiment the experiment to which to assign the intermediate 
  ///        trials to ignore as set by the user
  void prompt_trials_to_ignore( Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr experiment );
};

//-----------------------------------------------------------------------------
} // namespace Client
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CLIENT_CLIENT_H_

