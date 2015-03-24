#include <Reveal/core/xml.h>

#include <stdio.h>
#include <string>
#include <assert.h>

int main( int argc, char* argv[] ) {

  assert( argc == 3 );

  std::string infile = argv[1];
  std::string outfile = argv[2];

  Reveal::Core::xml_c xml_reader;

  xml_reader.read( infile );

  Reveal::Core::xml_c xml_writer;

  xml_writer.root( xml_reader.root() );
  xml_writer.write( outfile );
}
