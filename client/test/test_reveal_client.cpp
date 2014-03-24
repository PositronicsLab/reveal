#include <stdio.h>
#include <stdlib.h>

#include <Reveal/client.h>

int main( int argc, char* argv[] ) {
  Reveal::client_c client;

  if( !client.init() ) {
    printf( "ERROR: Client failed to initialize properly.\nExiting.\n" );
    client.terminate();
    exit(1);
  }

  client.go();
  client.terminate();

  return 0;
}
