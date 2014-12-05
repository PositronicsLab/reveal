#include <stdio.h>
#include <stdlib.h>

#include "Reveal/server/server.h"

//-----------------------------------------------------------------------------

int main( int argc, char* argv[] ) {
  Reveal::Server::server_c server;

  if( !server.init() ) {
    printf( "ERROR: Server failed to initialize properly.\nExiting.\n" );
    server.terminate();
    exit(1);
  }

  server.run();
  server.terminate();

  return 0;
}

//-----------------------------------------------------------------------------
