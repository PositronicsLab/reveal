#include "reveal/samples/exchange.h"

#include "reveal/core/authorization.h"
#include "reveal/core/digest.h"
#include "reveal/core/experiment.h"
#include "reveal/core/scenario.h"
#include "reveal/core/trial.h"
#include "reveal/core/solution.h"
#include "reveal/core/link.h"
#include "reveal/core/joint.h"

//----------------------------------------------------------------------------
namespace Reveal {
//----------------------------------------------------------------------------
namespace Samples {
//----------------------------------------------------------------------------
exchange_c::exchange_c( void ) {
  reset();
}

//----------------------------------------------------------------------------
exchange_c::~exchange_c( void ) {

}

//----------------------------------------------------------------------------
bool exchange_c::open( void ) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  return true;
}

//----------------------------------------------------------------------------
void exchange_c::close( void ) {
  google::protobuf::ShutdownProtobufLibrary();
}

//----------------------------------------------------------------------------
void exchange_c::reset( void ) {
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
bool exchange_c::build( std::string& serialized_message ) {

  bool passed = false;
  Reveal::Messages::Samples::Message msg = Reveal::Messages::Samples::Message();

  // build the authorization component of the message
  passed = build_authorization( &msg );
  if( !passed ) return false;

  // - build the rest of the message -
  switch( _origin ) {
  case ORIGIN_SERVER:
    passed = build_server_message( &msg );
    break;
  case ORIGIN_CLIENT:
    passed = build_client_message( &msg );
    break;
  default:
    passed = false;
    break;
  }
  // trap any error from building the message
  if( !passed ) {
    //_error = error;
    //return error;
    return false;
  }

  msg.SerializeToString( &serialized_message );

  //return ERROR_NONE;
  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::parse( const std::string& serialized_message ) {
  Reveal::Messages::Samples::Message msg;
  bool passed = false;

  // reset to clear any residual references maintained within this instance
  reset();

  // parse the serialized message into the protocol data structure
  msg.ParseFromString( serialized_message );

  // map the origin component of the message to the class instance
  passed = map_origin( &msg );
  if( !passed ) return false;

  // map the type component of the message to the class instance
  passed = map_type( &msg );
  if( !passed ) return false;

  // map the authorization component of the message to the class instance
  passed = map_authorization( &msg );
  if( !passed ) return false;

  // map the rest of the message body
  switch( _origin ) {
  case ORIGIN_SERVER:
    passed = map_server_message( &msg );
    break;
  case ORIGIN_CLIENT:
    passed = map_client_message( &msg );
    break;
  default:
    passed = false;
    break;
  }

  return passed;
}


//----------------------------------------------------------------------------
void exchange_c::set_origin( exchange_c::origin_e origin ) {
  _origin = origin;
}

//----------------------------------------------------------------------------
exchange_c::origin_e exchange_c::get_origin( void ) {
  return _origin;
}

//----------------------------------------------------------------------------
void exchange_c::set_error( exchange_c::error_e error ) {
  _error = error;
}

//----------------------------------------------------------------------------
exchange_c::error_e exchange_c::get_error( void ) {
  return _error;
}

//----------------------------------------------------------------------------
void exchange_c::set_type( exchange_c::type_e type ) {
  _type = type;
}

//----------------------------------------------------------------------------
exchange_c::type_e exchange_c::get_type( void ) {
  return _type;
}

//----------------------------------------------------------------------------
void exchange_c::set_authorization( Reveal::Core::authorization_ptr auth ) {
  assert( auth );

  _authorization = auth;
}

//----------------------------------------------------------------------------
Reveal::Core::authorization_ptr exchange_c::get_authorization( void ) {
  assert( _origin != ORIGIN_UNDEFINED );
  assert( _authorization );

  return _authorization;
}

//----------------------------------------------------------------------------
void exchange_c::set_digest( Reveal::Core::digest_ptr digest ) {
  assert( digest );

  _digest = digest;
}

//----------------------------------------------------------------------------
Reveal::Core::digest_ptr exchange_c::get_digest( void ) {
  assert( _origin != ORIGIN_UNDEFINED );
  assert( _error == ERROR_NONE );
  assert( _type == TYPE_DIGEST );
  assert( _digest );

  return _digest;
}

//----------------------------------------------------------------------------
void exchange_c::set_experiment( Reveal::Core::experiment_ptr experiment ) {
  assert( experiment );

  _experiment = experiment;
}

//----------------------------------------------------------------------------
Reveal::Core::experiment_ptr exchange_c::get_experiment( void ) {
  assert( _origin != ORIGIN_UNDEFINED );
  assert( _error == ERROR_NONE );
  assert( _experiment );

  return _experiment;
}

//----------------------------------------------------------------------------
void exchange_c::set_scenario( Reveal::Core::scenario_ptr scenario ) {
  assert( scenario );

  _scenario = scenario;
}

//----------------------------------------------------------------------------
Reveal::Core::scenario_ptr exchange_c::get_scenario( void ) {
  assert( _origin != ORIGIN_UNDEFINED );
  assert( _error == ERROR_NONE );
  assert( _scenario );

  return _scenario;
}

//----------------------------------------------------------------------------
void exchange_c::set_trial( Reveal::Core::trial_ptr trial ) {
  assert( trial );

  _trial = trial;
}

//----------------------------------------------------------------------------
Reveal::Core::trial_ptr exchange_c::get_trial( void ) {
  assert( _origin != ORIGIN_UNDEFINED );
  assert( _error == ERROR_NONE );
  assert( _type == TYPE_TRIAL );
  assert( _trial );

  return _trial;
}

//----------------------------------------------------------------------------
void exchange_c::set_solution( Reveal::Core::solution_ptr solution ) {
  assert( solution );

  _solution = solution;
}

//----------------------------------------------------------------------------
Reveal::Core::solution_ptr exchange_c::get_solution( void ) {
  assert( _origin == ORIGIN_CLIENT );
  assert( _error == ERROR_NONE );
  assert( _type == TYPE_SOLUTION );

  return _solution;
}
//----------------------------------------------------------------------------
bool exchange_c::build_server_experiment( std::string& msg, Reveal::Core::authorization_ptr auth, Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr experiment ) {
  reset();
  set_authorization( auth );
  set_origin( Reveal::Samples::exchange_c::ORIGIN_SERVER );
  set_type( Reveal::Samples::exchange_c::TYPE_EXPERIMENT );
  set_scenario( scenario );
  set_experiment( experiment );
  return build( msg );
}

//----------------------------------------------------------------------------
bool exchange_c::build_server_trial( std::string& msg, Reveal::Core::authorization_ptr auth, Reveal::Core::experiment_ptr experiment, Reveal::Core::trial_ptr trial ) {
  reset();
  set_authorization( auth );
  set_origin( Reveal::Samples::exchange_c::ORIGIN_SERVER );
  set_type( Reveal::Samples::exchange_c::TYPE_TRIAL );
  set_experiment( experiment );
  set_trial( trial );
  return build( msg );
}

//----------------------------------------------------------------------------
bool exchange_c::build_client_solution( std::string& msg, Reveal::Core::authorization_ptr auth, Reveal::Core::experiment_ptr experiment, Reveal::Core::solution_ptr solution ) {
  reset();
  set_authorization( auth );
  set_origin( Reveal::Samples::exchange_c::ORIGIN_CLIENT );
  set_type( Reveal::Samples::exchange_c::TYPE_SOLUTION );
  set_experiment( experiment );
  set_solution( solution );
  return build( msg );
}

//----------------------------------------------------------------------------
bool exchange_c::build_server_command_step( std::string& msg, Reveal::Core::authorization_ptr auth ) {
  reset();
  set_authorization( auth );
  set_origin( Reveal::Samples::exchange_c::ORIGIN_SERVER );
  set_type( Reveal::Samples::exchange_c::TYPE_STEP );
  return build( msg );
}

//----------------------------------------------------------------------------
bool exchange_c::build_server_command_exit( std::string& msg, Reveal::Core::authorization_ptr auth ) {
  reset();
  set_authorization( auth );
  set_origin( Reveal::Samples::exchange_c::ORIGIN_SERVER );
  set_type( Reveal::Samples::exchange_c::TYPE_EXIT );
  return build( msg );
}

//----------------------------------------------------------------------------
bool exchange_c::parse_server_experiment( const std::string& msg, Reveal::Core::authorization_ptr& auth, Reveal::Core::scenario_ptr& scenario, Reveal::Core::experiment_ptr& experiment ) {

  //error_e ex_err = parse( msg );
  //if( ex_err != ERROR_NONE ) return ex_err;
  if( !parse(msg) ) return false;

  auth = get_authorization();
  scenario = get_scenario(); 
  experiment = get_experiment();

  //return ERROR_NONE;
  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::parse_client_solution( const std::string& msg, Reveal::Core::authorization_ptr& auth, Reveal::Core::experiment_ptr& experiment, Reveal::Core::solution_ptr& solution ) {

  //error_e ex_err = parse( msg );
  //if( ex_err != ERROR_NONE ) return ex_err;
  if( !parse(msg) ) return false;

  auth = get_authorization();
  experiment = get_experiment();
  solution = get_solution(); 
  solution->experiment_id = experiment->experiment_id;

  //return ERROR_NONE;
  return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool exchange_c::build_authorization( Reveal::Messages::Samples::Message* msg ) {

//  transport_exchange_c::error_e error;
  Reveal::Messages::Samples::Message::Header* header = msg->mutable_header();

  // - build the authorization header segment -
  Reveal::Messages::Net::Authorization* msg_authorization = header->mutable_authorization();

  // map the authorization type
  Reveal::Core::authorization_c::type_e auth_type = _authorization->get_type();
  if( auth_type == Reveal::Core::authorization_c::TYPE_ANONYMOUS )
    msg_authorization->set_type( Reveal::Messages::Net::Authorization::ANONYMOUS );
  else if( auth_type == Reveal::Core::authorization_c::TYPE_IDENTIFIED )
    msg_authorization->set_type( Reveal::Messages::Net::Authorization::IDENTIFIED );
  else if( auth_type == Reveal::Core::authorization_c::TYPE_SESSION )
    msg_authorization->set_type( Reveal::Messages::Net::Authorization::SESSION );

  // map any authorization error
  Reveal::Core::authorization_c::error_e auth_error = _authorization->get_error();
  switch( auth_error ) {
  case Reveal::Core::authorization_c::ERROR_INVALID_SESSION:
    msg_authorization->set_error( Reveal::Messages::Net::Authorization::ERROR_INVALID_SESSION );
    break;
  case Reveal::Core::authorization_c::ERROR_INVALID_IDENTITY:
    msg_authorization->set_error( Reveal::Messages::Net::Authorization::ERROR_INVALID_IDENTITY );
    break;
  case Reveal::Core::authorization_c::ERROR_NONE:
  default:
    msg_authorization->set_error( Reveal::Messages::Net::Authorization::ERROR_NONE );
    break;
  }

  // map the user credentials
  if( auth_type == Reveal::Core::authorization_c::TYPE_IDENTIFIED ) {
    Reveal::Messages::Net::Authorization::Credential* msg_user = msg_authorization->mutable_user();
    msg_user->set_id( _authorization->get_user() );
    // TODO: password when full authorization implemented
//    printf( "type id'd\n" );
  }

  // map the session
  if( auth_type == Reveal::Core::authorization_c::TYPE_SESSION ) {
    Reveal::Messages::Net::Authorization::Session* msg_session = msg_authorization->mutable_session();
    msg_session->set_id( _authorization->get_session() );
//    printf( "session: %s\n", _authorization->get_session().c_str() );
  }

  //return ERROR_NONE;
  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::build_client_message( Reveal::Messages::Samples::Message* msg ) {

  //transport_exchange_c::error_e error;
  Reveal::Messages::Samples::Message::Header* header = msg->mutable_header();

  header->set_origin( Reveal::Messages::Samples::Message::CLIENT );

  if( _type == TYPE_HANDSHAKE ) {
    header->set_type( Reveal::Messages::Samples::Message::HANDSHAKE );

  } else if( _type == TYPE_ERROR ) {

  } else if( _type == TYPE_DIGEST ) {
    // digest request
    header->set_type( Reveal::Messages::Samples::Message::DIGEST );
    header->set_error( Reveal::Messages::Samples::Message::ERROR_NONE );

  } else if( _type == TYPE_EXPERIMENT ) {
    // scenario request
    header->set_type( Reveal::Messages::Samples::Message::EXPERIMENT );
    header->set_error( Reveal::Messages::Samples::Message::ERROR_NONE );

    write_experiment( msg );

  } else if( _type == TYPE_TRIAL ) {
    // trial request
    header->set_type( Reveal::Messages::Samples::Message::TRIAL );
    header->set_error( Reveal::Messages::Samples::Message::ERROR_NONE );

    write_experiment( msg );
    write_trial( msg );

  } else if( _type == TYPE_SOLUTION ) {
    // solution publication
    header->set_type( Reveal::Messages::Samples::Message::SOLUTION );
    header->set_error( Reveal::Messages::Samples::Message::ERROR_NONE );

    write_experiment( msg );
    write_solution( msg );

  } else {
    _error = ERROR_BUILD;
    //return ERROR_BUILD;
    return false;
  }

  //return ERROR_NONE;
  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::build_server_message( Reveal::Messages::Samples::Message* msg ) {

  //transport_exchange_c::error_e error;
  Reveal::Messages::Samples::Message::Header* header = msg->mutable_header();

  header->set_origin( Reveal::Messages::Samples::Message::SERVER );

  if( _type == TYPE_HANDSHAKE ) {
    header->set_type( Reveal::Messages::Samples::Message::HANDSHAKE );

  } else if( _type == TYPE_ERROR ) {
    header->set_type( Reveal::Messages::Samples::Message::ERROR );
    // cross reference the correct error
    error_e error = get_error();      // accessed by method to use asserts
    // TODO: map any newly defined errors
    if( error == ERROR_BAD_SCENARIO_REQUEST ) {
      header->set_error( Reveal::Messages::Samples::Message::ERROR_BAD_SCENARIO );
    } else if( error == ERROR_BAD_TRIAL_REQUEST ) {
      header->set_error( Reveal::Messages::Samples::Message::ERROR_BAD_TRIAL );
    } else {
      header->set_error( Reveal::Messages::Samples::Message::ERROR_GENERAL );
    }

  } else if( _type == TYPE_DIGEST ) {
    // digest response
    header->set_type( Reveal::Messages::Samples::Message::DIGEST );
    header->set_error( Reveal::Messages::Samples::Message::ERROR_NONE );

    write_digest( msg );

  } else if( _type == TYPE_EXPERIMENT ) {
    // experiment response
    header->set_type( Reveal::Messages::Samples::Message::EXPERIMENT );
    header->set_error( Reveal::Messages::Samples::Message::ERROR_NONE );

    write_experiment( msg );
    write_scenario( msg );

  } else if( _type == TYPE_TRIAL ) {
    // trial response
    header->set_type( Reveal::Messages::Samples::Message::TRIAL );
    header->set_error( Reveal::Messages::Samples::Message::ERROR_NONE );

    write_experiment( msg );
    write_trial( msg );

  } else if( _type == TYPE_SOLUTION ) {
    // solution receipt
    header->set_type( Reveal::Messages::Samples::Message::SOLUTION );

    // TODO : More comprehensive receipt including session, experiment id,  and               trial
  } else if( _type == TYPE_STEP ) {
    header->set_type( Reveal::Messages::Samples::Message::STEP );
  } else if( _type == TYPE_EXIT ) {
    header->set_type( Reveal::Messages::Samples::Message::EXIT );
  } else {
    _error = ERROR_BUILD;
    //return ERROR_BUILD;
    return false;
  }

  //return ERROR_NONE;
  return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool exchange_c::map_origin( Reveal::Messages::Samples::Message* msg ) {
  //transport_exchange_c::error_e error;

  if( msg->header().origin() == Reveal::Messages::Samples::Message::SERVER ) {
    _origin = ORIGIN_SERVER;
  } else if( msg->header().origin() == Reveal::Messages::Samples::Message::CLIENT ) {
    _origin = ORIGIN_CLIENT;
  }
  // Note: pure else case cannot happen

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::map_type( Reveal::Messages::Samples::Message* msg ) {

  if( msg->header().type() == Reveal::Messages::Samples::Message::ERROR ) {
    _type = TYPE_ERROR;
  } else if( msg->header().type() == Reveal::Messages::Samples::Message::HANDSHAKE ) {
    _type = TYPE_HANDSHAKE;
  } else if( msg->header().type() == Reveal::Messages::Samples::Message::DIGEST ) {
    _type = TYPE_DIGEST;
  } else if( msg->header().type() == Reveal::Messages::Samples::Message::EXPERIMENT ) {
    _type = TYPE_EXPERIMENT;
  } else if( msg->header().type() == Reveal::Messages::Samples::Message::TRIAL ) {
    _type = TYPE_TRIAL;
  } else if( msg->header().type() == Reveal::Messages::Samples::Message::SOLUTION ) {
    _type = TYPE_SOLUTION;
  } else if( msg->header().type() == Reveal::Messages::Samples::Message::STEP ) {
    _type = TYPE_STEP;
  } else if( msg->header().type() == Reveal::Messages::Samples::Message::EXIT ) {
    _type = TYPE_EXIT;
  } else {
    // UNDEFINED
    // set error
    // TODO: return error technically a malformed message
  }
/*
  if( _type == TYPE_ERROR )
    // determine error and course of action
    // if( proto.header().error() == Messages::Samples::Message::ERROR_? ) {
    //   _error = ERROR_?
    // } else if( ... ) {
    // }
    return _error;  // ?? Really ??  Parsing didn't fail!
  }
*/
  if( _type != TYPE_ERROR ) {
    _error = ERROR_NONE;
  }

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::map_authorization( Reveal::Messages::Samples::Message* msg ) {
  //transport_exchange_c::error_e error;
 
  // - parse authorization -
  _authorization = Reveal::Core::authorization_ptr( new Reveal::Core::authorization_c() );

  // parse authorization type
  if( msg->header().authorization().type() == Reveal::Messages::Net::Authorization::ANONYMOUS ) {
    _authorization->set_type( Reveal::Core::authorization_c::TYPE_ANONYMOUS );
  } else if( msg->header().authorization().type() == Reveal::Messages::Net::Authorization::IDENTIFIED ) {
    _authorization->set_type( Reveal::Core::authorization_c::TYPE_IDENTIFIED );
  } else if( msg->header().authorization().type() == Reveal::Messages::Net::Authorization::SESSION ) {
    _authorization->set_type( Reveal::Core::authorization_c::TYPE_SESSION );
  } 

  // parse authorization error
  if( msg->header().authorization().error() == Reveal::Messages::Net::Authorization::ERROR_NONE ) {
    _authorization->set_error( Reveal::Core::authorization_c::ERROR_NONE );
  } else if( msg->header().authorization().error() == Reveal::Messages::Net::Authorization::ERROR_INVALID_SESSION ) {
    _authorization->set_error( Reveal::Core::authorization_c::ERROR_INVALID_SESSION );
  } else if( msg->header().authorization().error() == Reveal::Messages::Net::Authorization::ERROR_INVALID_IDENTITY ) {
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

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::map_client_message( Reveal::Messages::Samples::Message* msg ) {
  //transport_exchange_c::error_e error;

  if( _type == TYPE_ERROR ) {
    // TODO : cross reference errors

  } else if( _type == TYPE_DIGEST ) {
    // flag as digest request.  Note: already handled in map_type 

  } else if( _type == TYPE_EXPERIMENT ) {

    if( !msg->has_experiment() ) return ERROR_PARSE;
    read_experiment( msg );

  } else if( _type == TYPE_TRIAL ) {

    if( !msg->has_trial() ) return ERROR_PARSE;
    read_trial( msg );

    if( !msg->has_experiment() ) return ERROR_PARSE;
    read_experiment( msg );

  } else if( _type == TYPE_SOLUTION ) {

    // read out the solution
    if( !msg->has_solution() ) return ERROR_PARSE;
    read_solution( msg );

    if( !msg->has_experiment() ) return ERROR_PARSE;
    read_experiment( msg );
  }

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::map_server_message( Reveal::Messages::Samples::Message* msg ) {
  //transport_exchange_c::error_e error;

  if( _type == TYPE_ERROR ) {
    // TODO : cross reference errors
    Reveal::Messages::Samples::Message::Error msg_error = msg->header().error();
    if( msg_error == Reveal::Messages::Samples::Message::ERROR_BAD_SCENARIO ) {
      _error = ERROR_BAD_SCENARIO_REQUEST;
    } else if( msg_error == Reveal::Messages::Samples::Message::ERROR_BAD_TRIAL ) {
      _error = ERROR_BAD_TRIAL_REQUEST;
    } else {
      // TODO:
    }
    printf( "detected error\n" );

  } else if( _type == TYPE_DIGEST ) {

    // read out the digest
    // TODO: enumerate a specific error
    if( !msg->has_digest() ) return ERROR_PARSE;
    read_digest( msg );

  } else if( _type == TYPE_EXPERIMENT ) {

    // TODO: enumerate a specific error
    if( !msg->has_experiment() ) return ERROR_PARSE;
    read_experiment( msg );

    if( !msg->has_scenario() ) return ERROR_PARSE;
    read_scenario( msg );

  } else if( _type == TYPE_TRIAL ) {

    // read out the trial
    if( !msg->has_trial() ) return ERROR_PARSE;
    read_trial( msg );

  } else if( _type == TYPE_SOLUTION ) {
    // accept the receipt.  Note: already handled in map_type
  } else if( _type == TYPE_STEP ) {
    // accept the command.  Note: already handled in map_type
  } else if( _type == TYPE_EXIT ) {
    // accept the command.  Note: already handled in map_type
  }

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::write_digest( Reveal::Messages::Samples::Message* msg ) {

  Reveal::Core::digest_ptr digest = get_digest(); // accessed by method to use asserts

  Reveal::Messages::Data::Digest* msg_digest = msg->mutable_digest();
  for( unsigned i = 0; i < digest->scenarios(); i++ ) {
    Reveal::Core::scenario_ptr scenario = digest->get_scenario( i );
    Reveal::Messages::Data::Scenario* msg_scenario = msg_digest->add_scenario();
    msg_scenario->set_id( scenario->id );
    msg_scenario->set_package_id( scenario->package_id );
    msg_scenario->set_description( scenario->description );
    msg_scenario->set_sample_rate( scenario->sample_rate );
    msg_scenario->set_sample_start_time( scenario->sample_start_time );
    msg_scenario->set_sample_end_time( scenario->sample_end_time );
    //msg_scenario->set_trials( scenario->trials );
    for( unsigned j = 0; j < scenario->uris.size(); j++ ) {
      msg_scenario->add_uri( scenario->uris.at(j) );
    }
  }

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::read_digest( Reveal::Messages::Samples::Message* msg ) {

  _digest = Reveal::Core::digest_ptr( new Reveal::Core::digest_c() );
  
  for( int i = 0; i < msg->digest().scenario_size(); i++ ) {
    Reveal::Core::scenario_ptr scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );
    _digest->add_scenario( scenario );

    scenario->id = msg->digest().scenario(i).id();
    scenario->package_id = msg->digest().scenario(i).package_id();
    scenario->description = msg->digest().scenario(i).description();
    scenario->sample_rate = msg->digest().scenario(i).sample_rate();
    scenario->sample_start_time = msg->digest().scenario(i).sample_start_time();
    scenario->sample_end_time = msg->digest().scenario(i).sample_end_time();
    //scenario->trials = msg->digest().scenario(i).trials();
    for( int j = 0; j < msg->digest().scenario(i).uri().size(); j++ ) {
      scenario->uris.push_back( msg->digest().scenario(i).uri(j) );
    }
  }

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::write_scenario( Reveal::Messages::Samples::Message* msg ) {

  Reveal::Core::scenario_ptr scenario = get_scenario(); // accessed by method to use asserts

  Reveal::Messages::Data::Scenario* msg_scenario = msg->mutable_scenario();

  msg_scenario->set_id( scenario->id );
  msg_scenario->set_package_id( scenario->package_id );
  msg_scenario->set_description( scenario->description );
  msg_scenario->set_sample_rate( scenario->sample_rate );
  msg_scenario->set_sample_start_time( scenario->sample_start_time );
  msg_scenario->set_sample_end_time( scenario->sample_end_time );
  //msg_scenario->set_trials( scenario->trials );
  for( unsigned j = 0; j < scenario->uris.size(); j++ ) {
    msg_scenario->add_uri( scenario->uris.at(j) );
  }

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::read_scenario( Reveal::Messages::Samples::Message* msg ) {

  _scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );
  
  _scenario->id = msg->scenario().id();
  _scenario->package_id = msg->scenario().package_id();
  _scenario->description = msg->scenario().description();
  _scenario->sample_rate = msg->scenario().sample_rate();
  _scenario->sample_start_time = msg->scenario().sample_start_time();
  _scenario->sample_end_time = msg->scenario().sample_end_time();
  //_scenario->trials = msg->scenario().trials();
  for( int j = 0; j < msg->scenario().uri().size(); j++ ) {
    _scenario->uris.push_back( msg->scenario().uri(j) );
  }

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::write_experiment( Reveal::Messages::Samples::Message* msg ) {
  Reveal::Core::experiment_ptr experiment = get_experiment(); // accessed by method to use asserts
 Reveal::Messages::Data::Experiment* msg_experiment = msg->mutable_experiment();

  msg_experiment->set_experiment_id( experiment->experiment_id );
  msg_experiment->set_scenario_id( experiment->scenario_id );
  msg_experiment->set_start_time( experiment->start_time );
  msg_experiment->set_end_time( experiment->end_time );
  msg_experiment->set_time_step( experiment->time_step );
  msg_experiment->set_epsilon( experiment->epsilon );
  msg_experiment->set_intermediate_trials( experiment->intermediate_trials_to_ignore );

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::read_experiment( Reveal::Messages::Samples::Message* msg ) {
  _experiment = Reveal::Core::experiment_ptr( new Reveal::Core::experiment_c() );
    
  _experiment->experiment_id = msg->experiment().experiment_id();
  _experiment->scenario_id = msg->experiment().scenario_id();
  _experiment->start_time = msg->experiment().start_time();
  _experiment->end_time = msg->experiment().end_time();
  _experiment->time_step = msg->experiment().time_step();
  _experiment->epsilon = msg->experiment().epsilon();
  _experiment->intermediate_trials_to_ignore = msg->experiment().intermediate_trials();

  Reveal::Core::authorization_ptr auth = get_authorization();
  _experiment->session_id = auth->get_session();

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::write_trial( Reveal::Messages::Samples::Message* msg ) {

  Reveal::Core::trial_ptr trial = get_trial();    // accessed by method to use asserts

  Reveal::Messages::Data::Trial* msg_trial = msg->mutable_trial();
  msg_trial->set_scenario_id( trial->scenario_id );
  msg_trial->set_t( trial->t );

  for( unsigned i = 0; i < trial->models.size(); i++ ) {
    Reveal::Core::model_ptr model = trial->models[i];
    Reveal::Messages::Data::Model* msg_model = msg_trial->add_model();

    msg_model->set_id( model->id );
    for( unsigned j = 0; j < model->links.size(); j++ ) {
      Reveal::Core::link_ptr link = model->links[j];
      Reveal::Messages::Data::Link* msg_link = msg_model->add_link();

      msg_link->set_id( link->id );

      Reveal::Messages::Data::State* state = msg_link->mutable_state();
      for( unsigned k = 0; k < link->state.size_q(); k++ ) 
        state->add_q( link->state.q(k) );
      for( unsigned k = 0; k < link->state.size_dq(); k++ )
        state->add_dq( link->state.dq(k) );    
    }
    for( unsigned j = 0; j < model->joints.size(); j++ ) {
      Reveal::Core::joint_ptr joint = model->joints[j];
      Reveal::Messages::Data::Joint* msg_joint = msg_model->add_joint();

      msg_joint->set_id( joint->id );

      Reveal::Messages::Data::Control* control = msg_joint->mutable_control();
      for( unsigned k = 0; k < joint->control.size_u(); k++ ) 
        control->add_u( joint->control.u(k) );
    }
  }

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::read_trial( Reveal::Messages::Samples::Message* msg ) {

  _trial = Reveal::Core::trial_ptr( new Reveal::Core::trial_c() );

  _trial->scenario_id = msg->trial().scenario_id();
  _trial->t = msg->trial().t();
  for( int i = 0; i < msg->trial().model_size(); i++ ) {
    Reveal::Core::model_ptr model = Reveal::Core::model_ptr( new Reveal::Core::model_c() );
    Reveal::Messages::Data::Model msg_model = msg->trial().model(i);
    
    model->id = msg_model.id();

    for( int j = 0; j < msg_model.link_size(); j++ ) {
      Reveal::Core::link_ptr link = Reveal::Core::link_ptr( new Reveal::Core::link_c() );
      Reveal::Messages::Data::Link msg_link = msg_model.link(j);
      link->id = msg_link.id();

      for( int k = 0; k < msg_link.state().q_size(); k++ ) {
        link->state.q( k, msg_link.state().q(k) );
      }
      for( int k = 0; k < msg_link.state().dq_size(); k++ ) {
        link->state.dq( k, msg_link.state().dq(k) );
      }

      model->links.push_back( link );
    }

    for( int j = 0; j < msg_model.joint_size(); j++ ) {
      Reveal::Core::joint_ptr joint = Reveal::Core::joint_ptr( new Reveal::Core::joint_c() );
      Reveal::Messages::Data::Joint msg_joint = msg_model.joint(j);
      joint->id = msg_joint.id();

      for( int k = 0; k < msg_joint.control().u_size(); k++ ) {
        joint->control.u( k, msg_joint.control().u(k) );
      }

      model->joints.push_back( joint );
    }
    _trial->models.push_back( model );
  }

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::write_solution( Reveal::Messages::Samples::Message* msg ) {

  Reveal::Core::solution_ptr solution = get_solution();    // accessed by method to use asserts
  Reveal::Messages::Data::Solution* msg_solution = msg->mutable_solution();
  msg_solution->set_scenario_id( solution->scenario_id );
  msg_solution->set_t( solution->t );
  msg_solution->set_real_time( solution->real_time );

  for( unsigned i = 0; i < solution->models.size(); i++ ) {
    Reveal::Core::model_ptr model = solution->models[i];
    Reveal::Messages::Data::Model* msg_model = msg_solution->add_model();

    msg_model->set_id( model->id );
    for( unsigned j = 0; j < model->links.size(); j++ ) {
      Reveal::Core::link_ptr link = model->links[j];
      Reveal::Messages::Data::Link* msg_link = msg_model->add_link();

      msg_link->set_id( link->id );

      Reveal::Messages::Data::State* state = msg_link->mutable_state();
      for( unsigned k = 0; k < link->state.size_q(); k++ ) 
        state->add_q( link->state.q(k) );
      for( unsigned k = 0; k < link->state.size_dq(); k++ )
        state->add_dq( link->state.dq(k) );    
    }
  }

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::read_solution( Reveal::Messages::Samples::Message* msg ) {

  _solution = Reveal::Core::solution_ptr( new Reveal::Core::solution_c( Reveal::Core::solution_c::CLIENT ) );

  _solution->scenario_id = msg->solution().scenario_id();
  _solution->t = msg->solution().t();
  _solution->real_time = msg->solution().real_time();

  for( int i = 0; i < msg->solution().model_size(); i++ ) {
    Reveal::Core::model_ptr model = Reveal::Core::model_ptr( new Reveal::Core::model_c() );
    Reveal::Messages::Data::Model msg_model = msg->solution().model(i);
    
    model->id = msg_model.id();

    for( int j = 0; j < msg_model.link_size(); j++ ) {
      Reveal::Core::link_ptr link = Reveal::Core::link_ptr( new Reveal::Core::link_c() );
      Reveal::Messages::Data::Link msg_link = msg_model.link(j);
      link->id = msg_link.id();

      for( int k = 0; k < msg_link.state().q_size(); k++ ) {
        link->state.q( k, msg_link.state().q(k) );
      }
      for( int k = 0; k < msg_link.state().dq_size(); k++ ) {
        link->state.dq( k, msg_link.state().dq(k) );
      }

      model->links.push_back( link );
    }

    _solution->models.push_back( model );
  }

  return true;
}

//----------------------------------------------------------------------------
} // namespace Samples
//----------------------------------------------------------------------------
} // namespace Reveal
//----------------------------------------------------------------------------
