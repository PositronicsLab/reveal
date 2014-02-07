/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

A unit test (not quite because it requires user verification) to validate
the read write operations using reveal protocol buffer messages

-----------------------------------------------------------------------------*/

#include "protocols.pb.h"
#include <stdio.h>
#include <string.h>

//-----------------------------------------------------------------------------
/// Validates reading and writing to and from a WorldState protobuffer
bool validate_state_message_rw() {
  // Create a test message
  reveal::ip::WorldState x;
  
  x.set_time( 0.0 );
 
  reveal::ip::WorldState::State* state = x.add_state();
  state->add_q( 1.0 );
  state->add_dq( 2.0 );

  reveal::ip::WorldState::Control* control = x.add_control();
  control->add_u( 3.0 );


  // Read data from the test message
  unsigned states = x.state_size();
  printf( "states: %u\n", states );
  for( unsigned i = 0; i < states; i++ ) {
    unsigned values;

    const reveal::ip::WorldState_State* state = &x.state( i );

    values = state->q_size();
    printf( "q values: %u\n", values );
    for( unsigned i = 0; i < values; i++ ) {
      printf( "q[%d]: %f\n", i, state->q( i ) );      
    }

    values = state->dq_size();
    printf( "dq values: %u\n", values );
    for( unsigned i = 0; i < values; i++ ) {
      printf( "dq[%d]: %f\n", i, state->dq( i ) );      
    }
  }
  
  unsigned controls = x.control_size();
  printf( "controls: %u\n", controls );
  for( unsigned i = 0; i < controls; i++ ) {
    unsigned values;

    const reveal::ip::WorldState_Control* control = &x.control( i );

    values = control->u_size();
    printf( "u values: %u\n", values );
    for( unsigned i = 0; i < values; i++ ) {
      printf( "u[%d]: %f\n", i, control->u( i ) );      
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
/// Validates reading and writing to and from a Models protobuffer
bool validate_model_message_rw() {
  // Create a test message
  reveal::ip::Models models;
  
  models.add_url( "http://robotics.gwu.edu/~positronics/" );
  models.add_url( "http://www.osrfoundation.org/" );
  models.add_url( "http://www.gazebosim.org/" );
  
  // Read data from the test message
  std::string url;
  unsigned urls = models.url_size();
  for( unsigned i = 0; i < urls; i++ ) {
    url = models.url( i );
    printf( "url[%u]: %s\n", i, url.c_str() );
  }

  return true;
}

//-----------------------------------------------------------------------------
/// Unit test entry point
int main ( int argc, char* argv[] ) {

  GOOGLE_PROTOBUF_VERIFY_VERSION;

  if( !validate_state_message_rw() ) {
    printf( "WorldState protobuf message validation : FAILED\n" );
  } else {
    printf( "WorldState protobuf message validation : PASSED\n" );
  }
    
  if( !validate_model_message_rw() ) {
    printf( "Model protobuf message validation : FAILED\n" );
  } else {
    printf( "Model protobuf message validation : PASSED\n" );
  }

  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}

//-----------------------------------------------------------------------------
