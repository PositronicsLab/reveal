#include <Reveal/transport_exchange.h>

#include <Reveal/authorization.h>
#include <Reveal/digest.h>
#include <Reveal/experiment.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>

//----------------------------------------------------------------------------

namespace Reveal {

//----------------------------------------------------------------------------

namespace Core {

//----------------------------------------------------------------------------
transport_exchange_c::transport_exchange_c( void ) {
  reset();
}

//----------------------------------------------------------------------------
transport_exchange_c::~transport_exchange_c( void ) {

}

//----------------------------------------------------------------------------
void transport_exchange_c::open( void ) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
}

//----------------------------------------------------------------------------
void transport_exchange_c::close( void ) {
  google::protobuf::ShutdownProtobufLibrary();
}

//----------------------------------------------------------------------------
void transport_exchange_c::reset( void ) {
  _origin = ORIGIN_UNDEFINED;
  _type = TYPE_UNDEFINED;
  _error = ERROR_NONE;

  _authorization = Reveal::Core::authorization_ptr();
  _digest = Reveal::Core::digest_ptr();
  _experiment = Reveal::Core::experiment_ptr();
  _scenario = Reveal::Core::scenario_ptr();
  _trial = Reveal::Core::trial_ptr();
  _solution = Reveal::Core::solution_ptr();
}

//----------------------------------------------------------------------------
void transport_exchange_c::set_origin( transport_exchange_c::origin_e origin ) {
  _origin = origin;
}

//----------------------------------------------------------------------------
transport_exchange_c::origin_e transport_exchange_c::get_origin( void ) {
  return _origin;
}

//----------------------------------------------------------------------------
void transport_exchange_c::set_error( transport_exchange_c::error_e error ) {
  _error = error;
}

//----------------------------------------------------------------------------
transport_exchange_c::error_e transport_exchange_c::get_error( void ) {
  return _error;
}

//----------------------------------------------------------------------------
void transport_exchange_c::set_type( transport_exchange_c::type_e type ) {
  _type = type;
}

//----------------------------------------------------------------------------
transport_exchange_c::type_e transport_exchange_c::get_type( void ) {
  return _type;
}

//----------------------------------------------------------------------------
void transport_exchange_c::set_authorization( authorization_ptr authorization ) {
  assert( authorization );

  _authorization = authorization;
}

//----------------------------------------------------------------------------
authorization_ptr transport_exchange_c::get_authorization( void ) {
  assert( _origin != ORIGIN_UNDEFINED );
  assert( _authorization );

  return _authorization;
}

//----------------------------------------------------------------------------
void transport_exchange_c::set_digest( digest_ptr digest ) {
  assert( digest );

  _digest = digest;
}

//----------------------------------------------------------------------------
digest_ptr transport_exchange_c::get_digest( void ) {
  assert( _origin != ORIGIN_UNDEFINED );
  assert( _error == ERROR_NONE );
  assert( _type == TYPE_DIGEST );
  assert( _digest );

  return _digest;
}

//----------------------------------------------------------------------------
void transport_exchange_c::set_experiment( experiment_ptr experiment ) {
  assert( experiment );

  _experiment = experiment;
}

//----------------------------------------------------------------------------
experiment_ptr transport_exchange_c::get_experiment( void ) {
  assert( _origin != ORIGIN_UNDEFINED );
  assert( _error == ERROR_NONE );
  assert( _experiment );

  return _experiment;
}

//----------------------------------------------------------------------------
void transport_exchange_c::set_scenario( scenario_ptr scenario ) {
  assert( scenario );

  _scenario = scenario;
}

//----------------------------------------------------------------------------
scenario_ptr transport_exchange_c::get_scenario( void ) {
  assert( _origin != ORIGIN_UNDEFINED );
  assert( _error == ERROR_NONE );
  assert( _scenario );

  return _scenario;
}

//----------------------------------------------------------------------------
void transport_exchange_c::set_trial( trial_ptr trial ) {
  assert( trial );

  _trial = trial;
}

//----------------------------------------------------------------------------
trial_ptr transport_exchange_c::get_trial( void ) {
  assert( _origin != ORIGIN_UNDEFINED );
  assert( _error == ERROR_NONE );
  assert( _type == TYPE_TRIAL );
  assert( _trial );

  return _trial;
}

//----------------------------------------------------------------------------
void transport_exchange_c::set_solution( solution_ptr solution ) {
  assert( solution );

  _solution = solution;
}

//----------------------------------------------------------------------------
solution_ptr transport_exchange_c::get_solution( void ) {
  assert( _origin == ORIGIN_CLIENT );
  assert( _error == ERROR_NONE );
  assert( _type == TYPE_SOLUTION );

  return _solution;
}

//----------------------------------------------------------------------------
transport_exchange_c::error_e transport_exchange_c::build( std::string& serialized_message ) {

  transport_exchange_c::error_e error;
  Reveal::Core::Messages::Net::Message msg = Reveal::Core::Messages::Net::Message();

  // build the authorization component of the message
  error = build_authorization( &msg );
  // trap any error from building authorization
  if( error != ERROR_NONE ) {
    _error = error;
    return error;
  }

  // - build the rest of the message -
  switch( _origin ) {
  case ORIGIN_SERVER:
    error = build_server_message( &msg );
    break;
  case ORIGIN_CLIENT:
    error = build_client_message( &msg );
    break;
  default:
    error = ERROR_BUILD;
    break;
  }
  // trap any error from building the message
  if( error != ERROR_NONE ) {
    _error = error;
    return error;
  }

  msg.SerializeToString( &serialized_message );

  return ERROR_NONE;
}

//----------------------------------------------------------------------------
transport_exchange_c::error_e transport_exchange_c::parse( const std::string& serialized_message ) {
  Reveal::Core::Messages::Net::Message msg;
  error_e error;

  // reset to clear any residual references maintained within this instance
  reset();

  // parse the serialized message into the protocol data structure
  msg.ParseFromString( serialized_message );

  // map the origin component of the message to the class instance
  error = map_origin( &msg );
  // trap any error from mapping the origin
  if( error != ERROR_NONE ) {
    _error = error;
    return error;
  }

  // map the type component of the message to the class instance
  error = map_type( &msg );
  // trap any error from mapping the type
  if( error != ERROR_NONE ) {
    _error = error;
    return error;
  }

  // map the authorization component of the message to the class instance
  error = map_authorization( &msg );
  // trap any error from mapping the authorization
  if( error != ERROR_NONE ) {
    _error = error;
    return error;
  }

  // map the rest of the message body
  switch( _origin ) {
  case ORIGIN_SERVER:
    error = map_server_message( &msg );
    break;
  case ORIGIN_CLIENT:
    error = map_client_message( &msg );
    break;
  default:
    error = ERROR_BUILD;
    break;
  }
  // trap any error from mapping the message body
  if( error != ERROR_NONE ) {
    _error = error;
    return error;
  }

  return ERROR_NONE;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
transport_exchange_c::error_e transport_exchange_c::build_authorization( Reveal::Core::Messages::Net::Message* msg ) {

  transport_exchange_c::error_e error;
  Reveal::Core::Messages::Net::Message::Header* header = msg->mutable_header();

  // - build the authorization header segment -
  Messages::Net::Authorization* msg_authorization = header->mutable_authorization();

  // map the authorization type
  Reveal::Core::authorization_c::type_e auth_type = _authorization->get_type();
  if( auth_type == Reveal::Core::authorization_c::TYPE_ANONYMOUS )
    msg_authorization->set_type( Messages::Net::Authorization::ANONYMOUS );
  else if( auth_type == Reveal::Core::authorization_c::TYPE_IDENTIFIED )
    msg_authorization->set_type( Messages::Net::Authorization::IDENTIFIED );
  else if( auth_type == Reveal::Core::authorization_c::TYPE_SESSION )
    msg_authorization->set_type( Messages::Net::Authorization::SESSION );

  // map any authorization error
  Reveal::Core::authorization_c::error_e auth_error = _authorization->get_error();
  switch( auth_error ) {
  case Reveal::Core::authorization_c::ERROR_INVALID_SESSION:
    msg_authorization->set_error( Messages::Net::Authorization::ERROR_INVALID_SESSION );
    break;
  case Reveal::Core::authorization_c::ERROR_INVALID_IDENTITY:
    msg_authorization->set_error( Messages::Net::Authorization::ERROR_INVALID_IDENTITY );
    break;
  case Reveal::Core::authorization_c::ERROR_NONE:
  default:
    msg_authorization->set_error( Messages::Net::Authorization::ERROR_NONE );
    break;
  }

  // map the user credentials
  if( auth_type == Reveal::Core::authorization_c::TYPE_IDENTIFIED ) {
    Messages::Net::Authorization::Credential* msg_user = msg_authorization->mutable_user();
    msg_user->set_id( _authorization->get_user() );
    // TODO: password when full authorization implemented
    printf( "type id'd\n" );
  }

  // map the session
  if( auth_type == Reveal::Core::authorization_c::TYPE_SESSION ) {
    Messages::Net::Authorization::Session* msg_session = msg_authorization->mutable_session();
    msg_session->set_id( _authorization->get_session() );
    printf( "session: %s\n", _authorization->get_session().c_str() );
  }
  return ERROR_NONE;
}

//----------------------------------------------------------------------------
transport_exchange_c::error_e transport_exchange_c::build_client_message( Reveal::Core::Messages::Net::Message* msg ) {

  transport_exchange_c::error_e error;
  Reveal::Core::Messages::Net::Message::Header* header = msg->mutable_header();

  header->set_origin( Messages::Net::Message::CLIENT );

  if( _type == TYPE_HANDSHAKE ) {
    header->set_type( Messages::Net::Message::HANDSHAKE );

  } else if( _type == TYPE_ERROR ) {

  } else if( _type == TYPE_DIGEST ) {
    // digest request
    header->set_type( Messages::Net::Message::DIGEST );
    header->set_error( Messages::Net::Message::ERROR_NONE );

  } else if( _type == TYPE_EXPERIMENT ) {
    // scenario request
    header->set_type( Messages::Net::Message::EXPERIMENT );
    header->set_error( Messages::Net::Message::ERROR_NONE );
    experiment_ptr experiment = get_experiment();    // accessed by method to use asserts

    Messages::Data::Experiment* msg_experiment = msg->mutable_experiment();
    msg_experiment->set_scenario_id( experiment->scenario_id );
    //msg_experiment->set_trials( experiment->number_of_trials );

  } else if( _type == TYPE_TRIAL ) {
    // trial request
    header->set_type( Messages::Net::Message::TRIAL );
    header->set_error( Messages::Net::Message::ERROR_NONE );
    trial_ptr trial = get_trial();    // accessed by method to use asserts

    experiment_ptr experiment = get_experiment();    // accessed by method to use asserts

    Messages::Data::Experiment* msg_experiment = msg->mutable_experiment();
    msg_experiment->set_scenario_id( experiment->scenario_id );

    Messages::Data::Trial* msg_trial = msg->mutable_trial();
    msg_trial->set_scenario_id( trial->scenario_id );
    msg_trial->set_trial_id( trial->trial_id );

  } else if( _type == TYPE_SOLUTION ) {
    // solution publication
    header->set_type( Messages::Net::Message::SOLUTION );
    header->set_error( Messages::Net::Message::ERROR_NONE );
    solution_ptr solution = get_solution();    // accessed by method to use asserts

    experiment_ptr experiment = get_experiment();    // accessed by method to use asserts
    Messages::Data::Experiment* msg_experiment = msg->mutable_experiment();
    msg_experiment->set_scenario_id( experiment->scenario_id );

    Messages::Data::Solution* msg_solution = msg->mutable_solution();
    msg_solution->set_scenario_id( solution->scenario_id );
    msg_solution->set_trial_id( solution->trial_id );
    msg_solution->set_t( solution->t );
    Messages::Data::State* state = msg_solution->mutable_state();
    for( unsigned i = 0; i < solution->state.size_q(); i++ ) 
      state->add_q( solution->state.q(i) );
    for( unsigned i = 0; i < solution->state.size_dq(); i++ )
      state->add_dq( solution->state.dq(i) );

  } else {
    _error = ERROR_BUILD;
    return ERROR_BUILD;
  }
  return ERROR_NONE;
}

//----------------------------------------------------------------------------
transport_exchange_c::error_e transport_exchange_c::build_server_message( Reveal::Core::Messages::Net::Message* msg ) {

  transport_exchange_c::error_e error;
  Reveal::Core::Messages::Net::Message::Header* header = msg->mutable_header();

  header->set_origin( Messages::Net::Message::SERVER );

  if( _type == TYPE_HANDSHAKE ) {
    header->set_type( Messages::Net::Message::HANDSHAKE );

  } else if( _type == TYPE_ERROR ) {
    header->set_type( Messages::Net::Message::ERROR );
    // cross reference the correct error
    error_e error = get_error();      // accessed by method to use asserts
    // TODO: map any newly defined errors
    if( error == ERROR_BAD_SCENARIO_REQUEST ) {
      header->set_error( Messages::Net::Message::ERROR_BAD_SCENARIO );
    } else if( error == ERROR_BAD_TRIAL_REQUEST ) {
      header->set_error( Messages::Net::Message::ERROR_BAD_TRIAL );
    } else {
      header->set_error( Messages::Net::Message::ERROR_GENERAL );
    }

  } else if( _type == TYPE_DIGEST ) {
    // digest response
    header->set_type( Messages::Net::Message::DIGEST );
    header->set_error( Messages::Net::Message::ERROR_NONE );
    digest_ptr digest = get_digest(); // accessed by method to use asserts

    Messages::Data::Digest* msg_digest = msg->mutable_digest();
    for( unsigned i = 0; i < digest->scenarios(); i++ ) {
      scenario_ptr scenario = digest->get_scenario( i );
      Messages::Data::Scenario* msg_scenario = msg_digest->add_scenario();
      msg_scenario->set_id( scenario->id );
      msg_scenario->set_description( scenario->description );
      msg_scenario->set_trials( scenario->trials );
      for( unsigned j = 0; j < scenario->uris.size(); j++ ) {
        msg_scenario->add_uri( scenario->uris.at(j) );
      }
    }

  } else if( _type == TYPE_EXPERIMENT ) {
    // experiment response
    header->set_type( Messages::Net::Message::EXPERIMENT );
    header->set_error( Messages::Net::Message::ERROR_NONE );
    experiment_ptr experiment = get_experiment(); // accessed by method to use asserts

    Messages::Data::Experiment* msg_experiment = msg->mutable_experiment();

    msg_experiment->set_experiment_id( experiment->experiment_id );
    msg_experiment->set_scenario_id( experiment->scenario_id );
    msg_experiment->set_trials( experiment->number_of_trials );

    scenario_ptr scenario = get_scenario(); // accessed by method to use asserts

    Messages::Data::Scenario* msg_scenario = msg->mutable_scenario();

    msg_scenario->set_id( scenario->id );
    msg_scenario->set_description( scenario->description );
    msg_scenario->set_trials( scenario->trials );
    for( unsigned j = 0; j < scenario->uris.size(); j++ ) {
      msg_scenario->add_uri( scenario->uris.at(j) );
    }

  } else if( _type == TYPE_TRIAL ) {
    // trial response
    header->set_type( Messages::Net::Message::TRIAL );
    header->set_error( Messages::Net::Message::ERROR_NONE );
    trial_ptr trial = get_trial();    // accessed by method to use asserts

    Messages::Data::Trial* msg_trial = msg->mutable_trial();
    msg_trial->set_scenario_id( trial->scenario_id );
    msg_trial->set_trial_id( trial->trial_id );
    msg_trial->set_t( trial->t );
    msg_trial->set_dt( trial->dt );
    Messages::Data::State* state = msg_trial->mutable_state();
    for( unsigned i = 0; i < trial->state.size_q(); i++ ) 
      state->add_q( trial->state.q(i) );
    for( unsigned i = 0; i < trial->state.size_dq(); i++ )
      state->add_dq( trial->state.dq(i) );
    Messages::Data::Control* control = msg_trial->mutable_control();
    for( unsigned i = 0; i < trial->control.size_u(); i++ )
      control->add_u( trial->control.u(i) );

  } else if( _type == TYPE_SOLUTION ) {
    // solution receipt
    header->set_type( Messages::Net::Message::SOLUTION );

    // TODO : More comprehensive receipt including session, experiment id,  and               trial
  } else {
    _error = ERROR_BUILD;
    return ERROR_BUILD;
  }
  return ERROR_NONE;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
transport_exchange_c::error_e transport_exchange_c::map_origin( Reveal::Core::Messages::Net::Message* msg ) {
  transport_exchange_c::error_e error;

  if( msg->header().origin() == Messages::Net::Message::SERVER ) {
    _origin = ORIGIN_SERVER;
  } else if( msg->header().origin() == Messages::Net::Message::CLIENT ) {
    _origin = ORIGIN_CLIENT;
  }
  // Note: pure else case cannot happen

  return ERROR_NONE;
}

//----------------------------------------------------------------------------
transport_exchange_c::error_e transport_exchange_c::map_type( Reveal::Core::Messages::Net::Message* msg ) {
  transport_exchange_c::error_e error;

  if( msg->header().type() == Messages::Net::Message::ERROR ) {
    _type = TYPE_ERROR;
  } else if( msg->header().type() == Messages::Net::Message::HANDSHAKE ) {
    _type = TYPE_HANDSHAKE;
  } else if( msg->header().type() == Messages::Net::Message::DIGEST ) {
    _type = TYPE_DIGEST;
  } else if( msg->header().type() == Messages::Net::Message::EXPERIMENT ) {
    _type = TYPE_EXPERIMENT;
  } else if( msg->header().type() == Messages::Net::Message::TRIAL ) {
    _type = TYPE_TRIAL;
  } else if( msg->header().type() == Messages::Net::Message::SOLUTION ) {
    _type = TYPE_SOLUTION;
  } else {
    // UNDEFINED
    // set error
    // TODO: return error technically a malformed message
  }
/*
  if( _type == TYPE_ERROR )
    // determine error and course of action
    // if( proto.header().error() == Messages::Net::Message::ERROR_? ) {
    //   _error = ERROR_?
    // } else if( ... ) {
    // }
    return _error;  // ?? Really ??  Parsing didn't fail!
  }
*/
  if( _type != TYPE_ERROR ) {
    _error = ERROR_NONE;
  }

  return ERROR_NONE;
}

//----------------------------------------------------------------------------
transport_exchange_c::error_e transport_exchange_c::map_authorization( Reveal::Core::Messages::Net::Message* msg ) {
  transport_exchange_c::error_e error;
 
  // - parse authorization -
  _authorization = Reveal::Core::authorization_ptr( new Reveal::Core::authorization_c() );

  // parse authorization type
  if( msg->header().authorization().type() == Messages::Net::Authorization::ANONYMOUS ) {
    _authorization->set_type( Reveal::Core::authorization_c::TYPE_ANONYMOUS );
  } else if( msg->header().authorization().type() == Messages::Net::Authorization::IDENTIFIED ) {
    _authorization->set_type( Reveal::Core::authorization_c::TYPE_IDENTIFIED );
  } else if( msg->header().authorization().type() == Messages::Net::Authorization::SESSION ) {
    _authorization->set_type( Reveal::Core::authorization_c::TYPE_SESSION );
  } 

  // parse authorization error
  if( msg->header().authorization().error() == Messages::Net::Authorization::ERROR_NONE ) {
    _authorization->set_error( Reveal::Core::authorization_c::ERROR_NONE );
  } else if( msg->header().authorization().error() == Messages::Net::Authorization::ERROR_INVALID_SESSION ) {
    _authorization->set_error( Reveal::Core::authorization_c::ERROR_INVALID_SESSION );
  } else if( msg->header().authorization().error() == Messages::Net::Authorization::ERROR_INVALID_IDENTITY ) {
    _authorization->set_error( Reveal::Core::authorization_c::ERROR_INVALID_IDENTITY );
  } 

  // parse user credentials
  if( _authorization->get_type() == Reveal::Core::authorization_c::TYPE_IDENTIFIED ) {
    if( msg->header().authorization().has_user() ) {
      _authorization->set_user( msg->header().authorization().user().id() );
      // TODO: update when any password is implemented
    } else {
      // TODO: ERROR : malformed message on user identity
    }
  } else if( _authorization->get_type() == Reveal::Core::authorization_c::TYPE_SESSION ) {
    if( msg->header().authorization().has_session() ) {
      _authorization->set_session( msg->header().authorization().session().id() );
    } else {
      // TODO: ERROR : malformed message session identity
    }
  }

  return ERROR_NONE;
}

//----------------------------------------------------------------------------
transport_exchange_c::error_e transport_exchange_c::map_client_message( Reveal::Core::Messages::Net::Message* msg ) {
  transport_exchange_c::error_e error;

  if( _type == TYPE_ERROR ) {
    // TODO : cross reference errors

  } else if( _type == TYPE_DIGEST ) {
    // flag as digest request.  Note: already handled in map_type 

  } else if( _type == TYPE_EXPERIMENT ) {
    if( !msg->has_experiment() ) return ERROR_PARSE;

    _experiment = experiment_ptr( new experiment_c() );
    _experiment->scenario_id = msg->experiment().scenario_id();

  } else if( _type == TYPE_TRIAL ) {
    if( !msg->has_trial() ) return ERROR_PARSE;
    _trial = trial_ptr( new trial_c() );
    _trial->scenario_id = msg->trial().scenario_id();
    _trial->trial_id = msg->trial().trial_id();

    if( !msg->has_experiment() ) return ERROR_PARSE;
    _experiment = experiment_ptr( new experiment_c() );
    _experiment->scenario_id = msg->experiment().scenario_id();

  } else if( _type == TYPE_SOLUTION ) {
    // read out the solution
    if( !msg->has_solution() ) return ERROR_PARSE;

    _solution = solution_ptr( new solution_c( Reveal::Core::solution_c::CLIENT ) );

    _solution->scenario_id = msg->solution().scenario_id();
    _solution->trial_id = msg->solution().trial_id();
    _solution->t = msg->solution().t();
    for( int i = 0; i < msg->solution().state().q_size(); i++ ) {
      _solution->state.append_q( msg->solution().state().q(i) );
    }
    for( int i = 0; i < msg->solution().state().dq_size(); i++ ) {
      _solution->state.append_dq( msg->solution().state().dq(i) );
    }

    if( !msg->has_experiment() ) return ERROR_PARSE;
    _experiment = experiment_ptr( new experiment_c() );
    _experiment->scenario_id = msg->experiment().scenario_id();

  }

  return ERROR_NONE;
}

//----------------------------------------------------------------------------
transport_exchange_c::error_e transport_exchange_c::map_server_message( Reveal::Core::Messages::Net::Message* msg ) {
  transport_exchange_c::error_e error;

  if( _type == TYPE_ERROR ) {
    // TODO : cross reference errors
    Messages::Net::Message::Error msg_error = msg->header().error();
    if( msg_error == Messages::Net::Message::ERROR_BAD_SCENARIO ) {
      _error = ERROR_BAD_SCENARIO_REQUEST;
    } else if( msg_error == Messages::Net::Message::ERROR_BAD_TRIAL ) {
      _error = ERROR_BAD_TRIAL_REQUEST;
    } else {
      // TODO:
    }
    printf( "detected error\n" );

  } else if( _type == TYPE_DIGEST ) {
    // read out the digest
    // TODO: enumerate a specific error
    if( !msg->has_digest() ) return ERROR_PARSE;

    _digest = digest_ptr( new digest_c() );
    
    for( unsigned i = 0; i < msg->digest().scenario_size(); i++ ) {
      scenario_ptr scenario = scenario_ptr( new scenario_c() );
      _digest->add_scenario( scenario );

      scenario->id = msg->digest().scenario(i).id();
      scenario->description = msg->digest().scenario(i).description();
      scenario->trials = msg->digest().scenario(i).trials();
      for( unsigned j = 0; j < msg->digest().scenario(i).uri().size(); j++ ) {
        scenario->uris.push_back( msg->digest().scenario(i).uri(j) );
      }
    }

  } else if( _type == TYPE_EXPERIMENT ) {
    // TODO: enumerate a specific error
    if( !msg->has_experiment() ) return ERROR_PARSE;

    _experiment = experiment_ptr( new experiment_c() );
    
    _experiment->experiment_id = msg->experiment().experiment_id();
    _experiment->scenario_id = msg->experiment().scenario_id();
    _experiment->number_of_trials = msg->experiment().trials();

    if( !msg->has_scenario() ) return ERROR_PARSE;

    _scenario = scenario_ptr( new scenario_c() );
    
    _scenario->id = msg->scenario().id();
    _scenario->description = msg->scenario().description();
    _scenario->trials = msg->scenario().trials();
    for( unsigned j = 0; j < msg->scenario().uri().size(); j++ ) {
      _scenario->uris.push_back( msg->scenario().uri(j) );
    }

  } else if( _type == TYPE_TRIAL ) {
    // read out the trial
    if( !msg->has_trial() ) return ERROR_PARSE;

    _trial = trial_ptr( new trial_c() );

    _trial->scenario_id = msg->trial().scenario_id();
    _trial->trial_id = msg->trial().trial_id();
    _trial->t = msg->trial().t();
    _trial->dt = msg->trial().dt();
    for( int i = 0; i < msg->trial().state().q_size(); i++ ) {
      _trial->state.append_q( msg->trial().state().q(i) );
    }
    for( int i = 0; i < msg->trial().state().dq_size(); i++ ) {
      _trial->state.append_dq( msg->trial().state().dq(i) );
    }
    for( int i = 0; i < msg->trial().control().u_size(); i++ ) {
      _trial->control.append_u( msg->trial().control().u(i) );
    }

  } else if( _type == TYPE_SOLUTION ) {
    // accept the receipt.  Note: already handled in map_type
  }

  return ERROR_NONE;
}

//----------------------------------------------------------------------------

} // namespace Core

//----------------------------------------------------------------------------

} // namespace Reveal

//----------------------------------------------------------------------------
