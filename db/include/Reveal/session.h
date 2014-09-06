/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

session.h defines the session_c data used to map users to connection instances
to the server.  It provides a layer of security and the intent is to allow
users to create multiple sessions such that they can rerun the same senarios
multiple times.
------------------------------------------------------------------------------*/

#ifndef _REVEAL_DB_SESSION_H_
#define _REVEAL_DB_SESSION_H_

//-----------------------------------------------------------------------------

#include <boost/shared_ptr.hpp>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------

class session_c;
typedef boost::shared_ptr< Reveal::DB::session_c > session_ptr;

class session_c {
public:
  session_c( void ) { }
  virtual ~session_c( void ) { }

  int session_id;
  int user_id;
  
  // start time
  // closed?
};

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_SESSION_H_
