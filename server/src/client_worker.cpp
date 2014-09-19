#include <Reveal/client_worker.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <uuid/uuid.h>

#include <Reveal/identity.h>
#include <Reveal/authorization.h>
#include <Reveal/user.h>
#include <Reveal/session.h>
#include <Reveal/transport_exchange.h>
#include <Reveal/pointers.h>
#include <Reveal/digest.h>
#include <Reveal/experiment.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>

#include <Reveal/database.h>

#include <Reveal/analytics/worker.h>

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
      break;  // BOMB for DEBUGGING MODE ONLY
    }

    // parse the serialized request
    if( exchange.parse( request ) != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
      printf( "ERROR: Failed to parse client request\n" );
      // TODO: improve error handling.  Bomb or recover here.       
    }

    // determine the course of action
    if( exchange.get_type() == Reveal::Core::transport_exchange_c::TYPE_HANDSHAKE ) {
      Reveal::Core::authorization_ptr auth = exchange.get_authorization();

      service_handshake_request( auth );
      // TODO : error checking

    } else if( exchange.get_type() == Reveal::Core::transport_exchange_c::TYPE_DIGEST ) {
      Reveal::Core::authorization_ptr auth = exchange.get_authorization();
      if( authorize( auth ) == ERROR_NONE ) {
        service_digest_request( auth );
        // TODO : error checking
      } else {
        service_failed_authorization( auth );
      }

    } else if( exchange.get_type() == Reveal::Core::transport_exchange_c::TYPE_EXPERIMENT ) {
      Reveal::Core::authorization_ptr auth = exchange.get_authorization();

      if( authorize( auth ) == ERROR_NONE ) {
        // extract the experiment from the client message
        Reveal::Core::experiment_ptr experiment = exchange.get_experiment();
        std::string scenario_name = experiment->scenario_id;

        //printf( "client_scenario:\n" );
        //scenario->print();

        service_experiment_request( auth, scenario_name );
        // TODO : error checking
      } else {
        service_failed_authorization( auth );
      }

    } else if( exchange.get_type() == Reveal::Core::transport_exchange_c::TYPE_TRIAL ) {
      Reveal::Core::authorization_ptr auth = exchange.get_authorization();
      if( authorize( auth ) == ERROR_NONE ) {
        // extract the trial from the client message
        Reveal::Core::experiment_ptr experiment = exchange.get_experiment();
        Reveal::Core::trial_ptr trial = exchange.get_trial();      

        service_trial_request( auth, experiment, trial->trial_id );
        // TODO : error checking
      } else {
        service_failed_authorization( auth );
      }

    } else if( exchange.get_type() == Reveal::Core::transport_exchange_c::TYPE_SOLUTION ) {
      Reveal::Core::authorization_ptr auth = exchange.get_authorization();
  std::string generate_uuid( void );
      if( authorize( auth ) == ERROR_NONE ) {
        // create a solution receipt
        Reveal::Core::experiment_ptr experiment = exchange.get_experiment();
        Reveal::Core::solution_ptr solution = exchange.get_solution();

        service_solution_submission( auth, experiment, solution );
        // TODO : error checking
      } else {
        service_failed_authorization( auth );
      }
    } else if( exchange.get_type() == Reveal::Core::transport_exchange_c::TYPE_ERROR ) {

    } 
  }
}

//-----------------------------------------------------------------------------
bool worker_c::is_user_valid( Reveal::Core::authorization_ptr auth, Reveal::Core::user_ptr& user ) {
  Reveal::DB::database_c::error_e db_error;

  // query the database for user data
  db_error = _db->query( user, auth->get_user() );

  // Superficial validation: if the user exists in the DB, it is valid
  if( db_error == Reveal::DB::database_c::ERROR_NONE )
    return true;

  // NOTE: deep validation will require much more considerate attention

  // TODO : DEEP VALIDATION

  return false;
}

