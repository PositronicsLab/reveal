#include <stdio.h>
#include <stdlib.h>

#include <Reveal/database.h>

int main( int argc, char* argv[] ) {
  Reveal::DB::database_c db( "localhost" );

  db.open();

  mongo::BSONObjBuilder b;
  b.append( "id", "1" );
  b.append( "name", "test" );
  b.append( "trials", 2 );
  b.append( "uri", "http://www.gazebosim.org/" );
  b.append( "uri", "http://www.osrffoundation.org/" );

  mongo::BSONObj p = b.obj();
  
  db.insert( "scenario", p );  

  return 0;
}
