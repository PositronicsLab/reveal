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
namespace Mongo {
//-----------------------------------------------------------------------------

class mongo_c;
typedef boost::shared_ptr<mongo_c> mongo_ptr;
//-----------------------------------------------------------------------------

class mongo_c : public service_c {
public:
  /// default constructor
  mongo_c( void );

  /// destructor
  virtual ~mongo_c( void );

  /// implementation of service_c interface.  opens the mongo database.
  /// @return true if the database is able to be open or is already open
  virtual bool open( void );

  /// implementation of service_c interface.  closes the mongo database.
  virtual void close( void );

  /// query the number of records in a table
  /// @param table the table to query
  /// @return the count of all records in a table
  unsigned count( std::string table );

  /// query the number of records in a table with a limiting where clause
  /// @param table the table to query
  /// @param where the where clause to filter by
  /// @return the count of all records in a table matching the where criteria
  unsigned count( std::string table, mongo::BSONObj where );

  /// inserts data contained in a bson object into the mongo database
  /// @param table the table to insert into
  /// @param bson the bson data to insert into the specified table
  /// @return false if insertion succeeded OR false if failed for any reason
  bool insert( std::string table, mongo::BSONObj bson );

  /// fetches a set of documents from the database and returns a cursor to the 
  /// set on success
  /// @param cursor a valid cursor is returned if fetch succeeds
  /// @param table the table to fetch the data from
  /// @param query the query to execute describing the documents to fetch
  /// @return false if insertion succeeded OR false if failed for any reason
  bool fetch( std::auto_ptr<mongo::DBClientCursor>& cursor, std::string table, mongo::Query query );

  /// updates a set of documents in the database that match specified criteria
  /// @param table the table to update documents in
  /// @param query the query to execute describing the documents to update
  /// @param where the where clause to filter by
  /// @return false if update succeeded OR false if failed for any reason
  bool update( std::string table, mongo::BSONObj query, mongo::BSONObj where );

private:
  /// indicates whether the database connection is currently open
  bool                         _open;
  /// the host URL of the active mongo service
  std::string                  _host;
  /// the port the active mongo service is listening on
  unsigned                     _port;
  /// the name of the database hosted by the mongo service
  std::string                  _dbname;
  /// the persistent connection to the mongo database
  mongo::DBClientConnection    _connection;

public:
  /// eases the service interface by casting a generic database service to a 
  /// mongo service
  /// @param db the database to retrieve a service pointer from
  /// @return a cast service pointer as a mongo service
  static mongo_ptr service( database_ptr db );
};

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_MONGO_H_
