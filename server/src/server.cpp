#include <Reveal/server.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>

#include <Reveal/protocol_manager.h>
#include <Reveal/server_message.h>
#include <Reveal/client_message.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>

#include <Reveal/pendulum.h>

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
      Reveal::Core::scenario_ptr scenario = clientmsg.get_scenario();
 
      // NOTE: If we use workers for DB interaction, worker spawned HERE.
      // Query into the database to construct the scenario

      // simulate database transaction time
      sleep(DB_SIMULATED_WORK_TIME);

      // Example scenario:
      if( scenario->name.compare( "test" ) == 0 ) {
        scenario->trials = 2;
        scenario->urls.push_back( "http://www.gazebosim.org/" );
        scenario->urls.push_back( "http://www.osrfoundation.org/" );
      } else if( scenario->name.compare( "pendulum" ) == 0 ) {
        scenario->trials = 10;
      }

      // once the scenario created, build a message
      servermsg.set_scenario( scenario );
      // serialize
      msg_response = servermsg.serialize();
    } else if( clientmsg.get_type() == Reveal::Core::client_message_c::TRIAL ) {
      Reveal::Core::trial_ptr trial = clientmsg.get_trial();

      // NOTE: If we use workers for DB interaction, worker spawned HERE.
      // Query the database to construct the trial
      // TODO: Add database API interface

      // simulate database transaction time
      sleep(DB_SIMULATED_WORK_TIME);

      // Example Trials
      if( trial->scenario == "test" ) {
        if( trial->index == 0 ) {
          trial->t = 2.01;
          trial->dt = 0.001;
          // build state
          trial->state.append_q( 1.0 );
          trial->state.append_q( 2.0 );
          trial->state.append_dq( 3.0 );
          trial->state.append_dq( 4.0 );
          // build command
          trial->control.append( 5.0 );
          trial->control.append( 6.0 );
        } else if( trial->index == 1 ) {
          trial->t = 1.01;
          trial->dt = 0.001;
          // build state
          trial->state.append_q( 11.1 );
          trial->state.append_q( 12.1 );
          trial->state.append_dq( 13.1 );
          trial->state.append_dq( 14.1 );
          // build command
          trial->control.append( 15.1 );
          trial->control.append( 16.1 );
        }
      } else if( trial->scenario == "pendulum" ) {
        double q, dq, t, dt; 
        if( pendulum_c::sample_trial( trial->index, q, dq, t, dt ) ) {
          trial->t = t;
          trial->dt = dt;
          // build state
          trial->state.append_q( q );
          trial->state.append_dq( dq );
          // no command
        } else {
          printf( "failed to fetch trial data from database\n" );
          // throw?
        }
      }

      // make adjustments then setup the servermsg using the same trial ref
      servermsg.set_trial( trial );
      msg_response = servermsg.serialize();
 
    } else if( clientmsg.get_type() == Reveal::Core::client_message_c::SOLUTION ) {
      Reveal::Core::solution_ptr solution = clientmsg.get_solution();

      // TODO: comment/remove later
      solution->print();

      // NOTE: If we use workers for DB interaction, worker spawned HERE.
      // NOTE: Spawn analytic worker after sending response
      // TODO: Add database API interface

      // make adjustments then setup the servermsg using the same solution ref
      // Note that there are differences here because we are replying with OK
      servermsg.set_solution( solution );
      msg_response = servermsg.serialize();

      // simulate database transaction time
      sleep(DB_SIMULATED_WORK_TIME);

      // simulate/validate/analyze
      if( solution->scenario == "pendulum" ) {
        assert( solution->index < 10 );
        const double EPSILON = PENDULUM_VALIDATION_EPSILON;
        std::vector<double> x;
        //x.clear();
        double q, dq, ti, dt, tf;
        if( pendulum_c::sample_trial( solution->index, q, dq, ti, dt ) ) {
          x.push_back( q );
          x.push_back( dq );
          tf = ti + dt;
          pendulum_c pendulum( EXPERIMENTAL_PENDULUM_L );
          boost::numeric::odeint::integrate_adaptive( stepper_type(), pendulum, x, ti, tf, dt );
          if( fabs(x[0] - solution->state.q(0)) < EPSILON && fabs(x[1] - solution->state.dq(0)) < EPSILON ) {
            printf( "Client passed pendulum trial[%d]\n", solution->index );
          } else {
            printf( "Client failed pendulum trial[%d]: server(q[%f],dq[%f]), client(q[%f],dq[%f])\n", solution->index, x[0], x[1], solution->state.q(0), solution->state.dq(0) );
          }
        } else {
          printf( "failed to fetch trial data from database\n" );
          // throw?
        }
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
