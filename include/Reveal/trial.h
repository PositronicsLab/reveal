/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_TRIAL_H_
#define _REVEAL_TRIAL_H_

//-----------------------------------------------------------------------------

#include "net.pb.h"
#include <vector>
#include <stdio.h>

#include <Reveal/state.h>
#include <Reveal/control.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

class Trial {
public:
  Trial( void ) {}
  virtual ~Trial( void ) {}

  std::string scenario;
  unsigned index;
  double t;
  double dt;
  State state;
  Control control;

  void Print( void ) {
    printf( "scenario[%s]", scenario.c_str() );
    printf( ", index[%u]", index );
    printf( ", t[%f]", t );
    printf( ", dt[%f]", dt );

    // State 
    printf( ", state[" );
    printf( " q[" );
    for( unsigned i = 0; i < state.Size_q(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%f", state.q(i) );
    }
    printf( "]" );
    printf( ", dq[" );
    for( unsigned i = 0; i < state.Size_dq(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%f", state.dq(i) );
    }
    printf( "] " );
    printf( "]" );

    // Control
    printf( ", control[" );
    printf( " u[" );
    for( unsigned i = 0; i < control.Size(); i++ ) {
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
