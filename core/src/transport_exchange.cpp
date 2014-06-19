#include <Reveal/transport_exchange.h>

#include <Reveal/digest.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>

//----------------------------------------------------------------------------

namespace Reveal {

//----------------------------------------------------------------------------

namespace Core {

//----------------------------------------------------------------------------
transport_exchange_c::transport_exchange_c( void ) {
  reset();
  protomsg = Reveal::Core::Messages::Net::Message();
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

  _digest = Reveal::Core::digest_ptr();
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
void transport_exchange_c::set_scenario( scenario_ptr scenario ) {
  assert( scenario );

  _scenario = scenario;
}

//----------------------------------------------------------------------------
scenario_ptr transport_exchange_c::get_scenario( void ) {
  assert( _origin != ORIGIN_UNDEFINED );
  assert( _error == ERROR_NONE );
  assert( _type == TYPE_SCENARIO );
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
  assert( _type == TYPE_TRIAL || _type == TYPE_SOLUTION );
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
transport_exchange_c::error_e transport_exchange_c::build( std::string& message ) {
  //Reveal::Core::Messages::Net::Message protomsg = Reveal::Core::Messages::Net::Message();
  Reveal::Core::Messages::Net::Message::Header* header;
  protomsg.Clear();

  header = protomsg.mutable_header();

  if( _origin == ORIGIN_SERVER ) {
    header->set_origin( Messages::Net::Message::SERVER );
    if( _type == TYPE_ERROR ) {
      header->set_type( Messages::Net::Message::ERROR );
      // cross reference the correct error
      error_e error = get_error();      // accessed by method to use asserts
      // TODO : Expand as error conditions are developed
      // if( error == ERROR_? ) {
      //   protomsg.header().set_error( ERROR_? );
      // }
      // TODO: map errors
      header->set_error( Messages::Net::Message::ERROR_NONE );
     } else if( _type == TYPE_DIGEST ) {
      // digest response
      header->set_type( Messages::Net::Message::DIGEST );
      header->set_error( Messages::Net::Message::ERROR_NONE );
      digest_ptr digest = get_digest(); // accessed by method to use asserts

      Messages::Data::Digest* protodigest = protomsg.mutable_digest();
      for( unsigned i = 0; i < digest->scenarios(); i++ ) {
        scenario_ptr scenario = digest->get_scenario( i );
        Messages::Data::Scenario* protoscenario = protodigest->add_scenario();
        protoscenario->set_id( scenario->id );
        protoscenario->set_name( scenario->name );
        protoscenario->set_description( scenario->description );
        protoscenario->set_trials( scenario->trials );
        for( unsigned j = 0; j < scenario->uris.size(); j++ ) {
          protoscenario->add_uri( scenario->uris.at(j) );
        }
      }

     } else if( _type == TYPE_SCENARIO ) {
      // scenario response
      header->set_type( Messages::Net::Message::SCENARIO );
      header->set_error( Messages::Net::Message::ERROR_NONE );
      scenario_ptr scenario = get_scenario(); // accessed by method to use asserts

      Messages::Data::Scenario* protoscenario = protomsg.mutable_scenario();
/*
      for( unsigned i = 0; i < digest->scenarios(); i++ ) {
        scenario_ptr scenario = digest->get_scenario( i );
        Messages::Data::Scenario* protoscenario = protodigest->add_scenario();
        protoscenario->set_id( scenario->id );
        protoscenario->set_name( scenario->name );
        protoscenario->set_description( scenario->description );
        protoscenario->set_trials( scenario->trials );
        for( unsigned j = 0; j < scenario->uris.size(); j++ ) {
          protoscenario->add_uri( scenario->uris.at(j) );
        }
      }
*/
      protoscenario->set_id( scenario->id );
      protoscenario->set_name( scenario->name );
      protoscenario->set_description( scenario->description );
      protoscenario->set_trials( scenario->trials );
      for( unsigned j = 0; j < scenario->uris.size(); j++ ) {
        protoscenario->add_uri( scenario->uris.at(j) );
      }
    } else if( _type == TYPE_TRIAL ) {
      // trial response
      header->set_type( Messages::Net::Message::TRIAL );
      header->set_error( Messages::Net::Message::ERROR_NONE );
      trial_ptr trial = get_trial();    // accessed by method to use asserts

      Messages::Data::Trial* prototrial = protomsg.mutable_trial();
      prototrial->set_scenario_id( trial->scenario_id );
      prototrial->set_trial_id( trial->trial_id );
      prototrial->set_t( trial->t );
      prototrial->set_dt( trial->dt );
      Messages::Data::State* state = prototrial->mutable_state();
      for( unsigned i = 0; i < trial->state.size_q(); i++ ) 
        state->add_q( trial->state.q(i) );
      for( unsigned i = 0; i < trial->state.size_dq(); i++ )
        state->add_dq( trial->state.dq(i) );
      Messages::Data::Control* control = prototrial->mutable_control();
      for( unsigned i = 0; i < trial->control.size_u(); i++ )
        control->add_u( trial->control.u(i) );

    } else if( _type == TYPE_SOLUTION ) {
      // solution receipt
      header->set_type( Messages::Net::Message::SOLUTION );
    } else {
      _error = ERROR_BUILD;
      return ERROR_BUILD;
    }
  } else if( _origin == ORIGIN_CLIENT ) {
    header->set_origin( Messages::Net::Message::CLIENT );
    if( _type == TYPE_ERROR ) {
      header->set_type( Messages::Net::Message::ERROR );
      // cross reference the correct error
      error_e error = get_error();      // accessed by method to use asserts
      // TODO : Expand as error conditions are developed
      // if( error == ERROR_? ) {
      //   protomsg.header().set_error( ERROR_? );
      // }
      // TODO: map errors
      header->set_error( Messages::Net::Message::ERROR_NONE );
    } else if( _type == TYPE_DIGEST ) {
      // digest request
      header->set_type( Messages::Net::Message::DIGEST );
      header->set_error( Messages::Net::Message::ERROR_NONE );
    } else if( _type == TYPE_SCENARIO ) {
      // scenario request
      header->set_type( Messages::Net::Message::SCENARIO );
      header->set_error( Messages::Net::Message::ERROR_NONE );
      scenario_ptr scenario = get_scenario();    // accessed by method to use asserts

      Messages::Data::Scenario* protoscenario = protomsg.mutable_scenario();
      protoscenario->set_id( scenario->id );

    } else if( _type == TYPE_TRIAL ) {
      // trial request
      header->set_type( Messages::Net::Message::TRIAL );
      header->set_error( Messages::Net::Message::ERROR_NONE );
      trial_ptr trial = get_trial();    // accessed by method to use asserts

      Messages::Data::Trial* prototrial = protomsg.mutable_trial();
      prototrial->set_scenario_id( trial->scenario_id );
      prototrial->set_trial_id( trial->trial_id );
    } else if( _type == TYPE_SOLUTION ) {
      // solution publication
      header->set_type( Messages::Net::Message::SOLUTION );
      header->set_error( Messages::Net::Message::ERROR_NONE );
      solution_ptr solution = get_solution();    // accessed by method to use asserts

      Messages::Data::Solution* protosolution = protomsg.mutable_solution();
      protosolution->set_scenario_id( solution->scenario_id );
      protosolution->set_trial_id( solution->trial_id );
      protosolution->set_t( solution->t );
      Messages::Data::State* state = protosolution->mutable_state();
      for( unsigned i = 0; i < solution->state.size_q(); i++ ) 
        state->add_q( solution->state.q(i) );
      for( unsigned i = 0; i < solution->state.size_dq(); i++ )
        state->add_dq( solution->state.dq(i) );

    } else {
      _error = ERROR_BUILD;
      return ERROR_BUILD;
    }
  } else {
    _error = ERROR_BUILD;
    return ERROR_BUILD;
  }
  protomsg.SerializeToString( &message );
  return ERROR_NONE;
}

//----------------------------------------------------------------------------
transport_exchange_c::error_e transport_exchange_c::parse( const std::string& message ) {
  protomsg.Clear();
  //Reveal::Core::Messages::Net::Message protomsg;
  error_e error;

  reset();

  protomsg.ParseFromString( message );
 
  if( protomsg.header().origin() == Messages::Net::Message::SERVER ) {
    _origin = ORIGIN_SERVER;
  } else if( protomsg.header().origin() == Messages::Net::Message::CLIENT ) {
    _origin = ORIGIN_CLIENT;
  }
  // Note: pure else case cannot happen

  if( protomsg.header().type() == Messages::Net::Message::ERROR ) {
    _type = TYPE_ERROR;
  } else if( protomsg.header().type() == Messages::Net::Message::DIGEST ) {
    _type = TYPE_DIGEST;
  } else if( protomsg.header().type() == Messages::Net::Message::SCENARIO ) {
    _type = TYPE_SCENARIO;
  } else if( protomsg.header().type() == Messages::Net::Message::TRIAL ) {
    _type = TYPE_TRIAL;
  } else if( protomsg.header().type() == Messages::Net::Message::SOLUTION ) {
    _type = TYPE_SOLUTION;
  } else {
    // UNDEFINED
    // set error
    // TODO: return error technically a malformed message
  }
/*
  if( _type == TYPE_ERROR )
    // determine error and course of action
    // if( protomsg.header().error() == Messages::Net::Message::ERROR_? ) {
    //   _error = ERROR_?
    // } else if( ... ) {
    // }
    return _error;  // ?? Really ??  Parsing didn't fail!
  }
*/
  if( _type != TYPE_ERROR ) {
    _error = ERROR_NONE;
  }

  if( _origin == ORIGIN_SERVER ) {
    if( _type == TYPE_ERROR ) {
      // cross reference errors
    } else if( _type == TYPE_DIGEST ) {
      // read out the digest
      // TODO: enumerate a specific error
      if( !protomsg.has_digest() ) return ERROR_PARSE;

      _digest = digest_ptr( new digest_c() );
      
      for( unsigned i = 0; i < protomsg.digest().scenario_size(); i++ ) {
        scenario_ptr scenario = scenario_ptr( new scenario_c() );
        _digest->add_scenario( scenario );

        scenario->id = protomsg.digest().scenario(i).id();
        scenario->name = protomsg.digest().scenario(i).name();
        scenario->description = protomsg.digest().scenario(i).description();
        scenario->trials = protomsg.digest().scenario(i).trials();
        for( unsigned j = 0; j < protomsg.digest().scenario(i).uri().size(); j++ ) {
          scenario->uris.push_back( protomsg.digest().scenario(i).uri(j) );
        }
      }
    } else if( _type == TYPE_SCENARIO ) {
      // read out the digest
      // TODO: enumerate a specific error
      if( !protomsg.has_scenario() ) return ERROR_PARSE;

      _scenario = scenario_ptr( new scenario_c() );
      
      _scenario->id = protomsg.scenario().id();
      _scenario->name = protomsg.scenario().name();
      _scenario->description = protomsg.scenario().description();
      _scenario->trials = protomsg.scenario().trials();
      for( unsigned j = 0; j < protomsg.scenario().uri().size(); j++ ) {
        _scenario->uris.push_back( protomsg.scenario().uri(j) );
      }
    } else if( _type == TYPE_TRIAL ) {
      // read out the trial

      _trial = trial_ptr( new trial_c() );

      _trial->scenario_id = protomsg.trial().scenario_id();
      _trial->trial_id = protomsg.trial().trial_id();
      _trial->t = protomsg.trial().t();
      _trial->dt = protomsg.trial().dt();
      for( int i = 0; i < protomsg.trial().state().q_size(); i++ ) {
        _trial->state.append_q( protomsg.trial().state().q(i) );
      }
      for( int i = 0; i < protomsg.trial().state().dq_size(); i++ ) {
        _trial->state.append_dq( protomsg.trial().state().dq(i) );
      }
      for( int i = 0; i < protomsg.trial().control().u_size(); i++ ) {
        _trial->control.append_u( protomsg.trial().control().u(i) );
      }

    } else if( _type == TYPE_SOLUTION ) {
      // accept the receipt.  Note: already handled in _type branch above
    }
  } else if( _origin == ORIGIN_CLIENT ) {
    if( _type == TYPE_ERROR ) {
      // cross reference errors
    } else if( _type == TYPE_DIGEST ) {
      // flag as digest request.  Note: already handled in _type branch above
    } else if( _type == TYPE_SCENARIO ) {
      _scenario = scenario_ptr( new scenario_c() );
      _scenario->id = protomsg.scenario().id();
    } else if( _type == TYPE_TRIAL ) {
      _trial = trial_ptr( new trial_c() );
      _trial->scenario_id = protomsg.trial().scenario_id();
      _trial->trial_id = protomsg.trial().trial_id();
    } else if( _type == TYPE_SOLUTION ) {
      // read out the solution

      _solution = solution_ptr( new solution_c() );

      _solution->scenario_id = protomsg.solution().scenario_id();
      _solution->trial_id = protomsg.solution().trial_id();
      _solution->t = protomsg.solution().t();
      for( int i = 0; i < protomsg.solution().state().q_size(); i++ ) {
        _solution->state.append_q( protomsg.solution().state().q(i) );
      }
      for( int i = 0; i < protomsg.solution().state().dq_size(); i++ ) {
        _solution->state.append_dq( protomsg.solution().state().dq(i) );
      }
    }
  }

  return ERROR_NONE;
}

//----------------------------------------------------------------------------

} // namespace Core

//----------------------------------------------------------------------------

} // namespace Reveal

//----------------------------------------------------------------------------
