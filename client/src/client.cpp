#include <Reveal/client.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>

#include <Reveal/protocol_manager.h>
#include <Reveal/pointers.h>
#include <Reveal/server_message.h>
#include <Reveal/client_message.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>

#include <Reveal/pendulum.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Client {

//-----------------------------------------------------------------------------
/// Default Constructor
client_c::client_c( void ) {

}

//-----------------------------------------------------------------------------
/// Destructor
client_c::~client_c( void ) {

}

//-----------------------------------------------------------------------------
/// Initialization
bool client_c::init( void ) {
  Reveal::Core::protocol_manager_c::start();

  return true;
}

//-----------------------------------------------------------------------------
/// Main loop
void client_c::go( void ) {

  std::string msg_request;
  std::string msg_response;

  Reveal::Core::client_message_c clientmsg;
  Reveal::Core::server_message_c servermsg;

  // connect to the transport layer
  if( !connect() ) return;

  // create a scenario
  Reveal::Core::scenario_ptr scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );
  // request Simulation Scenario by id
  //scenario->name = "test";
  scenario->name = "pendulum";
  // build a scenario request message
  clientmsg.set_scenario( scenario );
  // serialize the request message for transport
  msg_request = clientmsg.serialize();

  // send the request message to the server
  if( !_connection.write( msg_request ) ) {
    // write failed at connection
    printf( "ERROR: failed to write message to connection\n" );
    // TODO: improve error handling.  Should bomb or recover here.
  }
  // block waiting for a server response
  if( !_connection.read( msg_response ) ) {
    // read failed at connection
    // right now this should trigger an assert
    // TODO: improve error handling.  Should bomb or recover here.
  }
  
  // parse the serialized response message received from the server
  if( !servermsg.parse( msg_response ) ) {
    printf( "ERROR: Failed to parse ServerResponse\n" );
    // TODO: improve error handling.  Should bomb or recover here.
  }
  // get the scenario out of the message
  scenario = servermsg.get_scenario();

  // TODO: comment/remove later
  scenario->print();

  // set up simulation
  // TODO: develop API interfaces to handle setting up the sim

  // *For each trial* 
  for( unsigned i = 0; i < scenario->trials; i++ ) {
    // *Request a trial*

    // create a trial
    Reveal::Core::trial_ptr trial = Reveal::Core::trial_ptr( new Reveal::Core::trial_c() );
    // populate the trial structure with scenario information
    trial->scenario = scenario->name;
    trial->index = i;
  
    // set the client message from the trial data
    clientmsg.set_trial( trial );
    // serialize the message for transport
    msg_request = clientmsg.serialize();

    // send the trial request to the server
    if( !_connection.write( msg_request ) ) {
      // write failed at connection
      printf( "ERROR: failed to write message to connection\n" );
      // TODO: improve error handling.  Should bomb or recover here.
    }
    // block waiting for a server response
    if( !_connection.read( msg_response ) ) {
      // read failed at connection
      printf( "ERROR: failed to read message from connection\n" );
      // TODO: improve error handling.  Should bomb or recover here.
    }
    // parse the serialized response message received from the server
    if( !servermsg.parse( msg_response ) ) {
      printf( "ERROR: Failed to parse ServerResponse\n" );
      // TODO: improve error handling.  Should bomb or recover here.
    }

    // TODO: add checking that the message is of expected type
    trial = servermsg.get_trial();

    // TODO: comment/remove later
    trial->print();
  
    // Run simulation
    // TODO: develop API interfaces to handle setting the sim configuration
    // TODO: develop API interfaces to handle running the sim forward
    // TODO: develop API interfaces to handle extracting state from sim

    // generate a solution
    // following is just a populating a scenario with a bogus test case
    Reveal::Core::solution_ptr solution = Reveal::Core::solution_ptr( new Reveal::Core::solution_c() );
    if( trial->scenario == "test" ) {
      if( trial->index == 0 ) {
        solution->scenario = trial->scenario;
        solution->index = trial->index;
        solution->t = trial->t + trial->dt;
        solution->state.append_q( 4.0 );
        solution->state.append_q( 6.0 );
        solution->state.append_dq( 3.0 );
        solution->state.append_dq( 4.0 );
      } else if( trial->index == 1 ) {
        solution->scenario = trial->scenario;
        solution->index = trial->index;
        solution->t = trial->t + trial->dt;
        solution->state.append_q( 24.2 );
        solution->state.append_q( 26.2 );
        solution->state.append_dq( 15.1 );
        solution->state.append_dq( 16.1 );
      }
    } else if( trial->scenario == "pendulum" ) {
      // copy the solution header
      solution->scenario = trial->scenario;
      solution->index = trial->index;

      // simulate
      std::vector<double> x;
      x.push_back( trial->state.q(0) );
      x.push_back( trial->state.dq(0) );
      double ti = trial->t;
      double dt = trial->dt;
      double tf = ti + dt;
      pendulum_c pendulum( EXPERIMENTAL_PENDULUM_L );
#ifdef RUNGEKUTTA_STEPPER
      boost::numeric::odeint::integrate_adaptive( stepper_type(), pendulum, x, ti, tf, dt );
#elif defined(EULER_STEPPER)
      boost::numeric::odeint::integrate_const( stepper_type(), pendulum, x, ti, tf, dt );
#endif
      // add solution result
      solution->t = tf;
      solution->state.append_q( x[0] );
      solution->state.append_dq( x[1] );
      solution->print();
    }

    // set the client message from the solution data 
    clientmsg.set_solution( solution );
    // serialize the message for transport
    msg_request = clientmsg.serialize();

    // send the solution 'request' to the server  
    if( !_connection.write( msg_request ) ) {
      // write failed at connection
      printf( "ERROR: failed to write message to connection\n" );
      // TODO: improve error handling.  Should bomb or recover here.
    }
    // block waiting for a response
    if( !_connection.read( msg_response ) ) {
      // read failed at connection
      // right now this should trigger an assert within Read
      // TODO: improve error handling.  Should bomb or recover here.
    }
    // parse out the reponse to validate the solution was received
    if( !servermsg.parse( msg_response ) ) {
      printf( "ERROR: Failed to parse ServerResponse\n" );
      // TODO: improve error handling.  Should bomb or recover here.
    }
    // Note: the server solution response is just a receipt not a score.
    // TODO: add checking that the message was properly received and that
    //       server sent a receipt
  }
}

//-----------------------------------------------------------------------------
/// Clean up
void client_c::terminate( void ) {
  _connection.close();

  Reveal::Core::protocol_manager_c::shutdown();
}

//-----------------------------------------------------------------------------
/// Connect to the transport layer as a client
bool client_c::connect( void ) {
  printf( "Connecting to server...\n" );

  _connection = Reveal::Core::connection_c( REVEAL_SERVER_URI, PORT );
  if( !_connection.open() ) {
    return false;
  }

  printf( "Connected\n" );
  return true;
}

//-----------------------------------------------------------------------------

} // namespace Client

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------
