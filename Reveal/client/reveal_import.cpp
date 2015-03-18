#include "Reveal/client/importer.h"

int main( int argc, char* argv[] ) {
  Reveal::Client::importer_c importer; 

  importer.open();
  importer.close();
}