//-----------------------------------------------------------------------------
bool worker_c::is_session_valid( Reveal::Core::authorization_ptr auth, Reveal::Core::session_ptr& session ) {
  Reveal::DB::database_c::error_e db_error;

  // query the database for user data
  db_error = _db->query( session, auth->get_session() );

  // Superficial validation: if the session exists in the DB, it is valid
  if( db_error == Reveal::DB::database_c::ERROR_NONE )
    return true;

  // NOTE: deep validation will require much more considerate attention
  // For example we want to avoid session hijack and we want to invalidate
  // session that have expired.

  // TODO : DEEP VALIDATION

  return false;
}

//-----------------------------------------------------------------------------
bool worker_c::is_experiment_valid( Reveal::Core::authorization_ptr auth, Reveal::Core::experiment_ptr experiment_request, Reveal::Core::experiment_ptr& experiment_record ) {
  Reveal::DB::database_c::error_e db_error;
/*
  // query the database for user data
  db_error = _db->query( experiment_record, auth->get_session(), experiment_request->experiment_id, experiment_request->scenario_id );

  // Superficial validation: if the session exists in the DB, it is valid
  if( db_error == Reveal::DB::database_c::ERROR_NONE )
    return true;
*/
  return false;
}

//-----------------------------------------------------------------------------
bool worker_c::create_session( Reveal::Core::authorization_ptr auth, Reveal::Core::session_ptr& session ) {
  Reveal::DB::database_c::error_e db_error;

  session = Reveal::Core::session_ptr( new Reveal::Core::session_c() );
  session->session_id = Reveal::Server::generate_uuid();

  Reveal::Core::authorization_c::type_e type = auth->get_type();
  if( type == Reveal::Core::authorization_c::TYPE_IDENTIFIED ) {
    session->user_type = Reveal::Core::session_c::IDENTIFIED;
    session->user_id = auth->get_user();
  } else if( type == Reveal::Core::authorization_c::TYPE_ANONYMOUS ) {
    session->user_type = Reveal::Core::session_c::ANONYMOUS;
    session->user_id = "";
  }

  db_error = _db->insert( session );
  if( db_error == Reveal::DB::database_c::ERROR_NONE )
    return true;

  return false;
}

//-----------------------------------------------------------------------------
bool worker_c::create_experiment( Reveal::Core::authorization_ptr auth, Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr& experiment ) {
  Reveal::DB::database_c::error_e db_error;

  experiment = Reveal::Core::experiment_ptr( new Reveal::Core::experiment_c() );
  experiment->experiment_id = Reveal::Server::generate_uuid();
  experiment->session_id = auth->get_session();
  experiment->scenario_id = scenario->id;
  experiment->number_of_trials = scenario->trials;
  experiment->current_trial_index = 0;

  db_error = _db->insert( experiment );
  if( db_error == Reveal::DB::database_c::ERROR_NONE )
    return true;

  return false;
}

