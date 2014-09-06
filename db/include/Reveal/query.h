/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

query.h defines the query_c adapter that encapsulates the mongo query interface
A valid query into the mongo database should be defined through this interface
to prevent the unauthorized access of the loosely structured database
------------------------------------------------------------------------------*/

#ifndef _REVEAL_DB_QUERY_H_
#define _REVEAL_DB_QUERY_H_

//-----------------------------------------------------------------------------
#include <mongo/client/dbclient.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------

class query_c {
private:
  mongo::Query _query;

public:
  query_c( void );
  virtual ~query_c( void );

  mongo::Query operator()( void );
  void operator()( mongo::Query query );

  void digest( void );
  void scenario( int scenario_id );
  void scenario( const std::string& name );
  //void trial( int scenario_id );
  void trial( int scenario_id, int trial_id );
  //void solution( int scenario_id );
  void solution( int scenario_id, int trial_id );
  void model_solution( int scenario_id, int trial_id );

  void analyzer( int scenario_id );
  void solution_set( int scenario_id, int session_id );
};

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_QUERY_H_
