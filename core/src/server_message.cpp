#include <Reveal/server_message.h>

#include <assert.h>


#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Core {

//-----------------------------------------------------------------------------
/// Default Constructor
server_message_c::server_message_c( void ) {
  _type = UNDEFINED;
  _response = Messages::Net::ServerResponse();
}

//-----------------------------------------------------------------------------
/// Destructor
server_message_c::~server_message_c( void ) {

}

//-----------------------------------------------------------------------------
/// Parses a serialize protobuffer ServerResponse message string into the 
/// protobuf member and returns an error if the message fails to meet the 
/// required criteria
bool server_message_c::parse( const std::string& serial ) {

  // clear any old response in the message member
  _response.Clear();
  
  // parse the serialized message into the protobuf structure
  if( !_response.ParseFromString( serial ) ) {
    // TODO: send to error log instead of printing
    printf( "ERROR: Failed to Parse\n" );
    return false;
  }

  // determine the type of server response
  switch( _response.type() ) {
  case Messages::Net::ServerResponse::RESPONSE_SCENARIO:
    _type = SCENARIO;
    break;
  case Messages::Net::ServerResponse::RESPONSE_TRIAL:
    _type = TRIAL;
    break;
  case Messages::Net::ServerResponse::RESPONSE_RECEIVED_SOLUTION:
    _type = SOLUTION;
    break;
  case Messages::Net::ServerResponse::RESPONSE_ERROR:
    _type = ERROR;
    break;
  case Messages::Net::ServerResponse::RESPONSE_UNDEFINED:
  default:
    _type = UNDEFINED;
    return false;                // bad message.  Failed to parse
  }

  // validate that the message has the necessary optional structures if it is
  // defined as a given type
  if( _type == SCENARIO ) {
    if( !_response.has_scenario() ) {
      // Error.  malformed message
      return false;
    }
  } else if( _type == TRIAL ) {
    if( !_response.has_trial() ) {
      // Error.  malformed message
      return false;
    }
  } else if( _type == SOLUTION ) {
    if( !_response.has_solution() ) {
      // Error.  malformed message
      return false;
    }
  } else if( _type == ERROR ) {

  }

  // message may still not be fully validated, but it does fulfill the most
  // basic requirements to be classified by type
  return true;
}

//-----------------------------------------------------------------------------
/// Serializes the protobuf member into a string for transmission through the
/// transport layer
std::string server_message_c::serialize( void ) {
  std::string serial;

  _response.SerializeToString( &serial );
 
  return serial;
}

//-----------------------------------------------------------------------------
/// Returns the Type of message after it has been parsed or set
server_message_c::type_e server_message_c::get_type( void ) {
  return _type;
}

//-----------------------------------------------------------------------------
/// Returns the scenario data attached to the message
scenario_ptr server_message_c::get_scenario( void ) {
  assert( _type == SCENARIO );

  scenario_ptr scenario = scenario_ptr( new scenario_c() );

  scenario->name = _response.scenario().name();
  scenario->trials = _response.scenario().trials();
  for( int i = 0; i < _response.scenario().url_size(); i++ ) {
    scenario->urls.push_back( _response.scenario().url( i ) );
  }
  return scenario; 
}

//-----------------------------------------------------------------------------
/// Sets the protobuf message as a scenario and maps parameters from a scenario
/// data structure into the protobuffer
void server_message_c::set_scenario( scenario_ptr scenario ) {
  _type = SCENARIO;

  _response.Clear();
  _response.set_type( Messages::Net::ServerResponse::RESPONSE_SCENARIO );
  Messages::Net::ScenarioResponse* sr = _response.mutable_scenario();
  sr->set_name( scenario->name.c_str() );
  sr->set_trials( scenario->trials );
  for( unsigned i = 0; i < scenario->urls.size(); i++ ) {
    sr->add_url( scenario->urls.at(i) );
  }
}

