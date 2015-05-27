#include "Reveal/db/mongo/mongo.h"

#include <assert.h>

#include "Reveal/core/system.h"
#include "Reveal/db/database.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
mongo_c::mongo_c( void ) {
  _open = false;
}
//-----------------------------------------------------------------------------
mongo_c::~mongo_c( void ) {

}
//-----------------------------------------------------------------------------
bool mongo_c::open( void ) {
  if( _open ) return true;

  Reveal::Core::system_c system( Reveal::Core::system_c::DATABASE );
  if( !system.open() ) return false;

  _dbname = system.database_name();
  _host = system.database_host();
  _port = system.database_port();

  std::stringstream connection_str;
  connection_str << _host << ':' << _port;

  try {
    _connection.connect( connection_str.str() );
  } catch( const mongo::DBException &ex ) {
    std::cout << "ERROR: Failed to open connection to database host: " << ex.what() << std::endl;
    return false;
  }

  _open = true;
  return _open;
}

//-----------------------------------------------------------------------------
void mongo_c::close( void ) {

  if( !_open ) return;
  
/*
  if( _connection.isStillConnected() )
  
  mongo::BSONObj info;
  _connection.logout(  )
*/

  _open = false;
}
//*
//-----------------------------------------------------------------------------
mongo_ptr mongo_c::service( database_ptr db ) {
  mongo_ptr empty;

  // verify service and open
  if( !db->service() && !db->service()->open() ) return empty;

  // cast service to mongo
  return boost::dynamic_pointer_cast<mongo_c>( db->service() ); 
}
//*/
//-----------------------------------------------------------------------------
unsigned mongo_c::count( std::string table ) {
  assert( _open );

  std::string document = _dbname + "." + table;
  return _connection.count( document );
}
//-----------------------------------------------------------------------------
unsigned mongo_c::count( std::string table, mongo::BSONObj where ) {
  assert( _open );

  std::string document = _dbname + "." + table;
  return _connection.count( document, where );
}
//-----------------------------------------------------------------------------
bool mongo_c::insert( std::string table, mongo::BSONObj bson ) {
  if( !_open ) return false;

  std::string document = _dbname + "." + table;
  _connection.insert( document, bson );

  return true;
}

//-----------------------------------------------------------------------------
bool mongo_c::fetch( std::auto_ptr<mongo::DBClientCursor>& cursor, std::string table, mongo::Query query ) {
  if( !_open ) return false;

  std::string document = _dbname + "." + table; 
  cursor = _connection.query( document, query );

  return true;
}

//-----------------------------------------------------------------------------
bool mongo_c::update( const std::string table, mongo::BSONObj query, mongo::BSONObj where ) {
  if( !_open ) return false;

  std::string document = _dbname + "." + table;
  _connection.update( document, where, query );

  return true;
}

//-----------------------------------------------------------------------------
bool mongo_c::create_index( const std::string table, mongo::BSONObj keys, bool unique ) {
  if( !_open ) return false;

  std::string document = _dbname + "." + table;
  return _connection.ensureIndex( document, keys, unique );
}

//-----------------------------------------------------------------------------
bool mongo_c::drop_database( void ) {
  if( !_open ) return false;

  return _connection.dropDatabase( _dbname );
}

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

