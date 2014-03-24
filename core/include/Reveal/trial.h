/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_TRIAL_H_
#define _REVEAL_TRIAL_H_

//-----------------------------------------------------------------------------

#include <vector>
#include <stdio.h>

#include <Reveal/state.h>
#include <Reveal/control.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

class trial_c {
public:
  trial_c( void ) {}
  virtual ~trial_c( void ) {}

  std::string scenario;
  unsigned index;
  double t;
  double dt;
  state_c state;
  control_c control;

  void print( void ) {
    printf( "scenario[%s]", scenario.c_str() );
    printf( ", index[%u]", index );
    printf( ", t[%f]", t );
    printf( ", dt[%f]", dt );

    // State 
    printf( ", state[" );
    printf( " q[" );
    for( unsigned i = 0; i < state.size_q(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%f", state.q(i) );
    }
    printf( "]" );
    printf( ", dq[" );
    for( unsigned i = 0; i < state.size_dq(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%f", state.dq(i) );
    }
    printf( "] " );
    printf( "]" );

    // Control
    printf( ", control[" );
    printf( " u[" );
    for( unsigned i = 0; i < control.size_u(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%f", control.u(i) );
    }
    printf( "] " );
    printf( "]" );

    printf( "\n" );
  }
};

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_TRIAL_H_
