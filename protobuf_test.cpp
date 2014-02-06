#include "protocols.pb.h"
#include <stdio.h>

int main ( int argc, char* argv[] ) {

  GOOGLE_PROTOBUF_VERIFY_VERSION;

  // Create a test message
  reveal::WorldState x;
  
  x.set_time( 0.0 );
 
  reveal::WorldState::State* state = x.add_state();
  state->add_q( 1.0 );
  state->add_dq( 2.0 );

  reveal::WorldState::Control* control = x.add_control();
  control->add_u( 3.0 );


  // Read data from the test message
  unsigned states = x.state_size();
  printf( "states: %u\n", states );
  for( unsigned i = 0; i < states; i++ ) {
    unsigned values;

    const reveal::WorldState_State* state = &x.state( i );

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

    const reveal::WorldState_Control* control = &x.control( i );

    values = control->u_size();
    printf( "u values: %u\n", values );
    for( unsigned i = 0; i < values; i++ ) {
      printf( "u[%d]: %f\n", i, control->u( i ) );      
    }
  }

  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}
