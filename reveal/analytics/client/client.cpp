#include "client.h"

#include <boost/bind.hpp>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <limits>
#include <math.h>

#include "reveal/core/console.h"
#include "reveal/core/error.h"

#include "reveal/core/authorization.h"
#include "reveal/core/user.h"

#include "reveal/core/system.h"
#include "reveal/core/pointers.h"
#include "reveal/core/digest.h"
#include "reveal/core/experiment.h"
#include "reveal/core/scenario.h"
#include "reveal/core/trial.h"
#include "reveal/core/solution.h"

#include "reveal/analytics/exchange.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Analytics {
//-----------------------------------------------------------------------------
client_c::client_c( std::string uri ) : app_c( "reveal_analytics", "Reveal Analytics Client", uri ) {

}

//-----------------------------------------------------------------------------
client_c::~client_c( void ) {

}

//-----------------------------------------------------------------------------
bool client_c::startup( void ) {
  print_welcome_screen();
  if( !connect() ) return false;
  if( !_system->open() ) return false;

  // TODO: Custom implementations here  
  Reveal::Analytics::exchange_c exchange;
  exchange.open();

  if( !get_temp_directory( _working_directory ) ) {
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------- 
bool client_c::execute( void ) {

  if( !login() ) { 
    printf( "Failed to login.\nExiting.\n" );
    shutdown();
    return false;
  }

  while( true ) {

  }

  return true;
}

//-----------------------------------------------------------------------------
void client_c::shutdown( void ) {

  // TODO: Custom implementations here  
  // prune and remove temporary working directory
  remove_directory( _working_directory );

  Reveal::Analytics::exchange_c exchange;
  exchange.close();
}

//-----------------------------------------------------------------------------
std::string client_c::boilerplate_overview( void ) {
  std::stringstream ss;
  ss << "This client application allocates local computing resources for " << std::endl;
  ss << "analyzing robotics simulation data.  This application allocates a " << std::endl; 
  ss << "portion of the local processor to computing Reveal Analytics.  You " << std::endl;
  ss << "must provide one of your own local threads to ensure analytics is " << std::endl; 
  ss << "processed for your own experiments.  As long as this application " << std::endl;
  ss << "remains running, this system will provide resources for the service.";

  return ss.str();
}

//-----------------------------------------------------------------------------
std::string client_c::boilerplate_warnings( void ) {
  return "";
}

//-----------------------------------------------------------------------------
bool client_c::request_authorization( Reveal::Core::authorization_ptr& auth ) {
  std::string request;
  std::string reply;

  Reveal::Analytics::exchange_c client_exchange;
  Reveal::Analytics::exchange_c server_exchange;

  // create a authorization request
  client_exchange.set_origin( Reveal::Analytics::exchange_c::ORIGIN_WORKER );
  client_exchange.set_type( Reveal::Analytics::exchange_c::TYPE_HANDSHAKE );
  //client_exchange.set_error( Reveal::Analytics::exchange_c::ERROR_NONE );

  client_exchange.set_authorization( auth );

  // build the request message
  if( !client_exchange.build( request ) ) return false;

  // send the request message and wait for reply message
  if( !request_reply(request, reply) ) return false;

  // parse the reply message
  if( !server_exchange.parse( reply ) ) return false;
/*
  Reveal::Analytics::exchange_c::error_e transport_error = server_exchange.get_error();
  if( transport_error != Reveal::Analytics::exchange_c::ERROR_NONE ) {
    // The server sent a general error.  Suggest retrying the request.
    return false;
    // TODO : extend validation and error handling for any new cases that emerge
  }
*/
  // extract the authorization
  auth = server_exchange.get_authorization();
 
  return true;
}

//-----------------------------------------------------------------------------
bool client_c::request_job( Reveal::Core::authorization_ptr& auth ) {
  std::string request;
  std::string reply;

  Reveal::Analytics::exchange_c client_exchange;
  Reveal::Analytics::exchange_c server_exchange;

  // create a digest request
  client_exchange.set_origin( Reveal::Analytics::exchange_c::ORIGIN_WORKER );
  client_exchange.set_type( Reveal::Analytics::exchange_c::TYPE_REQUEST );
  //client_exchange.set_error( Reveal::Analytics::exchange_c::ERROR_NONE );
  client_exchange.set_request( Reveal::Analytics::exchange_c::REQUEST_JOB );

  client_exchange.set_authorization( auth );

  // build the request message
  if( !client_exchange.build( request ) ) return false;

  // send the request message and wait for reply message
  if( !request_reply(request, reply) ) return false;

  // parse the reply message
  if( !server_exchange.parse( reply ) ) return false;

  /*
  Reveal::Analytics::exchange_c::error_e transport_error = server_exchange.get_error();
  if( transport_error != Reveal::Analytics::exchange_c::ERROR_NONE ) {
    // The server sent a general error.  Suggest retrying the request.
    // TODO : extend validation and error handling for any new cases that emerge
    return false;
  }
*/
  // extract the job
  //digest = server_exchange.get_digest();
 
  return true;
}

//-----------------------------------------------------------------------------
bool client_c::login( void ) {

  // Note: this should actually be structured to trigger by a signal sent from 
  // server to reduce exploit
  for( unsigned i = 0; i < 3; i++ ) {
    // Prompt for Login
    bool anonymous_login = !Reveal::Core::console_c::prompt_yes_no( "Would you like to login (Y/N)?" );

    // Authorization
    _user = Reveal::Core::user_ptr( new Reveal::Core::user_c() );
    _auth = Reveal::Core::authorization_ptr( new Reveal::Core::authorization_c());

    if( anonymous_login ) {
      _auth->set_type( Reveal::Core::authorization_c::TYPE_ANONYMOUS );
    } else {
      _user->id = Reveal::Core::console_c::prompt( "Please enter a username:" );
      _auth->set_type( Reveal::Core::authorization_c::TYPE_IDENTIFIED );
      _auth->set_user( _user->id );
    }  

    if( anonymous_login ) 
      printf( "Logging in and requesting anonymous authorization.\n" );
    else
      printf( "Logging in and requesting authorization for user %s.\n", _user->id.c_str() );

    // TODO: hammer on server side validation.  Noticed that authorization is
    // granted if a bad user name is specified
    if( !request_authorization( _auth ) ) {
      printf( "ERROR: Failed to gain authorization\n" );
    } else {
      printf( "Authorization granted\n" );
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
} // namespace Analytics
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
