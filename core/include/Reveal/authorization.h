/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

authorization.h defines the authorization_c class that encapsulates
and manages the authorization data passed through the reveal exchange.
-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_AUTHORIZATION_
#define _REVEAL_CORE_AUTHORIZATION_

//----------------------------------------------------------------------------

#include <string>
#include <Reveal/pointers.h>

//----------------------------------------------------------------------------

namespace Reveal {

//----------------------------------------------------------------------------

namespace Core {

//----------------------------------------------------------------------------

class authorization_c {
public:

  enum error_e {
    ERROR_NONE = 0,
    ERROR_INVALID_SESSION,
    ERROR_INVALID_IDENTITY,
  };

  enum type_e {
    TYPE_ANONYMOUS = 0,
    TYPE_IDENTIFIED,
    TYPE_SESSION,
  };

  authorization_c( void ) {}
  virtual ~authorization_c( void ) {}

  void set_type( type_e type ) { _type = type; }
  type_e get_type( void ) { return _type; }

  void set_error( error_e error ) { _error = error; }
  error_e get_error( void ) { return _error; }

  // TODO : Refactor to use the user class
  void set_user( std::string user ) { _user = user; }
  std::string get_user( void ) { return _user; }

  // TODO : Refactor to use the session class
  void set_session( std::string session ) { _session = session; }
  std::string get_session( void ) { return _session; }

private:
  type_e _type;
  error_e _error;

  // credentials
  std::string _user;

  // session
  std::string _session;
};

//----------------------------------------------------------------------------

} // namespace Core

//----------------------------------------------------------------------------

} // namespace Reveal

//----------------------------------------------------------------------------

#endif // _REVEAL_CORE_AUTHORIZATION_
