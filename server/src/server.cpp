#include <Reveal/server.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>

#include <Reveal/protocol_manager.h>
#include <Reveal/server_message.h>
#include <Reveal/client_message.h>
#include <Reveal/pointers.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>
#include <Reveal/model_solution.h>

#include <Reveal/pendulum.h>

#include <Reveal/database.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Server {

//-----------------------------------------------------------------------------
/// Default Constructor
server_c::server_c( void ) {

}

//-----------------------------------------------------------------------------
/// Destructor
server_c::~server_c( void ) {

}

//-----------------------------------------------------------------------------
/// Initialization
bool server_c::init( void ) {
  Reveal::Core::protocol_manager_c::start();  

  _clientconnection = Reveal::Core::connection_c( PORT );
  if( !_clientconnection.open() ) {
    printf( "Failed to open clientconnection\n" );
    return false;
  }

  _workerconnection = Reveal::Core::connection_c( Reveal::Core::connection_c::DEALER, _clientconnection.context() );
  if( !_workerconnection.open() ) {
    printf( "Failed to open workerconnection\n" );
    return false; 
  }

  void* context = _clientconnection.context();
  for( unsigned i = 0; i < MAX_CLIENT_WORKERS; i++ ) {
    pthread_t worker_thread;
    pthread_create( &worker_thread, NULL, server_c::client_worker, context );
    workers.push_back( worker_thread );
  }

  printf( "Server is listening...\n" );

  return true; 
}

//-----------------------------------------------------------------------------
/// Main Loop
void server_c::run( void ) {
  _clientconnection.route( _workerconnection );
}

//-----------------------------------------------------------------------------
void server_c::terminate( void ) {
  _workerconnection.close();
  _clientconnection.close();

  Reveal::Core::protocol_manager_c::shutdown();

} 

//-----------------------------------------------------------------------------
void* server_c::client_worker( void* context ) {
  Reveal::Core::connection_c receiver = Reveal::Core::connection_c( Reveal::Core::connection_c::WORKER, context );

  std::string msg_request;
  std::string msg_response;

  Reveal::Core::client_message_c clientmsg;
  Reveal::Core::server_message_c servermsg;

  if( !receiver.open() ) {
    printf( "worker failed to open connection\n" );
    // return?
  }

  Reveal::DB::database_c db( "localhost" );
  db.open();

  while( true ) {
    // block waiting for a message from a client
    if( !receiver.read( msg_request ) ) {
      // read failed at connection
      printf( "ERROR: Failed to read message from client.\n" );
      // TODO: improve error handling.  Bomb or recover here.      
    }

    // parse the serialized request
    if( !clientmsg.parse( msg_request ) ) {
      printf( "ERROR: Failed to parse ClientRequest\n" );
      // TODO: improve error handling.  Bomb or recover here.      
    } 

    // determine the course of action
    if( clientmsg.get_type() == Reveal::Core::client_message_c::SCENARIO ) {
      Reveal::Core::scenario_ptr client_scenario;
      Reveal::Core::scenario_ptr server_scenario;

      // extract the scenario from the client message
      client_scenario = clientmsg.get_scenario();
 
      // query the database for scenario data
      db.query( server_scenario, client_scenario->name );

      // construct the scenario message
      servermsg.set_scenario( server_scenario );

      // serialize the message for transmission
      msg_response = servermsg.serialize();

    } else if( clientmsg.get_type() == Reveal::Core::client_message_c::TRIAL ) {
      Reveal::Core::trial_ptr client_trial;
      Reveal::Core::trial_ptr server_trial;

      // extract the trial from the client message
      client_trial = clientmsg.get_trial();

      // query the database for trial data
      db.query( server_trial, client_trial->scenario, client_trial->index );

      // construct the trial message
      servermsg.set_trial( server_trial );

      //serialize the message for transmission
      msg_response = servermsg.serialize();
 
    } else if( clientmsg.get_type() == Reveal::Core::client_message_c::SOLUTION ) {
      Reveal::Core::solution_ptr client_solution;

      // create a solution receipt
      client_solution = clientmsg.get_solution();

      // TODO: The general structure in this segment is subject to osrf feedback
      // We could send back a notice on good solution or bad as determined by
      // analytics, or we can keep client in the dark to minimize gaming.

      // construct the solution receipt message
      servermsg.set_solution( client_solution );

      // serialize the message for transmission
      msg_response = servermsg.serialize();

      // TODO: determine appropriate adjustment here.  This worker will bog
      // down chewing on the db operations and analytics here and will not
      // send the receipt until it reaches the bottom of this function.
      // It should immediately return the receipt then deal with database and
      // analytics

      // insert the client solution into the database
      db.insert( client_solution );

      // - Analytics -
      Reveal::Core::model_solution_ptr server_solution;
      db.query( server_solution, client_solution->scenario, client_solution->index );

      if( fabs(server_solution->state.q(0) - client_solution->state.q(0)) < server_solution->epsilon.q(0) && fabs(server_solution->state.dq(0) - client_solution->state.dq(0)) < server_solution->epsilon.dq(0) ) {
        printf( "Client passed pendulum trial[%d]\n", client_solution->index );
      } else {
        printf( "Client failed pendulum trial[%d]: server(q[%f],dq[%f]:Eq[%f],Edq[%f]), client(q[%f],dq[%f])\n", client_solution->index, server_solution->state.q(0), server_solution->state.dq(0), server_solution->epsilon.q(0), server_solution->epsilon.dq(0), client_solution->state.q(0), client_solution->state.dq(0) );
      }
    } else if( clientmsg.get_type() == Reveal::Core::client_message_c::ERROR ) {

    } 

    // if the last trial, then spawn an analytic worker that queries the db,
    // applies metrics, and writes results back to db
    // Note: may need a little more server complexity to join workers while
    // continuing to service requests

    // broadcast it back to the client
    receiver.write( msg_response );
  }
  receiver.close();
  return NULL;
}

//-----------------------------------------------------------------------------

} // namespace Server

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------
