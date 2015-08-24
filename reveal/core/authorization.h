/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

authorization.h defines the authorization_c class that encapsulates
and manages the authorization data passed through the reveal exchange.
-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_AUTHORIZATION_
#define _REVEAL_CORE_AUTHORIZATION_

//----------------------------------------------------------------------------

#include <string>
#include "reveal/core/pointers.h"

//----------------------------------------------------------------------------
namespace Reveal {
//----------------------------------------------------------------------------
namespace Core {
//----------------------------------------------------------------------------

class authorization_c {
public:

  /// The set of errors an authorization operation may return
  enum error_e {  
    ERROR_NONE = 0,          //< operation was successful
    ERROR_INVALID_SESSION,   //< operation failed due to an invalid session
    ERROR_INVALID_IDENTITY,  //< operation failed due to an invalid identity
  };

  /// The set of authorization types allowed
  enum type_e {
    TYPE_ANONYMOUS = 0, //< user is operating on anonymous authorization
    TYPE_IDENTIFIED,  //< user is operating on strictly identified authorization
    TYPE_SESSION,       //< user is operating on session based authorization
  };

  /// Default constructor
  authorization_c( void ) {}
  /// Destructor
  virtual ~authorization_c( void ) {}

  /// Sets the authorization type in this instance
  /// @param type one of the enumerated types allowed for authorization
  void set_type( type_e type ) { _type = type; }

  /// Gets the authorization type from this instance
  /// @return one of the enumerated types allowed for authorization
  type_e get_type( void ) { return _type; }

  /// Sets the authorization error in this instance
  /// @param error one of the enumerated errors allowed for authorization
  void set_error( error_e error ) { _error = error; }
  
  /// Gets the authorization error from this instance
  /// @return one of the enumerated errors allowed for authorization
  error_e get_error( void ) { return _error; }

  /// Sets the user identifier in this instance
  /// @param the user identifier to set in this instance
  void set_user( std::string user ) { _user = user; }

  /// Gets the user identifier from this instance
  /// @return the user identifier contained in this instance
  std::string get_user( void ) { return _user; }

  /// Sets the session identifier in this instance
  /// @param session the session identifier to set in this instance
  void set_session( std::string session ) { _session = session; }

  /// Gets the session identifier from this instance
  /// @return the session identifier in this instance
  std::string get_session( void ) { return _session; }

private:
  type_e _type;          //< the current type of this authorization
  error_e _error;        //< the current error of this authorization
  std::string _user;     //< the current user identifier in this authorization
  std::string _session;  //< the current session identifier in this authorzation
};

//----------------------------------------------------------------------------
} // namespace Core
//----------------------------------------------------------------------------
} // namespace Reveal
//----------------------------------------------------------------------------

#endif // _REVEAL_CORE_AUTHORIZATION_
