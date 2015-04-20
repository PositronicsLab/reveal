#ifndef _REVEAL_DB_SERVICE_H_
#define _REVEAL_DB_SERVICE_H_

#include <boost/shared_ptr.hpp>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------

class database_c;
typedef boost::shared_ptr<database_c> database_ptr;
//-----------------------------------------------------------------------------

class service_c;
typedef boost::shared_ptr<service_c> service_ptr;
//-----------------------------------------------------------------------------

class service_c {
public:
  virtual bool open( void ) = 0;
  virtual void close( void ) = 0;
};

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_SERVICE_H_
