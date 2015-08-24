#include "reveal/samples/service/worker.h"

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>

#include "reveal/core/system.h"
#include "reveal/core/authorization.h"
#include "reveal/core/user.h"
#include "reveal/core/session.h"
#include "reveal/core/pointers.h"
#include "reveal/core/digest.h"
#include "reveal/core/experiment.h"
#include "reveal/core/scenario.h"
#include "reveal/core/trial.h"
#include "reveal/core/solution.h"

#include "reveal/db/database.h"

#include "reveal/samples/exchange.h"

#include "reveal/analytics/worker.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Samples {
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
    return false;
  }

  _db = boost::shared_ptr<Reveal::DB::database_c>( new Reveal::DB::database_c() );
  if( !_db->open() ) {
    printf( "worker failed to open database\n" );
    return false;
  }

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
  Reveal::Samples::exchange_c exchange;
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
    //if( exchange.parse( request ) != Reveal::Samples::exchange_c::ERROR_NONE ) {
    if( !exchange.parse( request ) ) {
      printf( "ERROR: Failed to parse client request\n" );
      // TODO: improve error handling.  Bomb or recover here.       
    }

    // determine the course of action
    if( exchange.get_type() == Reveal::Samples::exchange_c::TYPE_HANDSHAKE ) {
      Reveal::Core::authorization_ptr auth = exchange.get_authorization();

      service_handshake_request( auth );
      // TODO : error checking

    } else if( exchange.get_type() == Reveal::Samples::exchange_c::TYPE_DIGEST ) {
      Reveal::Core::authorization_ptr auth = exchange.get_authorization();
      if( authorize( auth ) == ERROR_NONE ) {
        service_digest_request( auth );
        // TODO : error checking
      } else {
        service_failed_authorization( auth );
      }

    } else if( exchange.get_type() == Reveal::Samples::exchange_c::TYPE_EXPERIMENT ) {
      Reveal::Core::authorization_ptr auth = exchange.get_authorization();

      if( authorize( auth ) == ERROR_NONE ) {
        // extract the experiment from the client message
        Reveal::Core::experiment_ptr experiment = exchange.get_experiment();
        std::string scenario_name = experiment->scenario_id;

        //printf( "client_scenario:\n" );
        //scenario->print();

        service_experiment_request( auth, scenario_name, experiment );
        // TODO : error checking
      } else {
        service_failed_authorization( auth );
      }

    } else if( exchange.get_type() == Reveal::Samples::exchange_c::TYPE_TRIAL ) {
      Reveal::Core::authorization_ptr auth = exchange.get_authorization();
      if( authorize( auth ) == ERROR_NONE ) {
        // extract the trial from the client message
        Reveal::Core::experiment_ptr experiment = exchange.get_experiment();
        Reveal::Core::trial_ptr trial = exchange.get_trial();      

        trial->print();
        service_trial_request( auth, experiment, trial->t, experiment->epsilon );
        // TODO : error checking
      } else {
        service_failed_authorization( auth );
      }

    } else if( exchange.get_type() == Reveal::Samples::exchange_c::TYPE_SOLUTION ) {
      Reveal::Core::authorization_ptr auth = exchange.get_authorization();

      if( authorize( auth ) == ERROR_NONE ) {
        // create a solution receipt
        Reveal::Core::experiment_ptr experiment = exchange.get_experiment();
        experiment->session_id = auth->get_session();
        Reveal::Core::solution_ptr solution = exchange.get_solution();

        service_solution_submission( auth, experiment, solution );
        // TODO : error checking
      } else {
        service_failed_authorization( auth );
      }
    } else if( exchange.get_type() == Reveal::Samples::exchange_c::TYPE_ERROR ) {

    } 
  }
}

