#include <stdio.h>
#include <stdlib.h>

#include "reveal/core/authorization.h"
#include "reveal/core/user.h"
#include "reveal/core/system.h"

#include "client.h"

//-----------------------------------------------------------------------------

Reveal::Samples::client_ptr client;

/*
//-----------------------------------------------------------------------------
void test_identified_user( void ) {
  Reveal::Core::user_ptr user = Reveal::Core::user_ptr(new Reveal::Core::user_c() );
  user->id = "alice";

  Reveal::Core::authorization_ptr auth = Reveal::Core::authorization_ptr(new Reveal::Core::authorization_c() );
  auth->set_type( Reveal::Core::authorization_c::TYPE_IDENTIFIED );
  auth->set_user( user->id );

  Reveal::Client::client_c::error_e client_error;
  client_error = client.request_authorization( auth );
  if( client_error != Reveal::Client::client_c::ERROR_NONE ) {
    printf( "ERROR: client failed to gain identified authorization\n" );
  } else {
    printf( "SUCCESS: identified client gained authorization: session[%s]\n", auth->get_session().c_str() );
  }
}

//-----------------------------------------------------------------------------
void test_anonymous_user( void ) {

  Reveal::Core::authorization_ptr auth = Reveal::Core::authorization_ptr(new Reveal::Core::authorization_c() );
  auth->set_type( Reveal::Core::authorization_c::TYPE_ANONYMOUS );

  Reveal::Client::client_c::error_e client_error;
  client_error = client.request_authorization( auth );
  if( client_error != Reveal::Client::client_c::ERROR_NONE ) {
    printf( "ERROR: client failed to gain anonymous authorization\n" );
  } else {
    printf( "SUCCESS: anonymous client gained authorization: session[%s]\n", auth->get_session().c_str() );
  }
}
*/
//-----------------------------------------------------------------------------
int main( int argc, char* argv[] ) {
  boost::shared_ptr<Reveal::Core::system_c> system;
  system = boost::shared_ptr<Reveal::Core::system_c>( new Reveal::Core::system_c( Reveal::Core::system_c::CLIENT ) );

  if( !system->open() ) return false;

  std::string uri = system->samples_uri(); 

  system->close();

  client = Reveal::Samples::client_ptr( new Reveal::Samples::client_c( uri ) );

  if( !client->startup() ) {
    client->shutdown();
    exit( 1 );
  }

  if( !client->execute() ) {
    client->shutdown(); 
    exit( 2 );
  }

  client->shutdown();

  return 0;
}

//-----------------------------------------------------------------------------
