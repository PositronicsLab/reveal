#include <Reveal/db/importer.h>

#include <stdio.h>
#include <string>
#include <assert.h>

int main( int argc, char* argv[] ) {

  Reveal::DB::importer_c importer;

  importer.read( "/home/james/osrf/Reveal/packages/industrial_arm/shared/build/industrial_arm.scenario" );

  return 0;
}