//-----------------------------------------------------------------------------
bool worker_c::is_user_valid( Reveal::Core::authorization_ptr auth, Reveal::Core::user_ptr& user ) {
  Reveal::DB::database_c::error_e db_error;

  // query the database for user data
  db_error = _db->fetch( user, auth->get_user() );

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
  db_error = _db->fetch( session, auth->get_session() );

  // Superficial validation: if the session exists in the DB, it is valid
  if( db_error == Reveal::DB::database_c::ERROR_NONE )
    return true;

  // NOTE: deep validation will require much more considerate attention
  // For example we want to avoid session hijack and we want to invalidate
  // session that have expired.

  // TODO : DEEP VALIDATION

  return false;
}
/*
//-----------------------------------------------------------------------------
bool worker_c::is_experiment_valid( Reveal::Core::authorization_ptr auth, Reveal::Core::experiment_ptr experiment_request, Reveal::Core::experiment_ptr& experiment_record ) {
//  Reveal::DB::database_c::error_e db_error;

//  // query the database for user data
//  db_error = _db->fetch( experiment_record, auth->get_session(), experiment_request->experiment_id, experiment_request->scenario_id );

//  // Superficial validation: if the session exists in the DB, it is valid
//  if( db_error == Reveal::DB::database_c::ERROR_NONE )
//    return true;

  return false;
}
*/
//-----------------------------------------------------------------------------
bool worker_c::create_session( Reveal::Core::authorization_ptr auth, Reveal::Core::session_ptr& session ) {
  Reveal::DB::database_c::error_e db_error;

  session = Reveal::Core::session_ptr( new Reveal::Core::session_c() );
  session->session_id = generate_uuid();

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

  //experiment = Reveal::Core::experiment_ptr( new Reveal::Core::experiment_c() );
  experiment->experiment_id = generate_uuid();
  experiment->session_id = auth->get_session();
  //TODO: validate that experiment values within bounds of scenario

  db_error = _db->insert( experiment );
  if( db_error == Reveal::DB::database_c::ERROR_NONE )
    return true;

  return false;
}


