/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

session.h defines the session_c data structure that maintains session
information for a given user accessing Reveal.  Provides a layer of security
and a layer of multiuse in the authorization system such that a given user
can maintain multiple sessions and the user information is not shared broadly
in exchanges with the server
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_SESSION_H_
#define _REVEAL_CORE_SESSION_H_

//-----------------------------------------------------------------------------

#include <stdio.h>
#include <string>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Core {

//-----------------------------------------------------------------------------

class session_c {
public:

  enum type_e {
    ANONYMOUS = 0,
    IDENTIFIED
  };

  session_c( void ) { }
  virtual ~session_c( void ) { }

  type_e                 user_type;
  std::string            user_id;
  std::string            session_id;

  void print( void ) {
    printf( "session[%s]", session_id.c_str() );
    if( user_type == ANONYMOUS ) {
      printf( ", user_type[ANONYMOUS]\n" );
    } else {
      printf( ", user_type[IDENTIFIED], user_id[%s]\n", user_id.c_str() );
    }
  }
};

//-----------------------------------------------------------------------------

} // namespace Core

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_SESSION_H_