//-----------------------------------------------------------------------------
/// Return the trial data attached to the message
trial_ptr server_message_c::get_trial( void ) {
  assert( _type == TRIAL );

  trial_ptr trial = trial_ptr( new trial_c() );

  trial->scenario = _response.trial().scenario();
  trial->index = _response.trial().index();
  trial->t = _response.trial().t();
  trial->dt = _response.trial().dt();

  for( int i = 0; i < _response.trial().state().q_size(); i++ ) {
    trial->state.append_q( _response.trial().state().q(i) );
  }
  for( int i = 0; i < _response.trial().state().dq_size(); i++ ) {
    trial->state.append_dq( _response.trial().state().dq(i) );
  }
  for( int i = 0; i < _response.trial().control().u_size(); i++ ) {
    trial->control.append_u( _response.trial().control().u(i) );
  }

  return trial;
}

//-----------------------------------------------------------------------------
/// Sets the protobuf message as a trial and maps parameters from a trial 
/// data structure into the protobuffer
void server_message_c::set_trial( trial_ptr trial ) {
  _type = TRIAL;

  _response.Clear();
  _response.set_type( Messages::Net::ServerResponse::RESPONSE_TRIAL );
  Messages::Net::TrialResponse* tr = _response.mutable_trial();
  tr->set_scenario( trial->scenario.c_str() );
  tr->set_index( trial->index );
  tr->set_t( trial->t );
  tr->set_dt( trial->dt );
  Messages::Net::State* state = tr->mutable_state();
  for( unsigned i = 0; i < trial->state.size_q(); i++ ) {
    state->add_q( trial->state.q(i) );
  }
  for( unsigned i = 0; i < trial->state.size_dq(); i++ ) {
    state->add_dq( trial->state.dq(i) );
  }
  Messages::Net::Control* control = tr->mutable_control();
  for( unsigned i = 0; i < trial->control.size_u(); i++ ) {
    control->add_u( trial->control.u(i) );
  }
 
}

//-----------------------------------------------------------------------------
/// Returns the solution data attached to the message
solution_ptr server_message_c::get_solution( void ) {
  assert( _type == SOLUTION );

  solution_ptr solution = solution_ptr( new solution_c() );

  // TODO  Determine how this method apples.  The end result is actually 
  // a good/bad answer and not a SolutionPtr

  return solution;
}

//-----------------------------------------------------------------------------
/// Sets the protobuf message as a solution and maps parameters from a solution
/// data structure into the protobuffer
void server_message_c::set_solution( solution_ptr solution ) {
  _type = SOLUTION;

  _response.Clear();
  _response.set_type(Messages::Net::ServerResponse::RESPONSE_RECEIVED_SOLUTION);
  // TODO  Determine any other properties that might apply
  // TODO: Determine if should even take Solution parameter.  
  //       May be error instead
  Messages::Net::SolutionResponse* sr = _response.mutable_solution();
  sr->set_scenario( solution->scenario.c_str() );
  sr->set_index( solution->index );
}

//-----------------------------------------------------------------------------
/// Returns the error that was transmitted from a server
server_message_c::error_e server_message_c::get_error( void ) {
  assert( _type == ERROR );

  if( _response.error() == Messages::Net::ServerResponse::ERROR_BAD_REQUEST )
    return ERR_REQUEST;

  return ERR_NONE; 
}

//-----------------------------------------------------------------------------
/// Sets the protobuf message as an error and maps parameters from an error
/// type into the protobuffer
void server_message_c::set_error( const server_message_c::error_e& error ) {
  _type = ERROR;
  _error = error;

  _response.Clear();
  _response.set_type( Messages::Net::ServerResponse::RESPONSE_ERROR );
  if( error == ERR_REQUEST )
    _response.set_error( Messages::Net::ServerResponse::ERROR_BAD_REQUEST );

}

//-----------------------------------------------------------------------------

} // namespace Core

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------

