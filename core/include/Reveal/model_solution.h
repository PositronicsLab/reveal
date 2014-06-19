/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_MODEL_SOLUTION_H_
#define _REVEAL_CORE_MODEL_SOLUTION_H_

//-----------------------------------------------------------------------------

#include <vector>

#include <Reveal/state.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Core {

//-----------------------------------------------------------------------------

class model_solution_c {
public:
  model_solution_c( void ) {}
  virtual ~model_solution_c( void ) {}

  unsigned scenario_id;
  unsigned trial_id;
  double t;
  state_c state;
  //state_c epsilon;

  void print( void ) {
    printf( "scenario_id[%u]", scenario_id );
    printf( ", trial_id[%u]", trial_id );
    printf( ", t[%f]", t );

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
/*
    printf( ", epsilon[" );
    printf( " q[" );
    for( unsigned i = 0; i < epsilon.size_q(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%f", epsilon.q(i) );
    }
    printf( "]" );
    printf( ", dq[" );
    for( unsigned i = 0; i < epsilon.size_dq(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%f", epsilon.dq(i) );
    }
    printf( "] " );
    printf( "]" );
*/
    printf( "\n" );
  }
 
};

//-----------------------------------------------------------------------------

}  // namespace Core

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_MODEL_SOLUTION_H_
