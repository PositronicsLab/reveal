/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

------------------------------------------------------------------------------*/

#ifndef _REVEAL_ANALYTICS_CLIENT_H_
#define _REVEAL_ANALYTICS_CLIENT_H_

//-----------------------------------------------------------------------------

#include <string.h>

#include "reveal/core/system.h"
#include "reveal/core/connection.h"
#include "reveal/core/pointers.h"
#include "reveal/core/simulator.h"

//#include <boost/enable_shared_from_this.hpp>
//#include <boost/shared_ptr.hpp>

#include "reveal/core/app.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Analytics {
//-----------------------------------------------------------------------------

class client_c;
typedef boost::shared_ptr<client_c> client_ptr;

//-----------------------------------------------------------------------------
class client_c : public Reveal::Core::app_c {
public:

  /// Default Constructor
  client_c( std::string analytics_server_uri );
  /// Destructor
  virtual ~client_c( void );

  virtual bool startup( void );
  virtual bool execute( void );
  virtual void shutdown( void );

  virtual std::string boilerplate_overview( void );
  virtual std::string boilerplate_warnings( void );

  bool login( void );

  bool request_authorization( Reveal::Core::authorization_ptr& auth );
  bool request_job( Reveal::Core::authorization_ptr& auth );

private:
  Reveal::Core::user_ptr _user;          //< the user logged into the server
  Reveal::Core::authorization_ptr _auth; //< the authorization granted by server
};

//-----------------------------------------------------------------------------
} // namespace Analytics
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_ANALYTICS_CLIENT_H_

