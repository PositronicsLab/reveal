#include <stdio.h>
#include <stdlib.h>

#include "reveal/core/authorization.h"
#include "reveal/core/user.h"
#include "reveal/core/system.h"

#include "client.h"

//-----------------------------------------------------------------------------

Reveal::Analytics::client_ptr client;

//-----------------------------------------------------------------------------
int main( int argc, char* argv[] ) {
  boost::shared_ptr<Reveal::Core::system_c> system;
  system = boost::shared_ptr<Reveal::Core::system_c>( new Reveal::Core::system_c( Reveal::Core::system_c::CLIENT ) );

  if( !system->open() ) return false;

  std::string uri = system->analytics_uri(); 
  //printf( "analytics_uri[%s]\n", uri.c_str() );

  system->close();

  client = Reveal::Analytics::client_ptr( new Reveal::Analytics::client_c( uri ) );

  bool result;
  result = client->startup();
  if( !result ) exit( 1 );
  result = client->execute( );
  if( !result ) exit( 2 );
  client->shutdown();

  return 0;
}

//-----------------------------------------------------------------------------
