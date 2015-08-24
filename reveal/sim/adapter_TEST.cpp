/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

#include "reveal/sim/adapter.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>

//-----------------------------------------------------------------------------
int main( int argc, char* argv[] ) {

  Reveal::Sim::adapter_c adapter;

  adapter.identify_sims();

  printf( "found %u installed sims\n", adapter.interfaces.size() );

  for( unsigned i = 0; i < adapter.interfaces.size(); i++ ) {
    printf( "  id[%s], name[%s], source_path[%s]\n", adapter.interfaces[i].id.c_str(), adapter.interfaces[i].name.c_str(), adapter.interfaces[i].source_path.c_str() );

    adapter.compile_interface( adapter.interfaces[i] );
  }

  

  return 0;
}
