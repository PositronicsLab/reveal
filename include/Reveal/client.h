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

class Client {
public:
  Client( void );
  virtual ~Client( void );

  bool Init( void );
  void Go( void );
  void Terminate( void );

  bool Connect( void );

private:
  Connection _connection;
};

//-----------------------------------------------------------------------------

} // namespace reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CLIENT_H_

