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

  void scenario( const std::string& name );
  void trial( const std::string& scenario );
  void trial( const std::string& scenario, const unsigned& index );
  void solution( const std::string& scenario );
  void solution( const std::string& scenario, const unsigned& index );
  void model_solution( const std::string& scenario );
  void model_solution( const std::string& scenario, const unsigned& index );

};

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_QUERY_H_
