#include <Reveal/db/importer.h>

#include <stdio.h>
#include <string>
#include <assert.h>

int main( int argc, char* argv[] ) {

  if(argc != 3) {
    printf( "Usage: ... <PATH-TO-IMPORT-DATA> <SCENARIO-FILE>\n" );
    return 1;
  }

  std::string path = argv[1];
  std::string filename = argv[2];

  Reveal::DB::importer_c importer;
  importer.read( path, filename );

  return 0;
}
