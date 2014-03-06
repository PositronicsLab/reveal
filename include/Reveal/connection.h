/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CONNECTION_H_
#define _REVEAL_CONNECTION_H_

//-----------------------------------------------------------------------------

#include <zmq.h>
#include <string>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

#define PORT            20700
#define SND_BUFFER_SZ   64
#define RCV_BUFFER_SZ   SND_BUFFER_SZ + 1 

//-----------------------------------------------------------------------------

class Connection {
public:
  Connection( void );
  Connection( const unsigned& port );
  Connection( const std::string& host, const unsigned& port );
  virtual ~Connection( void );

  enum Role {
    UNDEFINED = 0,
    SERVER,
    CLIENT
  };

  bool Open( void );
  void Close( void );
  bool Read( std::string& msg );
  bool Write( const std::string& msg );

private:
  Role _role;
  std::string _host;
  unsigned _port;

  bool _open;

  void* _context;
  void* _socket;

  std::string ConnectionString( void );
};

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CONNECTION_H_
