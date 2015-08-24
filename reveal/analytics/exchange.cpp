#include "reveal/analytics/exchange.h"

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
namespace Analytics {
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
}

//----------------------------------------------------------------------------
bool exchange_c::build( std::string& serialized_message ) {
  bool passed = false;
  Reveal::Messages::Analytics::Message msg = Reveal::Messages::Analytics::Message();

  // build the authorization component of the message
  passed = build_authorization( &msg );
  if( !passed ) return false;

  // - build the rest of the message -
  switch( _origin ) {
  case ORIGIN_ADMIN:
    passed = build_admin_message( &msg );
    printf( "built message from the admin\n" );
    break;
  case ORIGIN_WORKER:
    passed = build_worker_message( &msg );
    printf( "built message from the worker\n" );
    break;
  default:
    passed = false;
    break;
  }
  // trap any error from building the message
  if( !passed ) return false;

  msg.SerializeToString( &serialized_message );

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::parse( const std::string& serialized_message ) {
  Reveal::Messages::Analytics::Message msg;
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

  // map the request component of the message to the class instance
  passed = map_request( &msg );
  if( !passed ) return false;

  // map the response component of the message to the class instance
  passed = map_response( &msg );
  if( !passed ) return false;

  // map the command component of the message to the class instance
  passed = map_command( &msg );
  if( !passed ) return false;

  // map the authorization component of the message to the class instance
  passed = map_authorization( &msg );
  if( !passed ) return false;

  // map the rest of the message body
  switch( _origin ) {
  case ORIGIN_ADMIN:
    passed = map_admin_message( &msg );
    break;
  case ORIGIN_WORKER:
    passed = map_worker_message( &msg );
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
void exchange_c::set_request( exchange_c::request_e request ) {
  _request = request;
}

//----------------------------------------------------------------------------
exchange_c::request_e exchange_c::get_request( void ) {
  return _request;
}

//----------------------------------------------------------------------------
void exchange_c::set_response( exchange_c::response_e response ) {
  _response = response;
}

//----------------------------------------------------------------------------
exchange_c::response_e exchange_c::get_response( void ) {
  return _response;
}

//----------------------------------------------------------------------------
void exchange_c::set_command( exchange_c::command_e command ) {
  _command = command;
}

//----------------------------------------------------------------------------
exchange_c::command_e exchange_c::get_command( void ) {
  return _command;
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
//----------------------------------------------------------------------------
bool exchange_c::map_origin( Reveal::Messages::Analytics::Message* msg ) {
  //transport_exchange_c::error_e error;

  if( msg->header().origin() == Reveal::Messages::Analytics::Message::ADMIN ) {
    _origin = ORIGIN_ADMIN;
  } else if( msg->header().origin() == Reveal::Messages::Analytics::Message::WORKER ) {
    _origin = ORIGIN_WORKER;
  }
  // Note: pure else case cannot happen

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::map_type( Reveal::Messages::Analytics::Message* msg ) {

  if( msg->header().type() == Reveal::Messages::Analytics::Message::ERROR ) {
    _type = TYPE_ERROR;
  } else if( msg->header().type() == Reveal::Messages::Analytics::Message::HANDSHAKE ) {
    _type = TYPE_HANDSHAKE;
  } else if( msg->header().type() == Reveal::Messages::Analytics::Message::REQUEST ) {
    _type = TYPE_REQUEST;
  } else if( msg->header().type() == Reveal::Messages::Analytics::Message::RESPONSE ) {
    _type = TYPE_RESPONSE;
  } else if( msg->header().type() == Reveal::Messages::Analytics::Message::COMMAND ) {
    _type = TYPE_COMMAND;
  } else {
    // UNDEFINED CASE
    //return false;
  }

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::map_request( Reveal::Messages::Analytics::Message* msg ) {

  if( msg->header().request() == Reveal::Messages::Analytics::Message::REQUEST_JOB ) {
    _request = REQUEST_JOB;
  } else if( msg->header().request() == Reveal::Messages::Analytics::Message::REQUEST_RECEIPT ) {
    _request = REQUEST_RECEIPT;
  } else {
    // UNDEFINED CASE
    //return false;
  }

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::map_response( Reveal::Messages::Analytics::Message* msg ) {

  if( msg->header().response() == Reveal::Messages::Analytics::Message::RESPONSE_JOB_ISSUED ) {
    _response = RESPONSE_JOB_ISSUED;
  } else if( msg->header().response() == Reveal::Messages::Analytics::Message::RESPONSE_JOB_DENIED ) {
    _response = RESPONSE_JOB_DENIED;
  } else if( msg->header().response() == Reveal::Messages::Analytics::Message::RESPONSE_RECEIPT_AUTHORIZED ) {
    _response = RESPONSE_RECEIPT_AUTHORIZED;
  } else if( msg->header().response() == Reveal::Messages::Analytics::Message::RESPONSE_RECEIPT_REJECTED ) {
    _response = RESPONSE_RECEIPT_REJECTED;
  } else {
    // UNDEFINED CASE
    //return false;
  }

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::map_command( Reveal::Messages::Analytics::Message* msg ) {

  if( msg->header().command() == Reveal::Messages::Analytics::Message::COMMAND_EXIT ) {
    _command = COMMAND_EXIT;
  } else {
    // UNDEFINED CASE
    //return false;
  }

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::map_authorization( Reveal::Messages::Analytics::Message* msg ) {
 
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
      return false;
    }
  } else if( _authorization->get_type() == Reveal::Core::authorization_c::TYPE_SESSION ) {
    if( msg->header().authorization().has_session() ) {
      _authorization->set_session( msg->header().authorization().session().id() );
    } else {
      // TODO: ERROR : malformed message session identity
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::map_worker_message( Reveal::Messages::Analytics::Message* msg ) {
  //transport_exchange_c::error_e error;
/*
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
*/
  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::map_admin_message( Reveal::Messages::Analytics::Message* msg ) {
  //transport_exchange_c::error_e error;

  if( _type == TYPE_ERROR ) {
    // TODO : cross reference errors
    Reveal::Messages::Analytics::Message::Error msg_error = msg->header().error();
/*
    if( msg_error == Reveal::Messages::Analytics::Message::ERROR_BAD_SCENARIO ) {
      _error = ERROR_BAD_SCENARIO_REQUEST;
    } else if( msg_error == Reveal::Messages::Analytics::Message::ERROR_BAD_TRIAL ) {
      _error = ERROR_BAD_TRIAL_REQUEST;
    } else {
      // TODO:
    }
*/
    printf( "detected error\n" );
/*
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
*/
  }

  return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool exchange_c::build_authorization( Reveal::Messages::Analytics::Message* msg ) {

//  transport_exchange_c::error_e error;
  Reveal::Messages::Analytics::Message::Header* header = msg->mutable_header();

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
bool exchange_c::build_worker_message( Reveal::Messages::Analytics::Message* msg ) {

  //transport_exchange_c::error_e error;
  Reveal::Messages::Analytics::Message::Header* header = msg->mutable_header();

  header->set_origin( Reveal::Messages::Analytics::Message::WORKER );

  if( _type == TYPE_ERROR ) {
  } else if( _type == TYPE_HANDSHAKE ) {
    header->set_type( Reveal::Messages::Analytics::Message::HANDSHAKE );
  } else if( _type == TYPE_REQUEST ) {
    header->set_type( Reveal::Messages::Analytics::Message::REQUEST );
    header->set_error( Reveal::Messages::Analytics::Message::ERROR_NONE );

    // write_request( msg );

  } else if( _type == TYPE_RESPONSE ) {
    header->set_type( Reveal::Messages::Analytics::Message::RESPONSE );
    header->set_error( Reveal::Messages::Analytics::Message::ERROR_NONE );

    // write_response( msg );

  } else if( _type == TYPE_COMMAND ) {
    header->set_type( Reveal::Messages::Analytics::Message::COMMAND );
    header->set_error( Reveal::Messages::Analytics::Message::ERROR_NONE );

    // write_command( msg );

  } else {
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
bool exchange_c::build_admin_message( Reveal::Messages::Analytics::Message* msg ) {

  //transport_exchange_c::error_e error;
  Reveal::Messages::Analytics::Message::Header* header = msg->mutable_header();

  header->set_origin( Reveal::Messages::Analytics::Message::ADMIN );

  if( _type == TYPE_ERROR ) {
    error_e error = get_error();      // accessed by method to use asserts
  } else if( _type == TYPE_HANDSHAKE ) {
    header->set_type( Reveal::Messages::Analytics::Message::HANDSHAKE );
  } else if( _type == TYPE_REQUEST ) {
    header->set_type( Reveal::Messages::Analytics::Message::REQUEST );
    header->set_error( Reveal::Messages::Analytics::Message::ERROR_NONE );

    // write_request( msg );

  } else if( _type == TYPE_RESPONSE ) {
    header->set_type( Reveal::Messages::Analytics::Message::RESPONSE );
    header->set_error( Reveal::Messages::Analytics::Message::ERROR_NONE );

    // write_response( msg );

  } else if( _type == TYPE_COMMAND ) {
    header->set_type( Reveal::Messages::Analytics::Message::COMMAND );
    header->set_error( Reveal::Messages::Analytics::Message::ERROR_NONE );

    // write_command( msg );

  } else {
    return false;
  }

/*
  if( _type == TYPE_HANDSHAKE ) {
    header->set_type( Reveal::Messages::Analytics::Message::HANDSHAKE );
  } else if( _type == TYPE_ERROR ) {
    header->set_type( Reveal::Messages::Analytics::Message::ERROR );
    // cross reference the correct error
    error_e error = get_error();      // accessed by method to use asserts
    // TODO: map any newly defined errors
    if( error == ERROR_BAD_SCENARIO_REQUEST ) {
      header->set_error( Reveal::Messages::Analytics::Message::ERROR_BAD_SCENARIO );
    } else if( error == ERROR_BAD_TRIAL_REQUEST ) {
      header->set_error( Reveal::Messages::Analytics::Message::ERROR_BAD_TRIAL );
    } else {
      header->set_error( Reveal::Messages::Analytics::Message::ERROR_GENERAL );
    }

  } else if( _type == TYPE_DIGEST ) {
    // digest response
    header->set_type( Reveal::Messages::Analytics::Message::DIGEST );
    header->set_error( Reveal::Messages::Analytics::Message::ERROR_NONE );

    write_digest( msg );

  } else if( _type == TYPE_EXPERIMENT ) {
    // experiment response
    header->set_type( Reveal::Messages::Analytics::Message::EXPERIMENT );
    header->set_error( Reveal::Messages::Analytics::Message::ERROR_NONE );

    write_experiment( msg );
    write_scenario( msg );

  } else if( _type == TYPE_TRIAL ) {
    // trial response
    header->set_type( Reveal::Messages::Analytics::Message::TRIAL );
    header->set_error( Reveal::Messages::Analytics::Message::ERROR_NONE );

    write_experiment( msg );
    write_trial( msg );

  } else if( _type == TYPE_SOLUTION ) {
    // solution receipt
    header->set_type( Reveal::Messages::Analytics::Message::SOLUTION );

    // TODO : More comprehensive receipt including session, experiment id,  and               trial
  } else if( _type == TYPE_STEP ) {
    header->set_type( Reveal::Messages::Analytics::Message::STEP );
  } else if( _type == TYPE_EXIT ) {
    header->set_type( Reveal::Messages::Analytics::Message::EXIT );
  } else {
    _error = ERROR_BUILD;
    //return ERROR_BUILD;
    return false;
  }
*/
  //return ERROR_NONE;
  return true;
}



//----------------------------------------------------------------------------
} // namespace Analytics
//----------------------------------------------------------------------------
} // namespace Reveal
//----------------------------------------------------------------------------