//-----------------------------------------------------------------------------
worker_c::error_e worker_c::authorize( Reveal::Core::authorization_ptr auth ) {
  assert( auth );
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
worker_c::error_e worker_c::service_failed_authorization( Reveal::Core::authorization_ptr auth ) {
  Reveal::Samples::exchange_c exchange;
  std::string reply;

  assert( auth );

  // construct the digest message
  exchange.set_origin( Reveal::Samples::exchange_c::ORIGIN_SERVER );
  exchange.set_type( Reveal::Samples::exchange_c::TYPE_ERROR );
  exchange.set_error( Reveal::Samples::exchange_c::ERROR_AUTHORIZATION );
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
  Reveal::Samples::exchange_c exchange;
  std::string reply;

  // construct the return handshake message
  exchange.set_origin( Reveal::Samples::exchange_c::ORIGIN_SERVER );
  exchange.set_type( Reveal::Samples::exchange_c::TYPE_HANDSHAKE );
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
  Reveal::Samples::exchange_c exchange;
  std::string reply;

  // construct the return handshake message
  exchange.set_origin( Reveal::Samples::exchange_c::ORIGIN_SERVER );
  exchange.set_type( Reveal::Samples::exchange_c::TYPE_HANDSHAKE );
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

  Reveal::Core::session_ptr session;
  Reveal::Core::user_ptr user;

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

  Reveal::Samples::exchange_c exchange;
  Reveal::Core::digest_ptr digest;
  std::string reply;

  // query the database for digest data
  Reveal::DB::database_c::error_e db_error = _db->fetch( digest );

  if( db_error == Reveal::DB::database_c::ERROR_NONE ) {
    // the query was successful

    //digest->print();      

    // construct the digest message
    exchange.set_origin( Reveal::Samples::exchange_c::ORIGIN_SERVER );
    exchange.set_type( Reveal::Samples::exchange_c::TYPE_DIGEST );
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
worker_c::error_e worker_c::service_experiment_request( Reveal::Core::authorization_ptr auth, std::string scenario_id, Reveal::Core::experiment_ptr experiment ) {
  //printf( "experiment requested\n" );
  //printf( "eps[%1.32f]\n", experiment->epsilon );

  Reveal::Samples::exchange_c exchange;
  Reveal::Core::scenario_ptr scenario;
  std::string reply;

  Reveal::DB::database_c::error_e db_error = _db->fetch( scenario, scenario_id );
  if( db_error != Reveal::DB::database_c::ERROR_NONE ) {
    // the query was unsuccessful
    return ERROR_QUERY;
  }

  if( !create_experiment( auth, scenario, experiment ) ) {
    return ERROR_CREATE;
  }

  //printf( "created experiment[%s], session[%s], scenario[%s]\n", experiment->experiment_id.c_str(), experiment->session_id.c_str(), experiment->scenario_id.c_str() ); 

  // construct the experiment message
  exchange.set_origin( Reveal::Samples::exchange_c::ORIGIN_SERVER );
  exchange.set_type( Reveal::Samples::exchange_c::TYPE_EXPERIMENT );
  exchange.set_experiment( experiment );
  exchange.set_scenario( scenario );
  exchange.set_authorization( auth );

  scenario->print();

  // serialize the message for transmission
  exchange.build( reply );

  // broadcast the reply message back to the client
  if( _connection.write( reply ) != Reveal::Core::connection_c::ERROR_NONE ) {
    // TODO: trap and recover
  }

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
worker_c::error_e worker_c::service_trial_request( Reveal::Core::authorization_ptr auth, Reveal::Core::experiment_ptr experiment, double t, double epsilon ) {

  Reveal::Samples::exchange_c exchange;
  Reveal::Core::trial_ptr trial;
  std::string reply;

  // query the database for trial data
  Reveal::DB::database_c::error_e db_error = _db->fetch( trial, experiment->scenario_id, t, epsilon );
  // TODO: Validation

  if( db_error == Reveal::DB::database_c::ERROR_NONE ) {
    // the query was successful

    // construct the trial message
    exchange.set_origin( Reveal::Samples::exchange_c::ORIGIN_SERVER );
    exchange.set_type( Reveal::Samples::exchange_c::TYPE_TRIAL );
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
    exchange.set_origin( Reveal::Samples::exchange_c::ORIGIN_SERVER );
    exchange.set_type( Reveal::Samples::exchange_c::TYPE_ERROR );
    exchange.set_error( Reveal::Samples::exchange_c::ERROR_BAD_TRIAL_REQUEST );
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
//  printf( "solution submitted\n" );

  Reveal::Samples::exchange_c exchange;
  std::string reply;

  // construct the solution receipt message
  exchange.set_origin( Reveal::Samples::exchange_c::ORIGIN_SERVER );
  exchange.set_type( Reveal::Samples::exchange_c::TYPE_SOLUTION );
  exchange.set_authorization( auth );
  exchange.set_experiment( experiment );

  //serialize the message for transmission
  exchange.build( reply );

  // broadcast the reply message back to the client
  if( _connection.write( reply ) != Reveal::Core::connection_c::ERROR_NONE ) {
    // TODO: trap and recover
  }

  // NOTE: Change in pattern from above.  Send reply lient$ 
  // the interactions necessary.  This prevents clients from waiting on data
  // transactions

  // insert the client solution into the database
  solution->experiment_id = experiment->experiment_id;
  Reveal::DB::database_c::error_e db_error = _db->insert( solution );
  if( db_error != Reveal::DB::database_c::ERROR_NONE ) {
    // TODO: trap and recover
  }

  // - Analytics -

  // A little unsophisticated and won't handle non-conforming scenarios but 
  // for an initial development, this is the best approach
  // If the trial is the 'last' trial, run analytics.

  printf( "Experiment {%s} has completed.  Starting analytics.\n", experiment->experiment_id.c_str() );

  // need a temporary (for this mode only) contructor for passing database and
  // the scenario data in
  Reveal::Analytics::worker_c analytics_worker;

  if( !analytics_worker.execute( _db, experiment->experiment_id, solution->t ) ) {
    printf( "Analytics failed to complete on Experiment {%s}\n", experiment->experiment_id.c_str() );
  } else {
    printf( "Completed Analytics on Experiment {%s}\n", experiment->experiment_id.c_str() );
  }

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
} // namespace Samples
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
