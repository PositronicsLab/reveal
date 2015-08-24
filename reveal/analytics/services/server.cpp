#include <stdio.h>
#include <stdlib.h>

#include "service.h"

//-----------------------------------------------------------------------------

int main( int argc, char* argv[] ) {
  Reveal::Analytics::service_c service;

  if( !service.init() ) {
    printf( "ERROR: Service failed to initialize properly.\nExiting.\n" );
    service.terminate();
    exit(1);
  }

  service.run();
  service.terminate();

  return 0;
}

//-----------------------------------------------------------------------------
