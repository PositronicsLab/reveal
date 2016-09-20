#include <stdio.h>
#include "reveal/core/connection.h"
#include <boost/shared_ptr.hpp>
#include "config.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------
typedef boost::shared_ptr<connection_c> connection_ptr;
//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

int exec_ipc_mode( void ) {
  Reveal::Core::connection_ptr connection;  //< a connection to the simulator

  // Note: before creating the following new pointer, generate a random string
  // as the ipc connection identifier and replace the string "insert-rando-id"
  // in the call to the constructor.  You can then store connection pointers
  // in a map so that you are guaranteed to talk to the expected worker.  Your 
  // connection-id generator should generate a string with something like the
  // following syntax <appname>-XXXXXX, e.g. pacer-145681
  std::string port_id = "insert-rando-id";

  connection = Reveal::Core::connection_ptr( new Reveal::Core::connection_c(Reveal::Core::connection_c::IPC_SERVER, port_id ) );

  if( connection->open() != Reveal::Core::connection_c::ERROR_NONE ) {
    printf( "planner failed to open server ipc connection\nExiting\n" );
    return 1;
  }

  while(true) {
    std::string request, reply;
    request = "hello";
    printf( "sending request: %s\n", request.c_str() );
    if( connection->write( request ) != Reveal::Core::connection_c::ERROR_NONE) {
      printf( "ERROR: connection failed to write request\nExiting\n" );
      return 1;
    }

    if( connection->read( reply ) != Reveal::Core::connection_c::ERROR_NONE ) {
      printf( "ERROR: connection failed to read reply\nExiting\n" );
      return 1;
    }
    printf( "received reply: %s\n", reply.c_str() );
  }
  return 0;
}

int exec_tcp_mode( void ) {
  Reveal::Core::connection_ptr connection;  //< a connection to the simulator

  unsigned port = 50000;

  connection = Reveal::Core::connection_ptr( new Reveal::Core::connection_c(Reveal::Core::connection_c::TCP_SERVER, port ) );
   
  if( connection->open() != Reveal::Core::connection_c::ERROR_NONE ) {
    printf( "planner failed to open server tcp connection\nExiting\n" );
    return 1;
  }

  while(true) {
    std::string request, reply;
    printf( "reading request: %s\n", request.c_str() );
    if( connection->read( request ) != Reveal::Core::connection_c::ERROR_NONE) {
      printf( "ERROR: connection failed to read request\nExiting\n" );
      return 1;
    }
    printf( "received request: %s\n", request.c_str() );
    reply = "good day";
    printf( "sending reply: %s\n", reply.c_str() ); 

    if( connection->write( reply ) != Reveal::Core::connection_c::ERROR_NONE ) {
      printf( "ERROR: connection failed to write reply\nExiting\n" );
      return 1;
    }
  }
  return 0;
}

int main( void ) { 
#ifdef IPC_MODE
  return exec_ipc_mode();
#else //TCP_MODE
  return exec_tcp_mode();
#endif
  return 0;
}
