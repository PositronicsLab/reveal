#include <Reveal/client.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>

#include <Reveal/pointers.h>
#include <Reveal/server_message.h>
#include <Reveal/client_message.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>

namespace Reveal {

//-----------------------------------------------------------------------------
/// Default Constructor
Client::Client( void ) {

}

//-----------------------------------------------------------------------------
/// Destructor
Client::~Client( void ) {

}

//-----------------------------------------------------------------------------
/// Initialization
bool Client::Init( void ) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  return true;
}

//-----------------------------------------------------------------------------
/// Main loop
void Client::Go( void ) {

  std::string msg_request;
  std::string msg_response;

  ClientMessage clientmsg;
  ServerMessage servermsg;

  // connect to the transport layer
  if( !Connect() ) return;

  // create a scenario
  ScenarioPtr scenario = ScenarioPtr( new Scenario() );
  // request Simulation Scenario by id
  scenario->name = "test";
  // build a scenario request message
  clientmsg.setScenario( scenario );
  // serialize the request message for transport
  msg_request = clientmsg.Serialize();

  // send the request message to the server
  if( !_connection.Write( msg_request ) ) {
    // write failed at connection
    printf( "ERROR: failed to write message to connection\n" );
    // TODO: improve error handling
  }
  // block waiting for a server response
  if( !_connection.Read( msg_response ) ) {
    // read failed at connection
    // right now this should trigger an assert
    // TODO: improve error handling
  }
  
  // parse the serialized response message received from the server
  if( !servermsg.Parse( msg_response ) ) {
    printf( "ERROR: Failed to parse ServerResponse\n" );
    // TODO: improve error handling
  }
  // get the scenario out of the message
  scenario = servermsg.getScenario();

  // TODO: comment/remove later
  scenario->Print();

  // set up simulation
  // TODO: develop API interfaces to handle setting up the sim

  // *For each trial* 
  for( unsigned i = 0; i < scenario->trials; i++ ) {
    // *Request a trial*

    // create a trial
    TrialPtr trial = TrialPtr( new Trial() );
    // populate the trial structure with scenario information
    trial->scenario = scenario->name;
    trial->index = i;
  
    // set the client message from the trial data
    clientmsg.setTrial( trial );
    // serialize the message for transport
    msg_request = clientmsg.Serialize();

    // send the trial request to the server
    if( !_connection.Write( msg_request ) ) {
      // write failed at connection
      printf( "ERROR: failed to write message to connection\n" );
      // TODO: improve error handling
    }
    // block waiting for a server response
    if( !_connection.Read( msg_response ) ) {
      // read failed at connection
      // right now this should trigger an assert within Read
      // TODO: improve error handling
    }
    // parse the serialized response message received from the server
    if( !servermsg.Parse( msg_response ) ) {
      printf( "ERROR: Failed to parse ServerResponse\n" );
      // TODO: improve error handling 
    }

    // TODO: add checking that the message is of expected type
    trial = servermsg.getTrial();

    // TODO: comment/remove later
    trial->Print();
  
    // Run simulation
    // TODO: develop API interfaces to handle setting the sim configuration
    // TODO: develop API interfaces to handle running the sim forward
    // TODO: develop API interfaces to handle extracting state from sim

    // generate a solution
    // following is just a populating a scenario with a bogus test case
    SolutionPtr solution = SolutionPtr( new Solution() );
    if( trial->scenario == "test" ) {
      if( trial->index == 0 ) {
        solution->scenario = trial->scenario;
        solution->index = trial->index;
        solution->t = trial->t + trial->dt;
        solution->state.Append_q( 4.0 );
        solution->state.Append_q( 6.0 );
        solution->state.Append_dq( 3.0 );
        solution->state.Append_dq( 4.0 );
      } else if( trial->index == 1 ) {
        solution->scenario = trial->scenario;
        solution->index = trial->index;
        solution->t = trial->t + trial->dt;
        solution->state.Append_q( 24.2 );
        solution->state.Append_q( 26.2 );
        solution->state.Append_dq( 15.1 );
        solution->state.Append_dq( 16.1 );
      }
    }

    // set the client message from the solution data 
    clientmsg.setSolution( solution );
    // serialize the message for transport
    msg_request = clientmsg.Serialize();

    // send the solution 'request' to the server  
    if( !_connection.Write( msg_request ) ) {
      // write failed at connection
      printf( "ERROR: failed to write message to connection\n" );
      // TODO: improve error handling 
    }
    // block waiting for a response
    if( !_connection.Read( msg_response ) ) {
      // read failed at connection
      // right now this should trigger an assert within Read
      // TODO: improve error handling 
    }
    // parse out the reponse to validate the solution was received
    if( !servermsg.Parse( msg_response ) ) {
      printf( "ERROR: Failed to parse ServerResponse\n" );
      // TODO: improve error handling 
    }
    // TODO: add checking that the message was properly received and that
    //       server sent a receipt
  }
}

//-----------------------------------------------------------------------------
/// Clean up
void Client::Terminate( void ) {
  _connection.Close();

  google::protobuf::ShutdownProtobufLibrary();
}

//-----------------------------------------------------------------------------
/// Connect to the transport layer as a client
bool Client::Connect( void ) {
  printf( "Connecting to server...\n" );

  _connection = Connection( "localhost", PORT );
  if( !_connection.Open() ) {
    return false;
  }

  printf( "Connected\n" );
  return true;
}

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------
