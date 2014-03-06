#include <Reveal/server.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>

#include <Reveal/server_message.h>
#include <Reveal/client_message.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>

namespace Reveal {

//-----------------------------------------------------------------------------
/// Default Constructor
Server::Server( void ) {

}

//-----------------------------------------------------------------------------
/// Destructor
Server::~Server( void ) {

}

//-----------------------------------------------------------------------------
/// Initialization
bool Server::Init( void ) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  _connection = Connection( PORT );
  if( !_connection.Open() ) {
    return false;
  }

  printf( "Server is listening...\n" );

  return true; 
}

//-----------------------------------------------------------------------------
/// Main Loop
void Server::Run( void ) {

  std::string msg_request;
  std::string msg_response;

  ClientMessage clientmsg;
  ServerMessage servermsg;

  while( true ) {

    // block waiting for a message from a client
    if( !_connection.Read( msg_request ) ) {
      // read failed at connection
      // TODO: improve error handling      
    }

    // parse the serialized request
    if( !clientmsg.Parse( msg_request ) ) {
      printf( "ERROR: Failed to parse ClientRequest\n" );
      // TODO: improve error handling      
    } 

    // determine the course of action
    if( clientmsg.getType() == ClientMessage::SCENARIO ) {
      ScenarioPtr scenario = clientmsg.getScenario();
 
      // NOTE: If we use workers for DB interaction, worker spawned HERE.
      // Query into the database to construct the scenario

      // Example scenario:
      if( scenario->name.compare( "test" ) == 0 ) {
        scenario->trials = 2;
        scenario->urls.push_back( "http://www.gazebosim.org/" );
        scenario->urls.push_back( "http://www.osrfoundation.org/" );
      }

      // once the scenario created, build a message
      servermsg.setScenario( scenario );
      // serialize
      msg_response = servermsg.Serialize();
    } else if( clientmsg.getType() == ClientMessage::TRIAL ) {
      TrialPtr trial = clientmsg.getTrial();

      // NOTE: If we use workers for DB interaction, worker spawned HERE.
      // Query the database to construct the trial
      // TODO: Add database API interface

      // Example Trials
      if( trial->scenario == "test" ) {
        if( trial->index == 0 ) {
          trial->t = 2.01;
          trial->dt = 0.001;
          // build state
          trial->state.Append_q( 1.0 );
          trial->state.Append_q( 2.0 );
          trial->state.Append_dq( 3.0 );
          trial->state.Append_dq( 4.0 );
          // build command
          trial->control.Append( 5.0 );
          trial->control.Append( 6.0 );
        } else if( trial->index == 1 ) {
          trial->t = 1.01;
          trial->dt = 0.001;
          // build state
          trial->state.Append_q( 11.1 );
          trial->state.Append_q( 12.1 );
          trial->state.Append_dq( 13.1 );
          trial->state.Append_dq( 14.1 );
          // build command
          trial->control.Append( 15.1 );
          trial->control.Append( 16.1 );
        }
      }

      // make adjustments then setup the servermsg using the same trial ref
      servermsg.setTrial( trial );
      msg_response = servermsg.Serialize();
 
    } else if( clientmsg.getType() == ClientMessage::SOLUTION ) {
      SolutionPtr solution = clientmsg.getSolution();

      // TODO: comment/remove later
      solution->Print();

      // NOTE: If we use workers for DB interaction, worker spawned HERE.
      // NOTE: Spawn analytic worker after sending response
      // TODO: Add database API interface

      // make adjustments then setup the servermsg using the same solution ref
      // Note that there are differences here because we are replying with OK
      servermsg.setSolution( solution );
      msg_response = servermsg.Serialize();
    } else if( clientmsg.getType() == ClientMessage::ERROR ) {

    } 

    // if the last trial, then spawn an analytic worker that queries the db,
    // applies metrics, and writes results back to db
    // Note: may need a little more server complexity to join workers while
    // continuing to service requests

    // broadcast it back to the client
    _connection.Write( msg_response );
    // NOTE: End of worker
  }
}

//-----------------------------------------------------------------------------
void Server::Terminate( void ) {
  _connection.Close();

  google::protobuf::ShutdownProtobufLibrary();

} 
/*
//-----------------------------------------------------------------------------
void* Server::AnalyticWorker( void* arg ) {
  printf( "Analytic Worker Thread Spawned...\n" );
  
  return NULL;
}
*/
//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------