//-----------------------------------------------------------------------------
worker_c::error_e worker_c::authorize( Reveal::Core::authorization_ptr auth ) {
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
worker_c::error_e worker_c::service_failed_authorization( Reveal::Core::authorization_ptr auth ) {

  Reveal::Core::transport_exchange_c exchange;
  std::string reply;

  // construct the digest message
  exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_SERVER );
  exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_ERROR );
  exchange.set_error( Reveal::Core::transport_exchange_c::ERROR_AUTHORIZATION );
  // TODO: what about the type of authorization error??
  // TODO: what about attaching the authorization pointer

  // serialize the message for transmission
  exchange.build( reply );

  // broadcast the reply message back to the client
  if( _connection.write( reply ) != Reveal::Core::connection_c::ERROR_NONE ) {
    // TODO: trap and recover
  }
  
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
worker_c::error_e worker_c::send_valid_handshake_response( Reveal::Core::authorization_ptr auth ) { 
  Reveal::Core::transport_exchange_c exchange;
  std::string reply;

  // construct the return handshake message
  exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_SERVER );
  exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_HANDSHAKE );
  exchange.set_authorization( auth );

  // serialize the message for transmission
  exchange.build( reply );

  // broadcast the reply message back to the client
  if( _connection.write( reply ) != Reveal::Core::connection_c::ERROR_NONE ) {
    // TODO: trap and recover
  }
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
worker_c::error_e worker_c::send_invalid_handshake_response( Reveal::Core::authorization_ptr auth ) { 
  Reveal::Core::transport_exchange_c exchange;
  std::string reply;

  // construct the return handshake message
  exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_SERVER );
  exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_HANDSHAKE );
  // TODO: set error
  exchange.set_authorization( auth );

  // serialize the message for transmission
  exchange.build( reply );

  // broadcast the reply message back to the client
  if( _connection.write( reply ) != Reveal::Core::connection_c::ERROR_NONE ) {
    // TODO: trap and recover
  }
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
worker_c::error_e worker_c::service_handshake_request( Reveal::Core::authorization_ptr auth ) { 

  //Reveal::Core::transport_exchange_c exchange;
  //std::string reply;
  Reveal::Core::session_ptr session;
  Reveal::DB::database_c::error_e db_error;
  Reveal::Core::user_ptr user;
 // Reveal::Core::authorization_ptr auth_reply;

  Reveal::Core::authorization_c::type_e type = auth->get_type();
  if( type == Reveal::Core::authorization_c::TYPE_IDENTIFIED ) {
    // validate user credentials

    printf( "client requested identified authorization: id[%s]\n", auth->get_user().c_str() );

    // TODO : Logging

    if( is_user_valid( auth, user ) ) {
      if( create_session( auth, session ) ) {
        printf( "created session: " ); session->print();

        auth->set_error( Reveal::Core::authorization_c::ERROR_NONE );
        auth->set_user( user->id );
        auth->set_type( Reveal::Core::authorization_c::TYPE_SESSION );
        auth->set_session( session->session_id );

        send_valid_handshake_response( auth );
      } else {
        // TODO : handle failed session insert into database

      }
    } else {
      printf( "ERROR: failed to find %s in user table\n", auth->get_user().c_str() );

      // failed query and failed authentication
      auth->set_error( Reveal::Core::authorization_c::ERROR_INVALID_IDENTITY );

      send_invalid_handshake_response( auth );
    }
  } else if( type == Reveal::Core::authorization_c::TYPE_ANONYMOUS ) {

    printf( "client requested anonymous authorization\n" );

    // TODO : Logging

    if( create_session( auth, session ) ) {
      printf( "created session: " ); session->print();

      auth->set_type( Reveal::Core::authorization_c::TYPE_SESSION );
      auth->set_session( session->session_id );

      send_valid_handshake_response( auth );
    } else {
      // TODO : handle failed session insert into database
    }
  } else if( type == Reveal::Core::authorization_c::TYPE_SESSION ) {
    // this should only occur if a session was disrupted to the point that 
    // the client is trying to resume after being completely disconnected 
    // from the server.  This is the most suspicious case though and 
    // probably should be denied as it is possible to session hijack if allowed.
  }

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
worker_c::error_e worker_c::service_digest_request( Reveal::Core::authorization_ptr auth ) {
  printf( "digest requested\n" );

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
    exchange.set_authorization( auth );

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
worker_c::error_e worker_c::service_experiment_request( Reveal::Core::authorization_ptr auth, std::string scenario_id ) {
  printf( "experiment requested\n" );

  Reveal::Core::transport_exchange_c exchange;
  Reveal::Core::scenario_ptr scenario;
  Reveal::Core::experiment_ptr experiment;
  std::string reply;

  Reveal::DB::database_c::error_e db_error = _db->query( scenario, scenario_id );
  if( db_error != Reveal::DB::database_c::ERROR_NONE ) {
    // the query was unsuccessful
    return ERROR_QUERY;
  }

  if( !create_experiment( auth, scenario, experiment ) ) {
    return ERROR_CREATE;
  }

  printf( "created experiment[%s], session[%s], scenario[%s]\n", experiment->experiment_id.c_str(), experiment->session_id.c_str(), experiment->scenario_id.c_str() ); 

  // construct the experiment message
  exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_SERVER );
  exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_EXPERIMENT );
  exchange.set_experiment( experiment );
  exchange.set_scenario( scenario );
  exchange.set_authorization( auth );

  // serialize the message for transmission
  exchange.build( reply );

  // broadcast the reply message back to the client
  if( _connection.write( reply ) != Reveal::Core::connection_c::ERROR_NONE ) {
    // TODO: trap and recover
  }

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
worker_c::error_e worker_c::service_trial_request( Reveal::Core::authorization_ptr auth, Reveal::Core::experiment_ptr experiment, int trial_id ) {
  printf( "trial requested scenario_id[%s], trial_id[%u]\n", experiment->scenario_id.c_str(), trial_id );

  Reveal::Core::transport_exchange_c exchange;
  Reveal::Core::trial_ptr trial;
  std::string reply;

  // query the database for trial data
  Reveal::DB::database_c::error_e db_error = _db->query( trial, experiment->scenario_id, trial_id );
  // TODO: Validation

  if( db_error == Reveal::DB::database_c::ERROR_NONE ) {
    // the query was successful

    // construct the trial message
    exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_SERVER );
    exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_TRIAL );
    exchange.set_trial( trial );
    exchange.set_experiment( experiment );
    exchange.set_authorization( auth );

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
    printf( "ERROR: Failed to find trial\n" );
    // the query returned an empty set

    // construct an error message
    exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_SERVER );
    exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_ERROR );
    exchange.set_error( Reveal::Core::transport_exchange_c::ERROR_BAD_TRIAL_REQUEST );
    exchange.set_authorization( auth );

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
worker_c::error_e worker_c::service_solution_submission( Reveal::Core::authorization_ptr auth, Reveal::Core::experiment_ptr experiment, Reveal::Core::solution_ptr solution ) {
  printf( "solution submitted\n" );

  Reveal::Core::transport_exchange_c exchange;
  std::string reply;

  // construct the solution receipt message
  exchange.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_SERVER );
  exchange.set_type( Reveal::Core::transport_exchange_c::TYPE_SOLUTION );
  exchange.set_authorization( auth );
  exchange.set_experiment( experiment );

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

  Reveal::Core::solution_ptr model;
  _db->query( model, Reveal::Core::solution_c::MODEL, solution->scenario_id, solution->trial_id );


  //if( fabs(server_solution->state.q(0) - client_solution->state.q(0)) < server_solution->epsilon.q(0) && fabs(server_solution->state.dq(0) - client_solution->state.dq(0)) < server_solution->epsilon.dq(0) ) {

