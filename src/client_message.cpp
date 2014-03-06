#include <Reveal/client_message.h>

#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>

//-----------------------------------------------------------------------------
namespace Reveal {

//-----------------------------------------------------------------------------
/// Default Constructor
ClientMessage::ClientMessage( void ) {
  _type = UNDEFINED;
  _request = Messages::Net::ClientRequest();
}

//-----------------------------------------------------------------------------
/// Destructor
ClientMessage::~ClientMessage( void ) {

}

//-----------------------------------------------------------------------------
/// Parses a serialized protobuffer ClientRequest message string into the 
/// protobuf member and returns an error if the message fails to meet the 
/// required criteria
bool ClientMessage::Parse( const std::string& serial ) {

  // clear any old request in the message member
  _request.Clear();

  // parse the serialized message into the protobuf structure
  if( !_request.ParseFromString( serial ) ) {
    // TODO: send to error log instead of printing
    printf( "ERROR: Failed to Parse" );
    return false;
  }

  // determine the type of client request
  switch( _request.type() ) {
  case Messages::Net::ClientRequest::REQUEST_SCENARIO:
    _type = SCENARIO;
    break;
  case Messages::Net::ClientRequest::REQUEST_TRIAL:
    _type = TRIAL;
    break;
  case Messages::Net::ClientRequest::REQUEST_SOLUTION:
    _type = SOLUTION;
    break;
  case Messages::Net::ClientRequest::REQUEST_ERROR:
    _type = ERROR;
    break;
  case Messages::Net::ClientRequest::REQUEST_UNDEFINED:
  default:
    _type = UNDEFINED;
    return false;               // bad message. Failed to parse
  }
 
  // validate that the message has the necessary optional structures if it is 
  // defined as a given type
  if( _type == SCENARIO ) {
    if( !_request.has_scenario() ) {
      // Error.  malformed message
      return false;
    }
  } else if( _type == TRIAL ) {
    if( !_request.has_trial() ) {
      // Error.  malformed message
      return false;
    }
  } else if( _type == SOLUTION ) {
    if( !_request.has_solution() ) {
      // Error.  malformed message
      return false;
    }
  } else if( _type == ERROR ) {
    // TODO: update with deeper error checking
  }
  
  // message may still not be fully validated, but it does fulfill the most
  // basic requirements to be classified by Type
  return true;
}

//-----------------------------------------------------------------------------
/// Serializes the protobuf member into a string for transmission through the 
/// transport layer
std::string ClientMessage::Serialize( void ) {
  std::string serial;

  _request.SerializeToString( &serial );
 
  return serial;
}

//-----------------------------------------------------------------------------
/// Returns the Type of message after it has been parsed or set
ClientMessage::Type ClientMessage::getType( void ) {
  return _type;
}

//-----------------------------------------------------------------------------
/// Returns the scenario data attached to the message.
ScenarioPtr ClientMessage::getScenario( void ) {
  assert( _type == SCENARIO );

  ScenarioPtr scenario = ScenarioPtr( new Scenario() );
  scenario->name = _request.scenario().name();

  return scenario;
}

//-----------------------------------------------------------------------------
/// Sets the protobuf message as a scenario and maps parameters from a scenario
/// data structure into the protobuffer
void ClientMessage::setScenario( ScenarioPtr scenario ) {
  _type = SCENARIO;

  _request.set_type( Messages::Net::ClientRequest::REQUEST_SCENARIO );
  Messages::Net::ScenarioRequest* sr = _request.mutable_scenario();
  sr->set_name( scenario->name.c_str() );
}

//-----------------------------------------------------------------------------
/// Returns the trial data attached to the message
TrialPtr ClientMessage::getTrial( void ) {
  assert( _type == TRIAL );
 
  TrialPtr trial = TrialPtr( new Trial() );

  trial->scenario = _request.trial().scenario();
  trial->index = _request.trial().index();

  return trial;
}

//-----------------------------------------------------------------------------
/// Sets the protobuf message as a trial and maps parameters from a trial
/// data structure into the protobuffer
void ClientMessage::setTrial( TrialPtr trial ) {
  _type = TRIAL;

  _request.Clear();
  _request.set_type( Messages::Net::ClientRequest::REQUEST_TRIAL );
  Messages::Net::TrialRequest* tr = _request.mutable_trial();
  tr->set_scenario( trial->scenario.c_str() );
  tr->set_index( trial->index );
}

//-----------------------------------------------------------------------------
/// Returns the solution data attached to the message
SolutionPtr ClientMessage::getSolution( void ) {
  assert( _type == SOLUTION );

  SolutionPtr solution = SolutionPtr( new Solution() );

  solution->scenario = _request.solution().scenario();
  solution->index = _request.solution().index();
  solution->t = _request.solution().t();

  for( int i = 0; i < _request.solution().state().q_size(); i++ ) {
    solution->state.Append_q( _request.solution().state().q(i) );
  }
  for( int i = 0; i < _request.solution().state().dq_size(); i++ ) {
    solution->state.Append_dq( _request.solution().state().dq(i) );
  }

  return solution;
}

//-----------------------------------------------------------------------------
/// Sets the protobuf message as a solution and maps parameters from a solution
/// data structure into the protobuffer
void ClientMessage::setSolution( SolutionPtr solution ) {
  _type = SOLUTION;

  _request.set_type( Messages::Net::ClientRequest::REQUEST_SOLUTION );
  Messages::Net::SolutionRequest* sr = _request.mutable_solution();
  sr->set_scenario( solution->scenario.c_str() );
  sr->set_index( solution->index );
  sr->set_t( solution->t );
  Messages::Net::State* state = sr->mutable_state();
  for( unsigned i = 0; i < solution->state.Size_q(); i++ ) {
    state->add_q( solution->state.q(i) );
  }
  for( unsigned i = 0; i < solution->state.Size_dq(); i++ ) {
    state->add_dq( solution->state.dq(i) );
  }
}

//-----------------------------------------------------------------------------
/// Returns the error that was transmitted from a client
ClientMessage::Error ClientMessage::getError( void ) {
  assert( _type == ERROR );

  if( _request.error() == Messages::Net::ClientRequest::ERROR_BAD_RESPONSE )
    return ERR_RESPONSE;

  return ERR_NONE;
}

//-----------------------------------------------------------------------------
/// Sets the protobuf message as an error and maps parameters from an error
/// type into the protobuffer
void ClientMessage::setError( const ClientMessage::Error& error ) {
  _type = ERROR;
  _error = error;

  _request.Clear();
  _request.set_type( Messages::Net::ClientRequest::REQUEST_ERROR );
  if( error == ERR_RESPONSE )
    _request.set_error( Messages::Net::ClientRequest::ERROR_BAD_RESPONSE );

}

//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

