#include <Reveal/worker.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>

#include <Reveal/transport_exchange.h>
#include <Reveal/pointers.h>
#include <Reveal/digest.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>
#include <Reveal/model_solution.h>

#include <Reveal/database.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Server {

//-----------------------------------------------------------------------------

worker_c::worker_c( void* context ) {
  _context = context;
}

//-----------------------------------------------------------------------------
worker_c::~worker_c( void ) {

}

//-----------------------------------------------------------------------------
bool worker_c::init( void ) {
  _connection = Reveal::Core::connection_c( Reveal::Core::connection_c::WORKER, _context );

  if( _connection.open() != Reveal::Core::connection_c::ERROR_NONE ) {
    printf( "worker failed to open connection\n" );
    // return?
    // TODO: determine what to do in event of failure to open connection
  }

  _db = boost::shared_ptr<Reveal::DB::database_c>( new Reveal::DB::database_c( "localhost" ) );
  _db->open();

  return true;
}

//-----------------------------------------------------------------------------
void worker_c::terminate( void ) {
  _db->close();
  // Note: may need to loop on connection close operation if they don't
  // return ERROR_NONE and do return ERROR_INTERRUPT
  _connection.close();
}

//-----------------------------------------------------------------------------
void worker_c::work( void ) {
  Reveal::Core::transport_exchange_c exchange;
  std::string request;

  while( true ) {
    // block waiting for a message from a client
    if( _connection.read( request ) != Reveal::Core::connection_c::ERROR_NONE ) {
      // read failed at connection
      printf( "ERROR: Failed to read message from client.\n" );
      // TODO: improve error handling.  Bomb or recover here.      
    }

    // parse the serialized request
    if( exchange.parse( request ) != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
      printf( "ERROR: Failed to parse client request\n" );
      // TODO: improve error handling.  Bomb or recover here.       
    }

    // determine the course of action
    if( exchange.get_type() == Reveal::Core::transport_exchange_c::TYPE_DIGEST ) {
      service_digest_request( );
      // TODO : error checking

    } else if( exchange.get_type() == Reveal::Core::transport_exchange_c::TYPE_SCENARIO ) {
      // extract the scenario from the client message
      Reveal::Core::scenario_ptr scenario = exchange.get_scenario();

      //printf( "client_scenario:\n" );
      //scenario->print();

      service_scenario_request( scenario->id );
      // TODO : error checking

    } else if( exchange.get_type() == Reveal::Core::transport_exchange_c::TYPE_TRIAL ) {
      // extract the trial from the client message
      Reveal::Core::trial_ptr trial = exchange.get_trial();      

      service_trial_request( trial->scenario_id, trial->trial_id );
      // TODO : error checking

    } else if( exchange.get_type() == Reveal::Core::transport_exchange_c::TYPE_SOLUTION ) {
      // create a solution receipt
      Reveal::Core::solution_ptr solution = exchange.get_solution();

      service_solution_submission( solution );
      // TODO : error checking

    } else if( exchange.get_type() == Reveal::Core::transport_exchange_c::TYPE_ERROR ) {

    } 
  }
}

//-----------------------------------------------------------------------------
worker_c::error_e worker_c::service_digest_request( void ) {
  Reveal::Core::transport_exchange_c exchange;
  Reveal::Core::digest_ptr digest;
  std::string reply;

  // query the database for digest data
  Reveal::DB::database_c::error_e db_error = _db->query( digest );

  if( db_error == Reveal::DB::database_c::ERROR_NONE ) {
    // the query was successful

    //digest->print();      

    // construct the digest message
    exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_SERVER );
    exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_DIGEST );
    exchange.set_digest( digest );

    // serialize the message for transmission
    exchange.build( reply );

    // broadcast the reply message back to the client
    if( _connection.write( reply ) != Reveal::Core::connection_c::ERROR_NONE ) {
      // TODO: trap and recover
    }

    return ERROR_NONE;
  }

  // otherwise there was an error in the query
  if( db_error == Reveal::DB::database_c::ERROR_EMPTYSET ) {
    // the query returned an empty set
  }
  return ERROR_NONE;  // temporary until any error enumeration is determined
}