//    if( fabs(server_solution->state.q(0) - client_solution->state.q(0)) < server_solution->epsilon.q(0) && fabs(server_solution->state.dq(0) - client_solution->state.dq(0)) < server_solution->epsilon.dq(0) ) {
//      printf( "Client passed pendulum trial[%d]\n", client_solution->trial_id );
//    } else {
//      printf( "Client failed pendulum trial[%d]: server(q[%f],dq[%f]:Eq[%f],Edq[%f]), client(q[%f],dq[%f])\n", client_solution->trial_id, server_solution->state.q(0), server_solution->state.dq(0), server_solution->epsilon.q(0), server_solution->epsilon.dq(0), client_solution->state.q(0), client_solution->state.dq(0) );
//    }
  //}


  // A little unsophisticated and won't handle non-conforming scenarios but 
  // for an initial development, this is the best approach
  // If the trial is the 'last' trial, run analytics.

  std::string session_id = "";
  std::string scenario_id = solution->scenario_id;

  if( solution->trial_id == 9 ) {
    printf( "here\n" );

    Reveal::Core::analysis_ptr       analysis;
    Reveal::Core::solution_set_ptr   solution_set;

    // need a temporary (for this mode only) contructor for passing database and
    // the scenario data in
    Reveal::Analytics::worker_c analytics_worker( _db, scenario_id, session_id );

    analytics_worker.query();
    analytics_worker.load();
    analytics_worker.analyze();
    //analytics_worker.insert();

  }

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------

} // namespace Server

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------
