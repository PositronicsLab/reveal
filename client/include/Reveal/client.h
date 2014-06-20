/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CLIENT_CLIENT_H_
#define _REVEAL_CLIENT_CLIENT_H_

//-----------------------------------------------------------------------------

#include <string.h>

#include <Reveal/connection.h>
#include <Reveal/pointers.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Client {

//-----------------------------------------------------------------------------

#define REVEAL_SERVER_URI "localhost"

//-----------------------------------------------------------------------------

class client_c {
public:
  client_c( void );
  virtual ~client_c( void );

  bool init( void );
  void go( void );
  void terminate( void );

  bool connect( void );

  // API
  bool request_digest( Reveal::Core::digest_ptr& digest );
  bool request_scenario( Reveal::Core::scenario_ptr& scenario );
  bool request_trial( Reveal::Core::trial_ptr& trial );
  bool submit_solution( Reveal::Core::solution_ptr& solution );

private:
  Reveal::Core::connection_c _connection;

  bool request_reply( const std::string& request, std::string& reply );
};

//-----------------------------------------------------------------------------

} // namespace Client

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CLIENT_CLIENT_H_

