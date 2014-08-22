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
