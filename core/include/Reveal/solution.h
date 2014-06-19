/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_SOLUTION_H_
#define _REVEAL_CORE_SOLUTION_H_

//-----------------------------------------------------------------------------

#include <vector>

#include <Reveal/state.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Core {

//-----------------------------------------------------------------------------

class solution_c {
public:
  solution_c( void ) {}
  virtual ~solution_c( void ) {}

  unsigned scenario_id;
  unsigned trial_id;
  // user identifier
  double t;
  state_c state;

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
  }
 
};

//-----------------------------------------------------------------------------

}  // namespace Core

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_SOLUTION_H_
