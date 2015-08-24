/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_APP_H_
#define _REVEAL_CORE_APP_H_

//-----------------------------------------------------------------------------

#include <string.h>

#include "reveal/core/system.h"
#include "reveal/core/connection.h"
#include "reveal/core/pointers.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class app_c;
typedef boost::shared_ptr<app_c> app_ptr;

//-----------------------------------------------------------------------------
class app_c : public boost::enable_shared_from_this<app_c> {
protected:
  std::string _uri;
  Reveal::Core::connection_c _connection;
  boost::shared_ptr<Reveal::Core::system_c> _system;
  std::string _working_directory;

  std::string _id;
  std::string _title;
public:

  app_c( std::string name, std::string title, std::string uri );
  virtual ~app_c( void );
  app_ptr ptr( void );

  virtual bool startup( void );
  virtual bool execute( void );
  virtual void shutdown( void );

protected:
  bool connect( void );
  bool request_reply( const std::string& request, std::string& reply );

  virtual std::string boilerplate_salutations( void );
  virtual std::string boilerplate_preamble( void );
  virtual std::string boilerplate_overview( void );
  virtual std::string boilerplate_warnings( void );
  
  void print_welcome_screen( void );
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_APP_H_

