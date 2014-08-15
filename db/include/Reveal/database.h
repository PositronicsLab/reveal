#ifndef _REVEAL_DB_DATABASE_
#define _REVEAL_DB_DATABASE_

//-----------------------------------------------------------------------------
#include <cstdlib>
#include <iostream>
#include <memory>

#include <mongo/client/dbclient.h>
//#include <string>

#include <Reveal/pointers.h>
#include <Reveal/analysis.h>
#include <Reveal/analyzer.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace DB {

//-----------------------------------------------------------------------------

class database_c {
private:
  std::string                  _host;
  unsigned                     _port;
  bool                         _open;
  std::string                  _dbname;

//  mongo::ConnectionString      _connection_string;
  mongo::DBClientConnection    _connection;

public:
  enum error_e {
    ERROR_NONE,
    ERROR_EMPTYSET
  };

  database_c( void );
  database_c( const std::string& host );
  virtual ~database_c( void );

  bool open( void );
  void close( void );
/*
  error_e update( const std::string query );
  error_e fetch( const std::string query, table_c& data );
  error_e insert( const std::string query );
*/

private:
  bool insert( const std::string& table, const mongo::BSONObj& query );
  bool fetch( std::auto_ptr<mongo::DBClientCursor>& cursor, const std::string& table, mongo::Query query );

public:
  error_e query( Reveal::Core::digest_ptr& digest );

  error_e insert( Reveal::Core::scenario_ptr scenario );
  error_e query( Reveal::Core::scenario_ptr& scenario, const std::string& name );
  error_e query( Reveal::Core::scenario_ptr& scenario, int scenario_id );

  error_e insert( Reveal::Core::trial_ptr trial );
  error_e query( Reveal::Core::trial_ptr& trial, int scenario_id, int trial_id );

  error_e insert( Reveal::Core::solution_ptr solution );
  error_e query( Reveal::Core::solution_ptr& solution, Reveal::Core::solution_c::type_e type, int scenario_id, int trial_id );

  error_e insert( Reveal::Core::analyzer_ptr analyzer );
  error_e query( Reveal::Core::analyzer_ptr& analyzer, int scenario_id );
  error_e query( Reveal::Core::solution_set_ptr& solution_set, int scenario_id, int session_id );
  error_e insert( Reveal::Core::analysis_ptr analysis );
};

//-----------------------------------------------------------------------------

} // namespace DB

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_DATABASE_
