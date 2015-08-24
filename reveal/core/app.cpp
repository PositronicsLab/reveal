#include "reveal/core/app.h"

#include <stdio.h>

#include "reveal/core/console.h"
#include "reveal/core/authorization.h"
#include "reveal/core/user.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------
app_c::app_c( std::string id, std::string title, std::string uri ) : 
  _id( id ), 
  _title( title ), 
  _uri( uri ) 
{
  _system = boost::shared_ptr<Reveal::Core::system_c>( new Reveal::Core::system_c( Reveal::Core::system_c::CLIENT ) );
}

//-----------------------------------------------------------------------------
app_c::~app_c( void ) {
  _connection.close();
  _system->close();

}

//-----------------------------------------------------------------------------
app_ptr app_c::ptr( void ) {
  return shared_from_this();
}

//-----------------------------------------------------------------------------
bool app_c::startup( void ) {
  std::string salutations = boilerplate_salutations();
  std::string preamble = boilerplate_preamble();
  std::string overview = boilerplate_overview();
  std::string warnings = boilerplate_warnings();
  if( salutations != "" ) printf( "%s\n\n", salutations.c_str() );
  if( preamble != "" ) printf( "%s\n\n", preamble.c_str() );
  if( overview != "" ) printf( "%s\n\n", overview.c_str() );
  if( warnings != "" ) printf( "%s\n\n", warnings.c_str() );

  if( !connect() ) return false;
  if( !_system->open() ) return false;
 
  // TODO: Custom implementations here  

  return true;
}

//-----------------------------------------------------------------------------
bool app_c::execute( void ) {

  // TODO: Custom implementations here  

  return true;
}

//-----------------------------------------------------------------------------
void app_c::shutdown( void ) {

  // TODO: Custom implementations here  

}

//-----------------------------------------------------------------------------
bool app_c::connect( void ) {
  printf( "Connecting to server...\n" );

  std::string protocol, host;
  unsigned port;
  if( !split_uri( _uri, protocol, host, port ) ) {
    printf( "Failed to identify server\n" );
    return false;
  }

  //printf( "Connecting to host[%s] port[%u]\n", host.c_str(), port );

  _connection = Reveal::Core::connection_c( host, port );
  if( _connection.open() != Reveal::Core::connection_c::ERROR_NONE ) {
    return false;
  }

  printf( "Connected\n" );
  return true;
}

//-----------------------------------------------------------------------------
bool app_c::request_reply( const std::string& request, std::string& reply ) {
  //assert( _connection.open() ); cannot do this assertion
  // send the request message to the server
  if( _connection.write( request ) != Reveal::Core::connection_c::ERROR_NONE ) {
    // write failed at connection
    return false;
  }
  // block waiting for a server response
  if( _connection.read( reply ) != Reveal::Core::connection_c::ERROR_NONE ) {
    // read failed at connection
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
std::string app_c::boilerplate_salutations( void ) {
  return "Welcome to the " + _title + ".";
}

//-----------------------------------------------------------------------------
std::string app_c::boilerplate_preamble( void ) {
  return "Important legal information is printed in this statement.";
}

//-----------------------------------------------------------------------------
std::string app_c::boilerplate_overview( void ) {
  return "API NOTE:The text 'overview' should be implemented!";
}

//-----------------------------------------------------------------------------
std::string app_c::boilerplate_warnings( void ) {
  return "API NOTE:The text 'warnings' should be implemented!";
}

//-----------------------------------------------------------------------------
void app_c::print_welcome_screen( void ) {
  std::string salutations = boilerplate_salutations();
  std::string preamble = boilerplate_preamble();
  std::string overview = boilerplate_overview();
  std::string warnings = boilerplate_warnings();

  char dashes[32], dbldashes[32];
  sprintf( dashes, "%.*s", 20, "--------------------------------");
  sprintf( dbldashes, "%.*s", 20, "================================");

  if( salutations != "" ) {
    Reveal::Core::console_c::printline( dashes );
    Reveal::Core::console_c::printline( salutations );
  }
  if( preamble != "" ) {
    Reveal::Core::console_c::printline( dashes );
    Reveal::Core::console_c::printline( preamble );
  }
  if( overview != "" ) {
    Reveal::Core::console_c::printline( dashes );
    Reveal::Core::console_c::printline( overview );
  }
  if( warnings != "" ) { 
    Reveal::Core::console_c::printline( dashes );
    Reveal::Core::console_c::printline( warnings );
  }
  Reveal::Core::console_c::printline( dbldashes );
}

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