//-----------------------------------------------------------------------------
worker_c::error_e worker_c::service_scenario_request( int scenario_id ) {
  Reveal::Core::transport_exchange_c exchange;
  Reveal::Core::scenario_ptr scenario;
  std::string reply;

  // query the database for scenario data
  Reveal::DB::database_c::error_e db_error = _db->query( scenario, scenario_id );

  if( db_error == Reveal::DB::database_c::ERROR_NONE ) {
    // the query was successful

    // construct the scenario message
    exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_SERVER );
    exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_SCENARIO );
    exchange.set_scenario( scenario );

    // serialize the message for transmission
    exchange.build( reply );

    // broadcast the reply message back to the client
    if( _connection.write( reply ) != Reveal::Core::connection_c::ERROR_NONE ) {
      // TODO: trap and recover
    }

    return ERROR_NONE;
  }

  // otherwise there was an error in the query
  if( db_error == Reveal::DB::database_c::ERROR_EMPTYSET ) {
    // the query returned an empty set

    // construct an error message
    exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_SERVER );
    exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_ERROR );
    exchange.set_error( Reveal::Core::transport_exchange_c::ERROR_BAD_SCENARIO_REQUEST );

    //serialize the message for transmission
    exchange.build( reply );

    // broadcast the reply message back to the client
    if( _connection.write( reply ) != Reveal::Core::connection_c::ERROR_NONE ) {
      // TODO: trap and recover
    }
  }
  return ERROR_NONE;  // temporary until any error enumeration is determined
}

//-----------------------------------------------------------------------------
worker_c::error_e worker_c::service_trial_request( int scenario_id, int trial_id ) {
  Reveal::Core::transport_exchange_c exchange;
  Reveal::Core::trial_ptr trial;
  std::string reply;

  // query the database for trial data
  Reveal::DB::database_c::error_e db_error = _db->query( trial, scenario_id, trial_id );
  // TODO: Validation

  if( db_error == Reveal::DB::database_c::ERROR_NONE ) {
    // the query was successful

    // construct the trial message
    exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_SERVER );
    exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_TRIAL );
    exchange.set_trial( trial );

    //serialize the message for transmission
    exchange.build( reply );

    // broadcast the reply message back to the client
    if( _connection.write( reply ) != Reveal::Core::connection_c::ERROR_NONE ) {
      // TODO: trap and recover
    }

    return ERROR_NONE;
  }

  // otherwise there was an error in the query
  if( db_error == Reveal::DB::database_c::ERROR_EMPTYSET ) {
    // the query returned an empty set

    // construct an error message
    exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_SERVER );
    exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_ERROR );
    exchange.set_error( Reveal::Core::transport_exchange_c::ERROR_BAD_TRIAL_REQUEST );

    //serialize the message for transmission
    exchange.build( reply );

    // broadcast the reply message back to the client
    if( _connection.write( reply ) != Reveal::Core::connection_c::ERROR_NONE ) {
      // TODO: trap and recover
    }
  }
  return ERROR_NONE;  // temporary until any error enumeration is determined
}

//-----------------------------------------------------------------------------
worker_c::error_e worker_c::service_solution_submission( Reveal::Core::solution_ptr solution ) {
  Reveal::Core::transport_exchange_c exchange;
  std::string reply;

  // construct the solution receipt message
  exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_SERVER );
  exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_SOLUTION );

  //serialize the message for transmission
  exchange.build( reply );

  // broadcast the reply message back to the client
  if( _connection.write( reply ) != Reveal::Core::connection_c::ERROR_NONE ) {
    // TODO: trap and recover
  }

  // NOTE: Change in pattern from above.  Send reply immediately then determine
  // the interactions necessary.  This prevents clients from waiting on data
  // transactions

  // insert the client solution into the database
  Reveal::DB::database_c::error_e db_error = _db->insert( solution );

  // - Analytics -
  Reveal::Core::model_solution_ptr model_solution;
  _db->query( model_solution, solution->scenario_id, solution->trial_id );

  //if( fabs(server_solution->state.q(0) - client_solution->state.q(0)) < server_solution->epsilon.q(0) && fabs(server_solution->state.dq(0) - client_solution->state.dq(0)) < server_solution->epsilon.dq(0) ) {

//    if( fabs(server_solution->state.q(0) - client_solution->state.q(0)) < server_solution->epsilon.q(0) && fabs(server_solution->state.dq(0) - client_solution->state.dq(0)) < server_solution->epsilon.dq(0) ) {
//      printf( "Client passed pendulum trial[%d]\n", client_solution->trial_id );
//    } else {
//      printf( "Client failed pendulum trial[%d]: server(q[%f],dq[%f]:Eq[%f],Edq[%f]), client(q[%f],dq[%f])\n", client_solution->trial_id, server_solution->state.q(0), server_solution->state.dq(0), server_solution->epsilon.q(0), server_solution->epsilon.dq(0), client_solution->state.q(0), client_solution->state.dq(0) );
//    }
  //}

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------

} // namespace Server

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------
