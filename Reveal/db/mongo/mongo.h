#ifndef _REVEAL_DB_MONGO_H_
#define _REVEAL_DB_MONGO_H_

#include <boost/shared_ptr.hpp>

#include "Reveal/db/service.h"

#include <mongo/client/dbclient.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------

class mongo_c;
typedef boost::shared_ptr<mongo_c> mongo_ptr;
//-----------------------------------------------------------------------------

class mongo_c : public service_c {
public:
  mongo_c( void );
  virtual ~mongo_c( void );

  virtual bool open( void );
  virtual void close( void );

  unsigned count( std::string table );
  unsigned count( std::string table, mongo::BSONObj where );
  bool insert( std::string table, mongo::BSONObj bson );
  bool fetch( std::auto_ptr<mongo::DBClientCursor>& cursor, std::string table, mongo::Query query );
  bool update( std::string table, mongo::BSONObj query, mongo::BSONObj where );

private:
  bool                         _open;
  std::string                  _host;
  unsigned                     _port;
  std::string                  _dbname;
  mongo::DBClientConnection    _connection;

public:
  static mongo_ptr service( database_ptr db );
};

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_MONGO_H_
