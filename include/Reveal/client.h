/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CLIENT_H_
#define _REVEAL_CLIENT_H_

//-----------------------------------------------------------------------------

#include <string.h>

#include <Reveal/connection.h>

#include "net.pb.h"

//-----------------------------------------------------------------------------

namespace Reveal {

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

private:
  connection_c _connection;
};

//-----------------------------------------------------------------------------

} // namespace reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CLIENT_H_

