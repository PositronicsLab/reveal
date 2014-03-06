/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_SOLUTION_H_
#define _REVEAL_SOLUTION_H_

//-----------------------------------------------------------------------------

#include "net.pb.h"
#include <vector>

#include <Reveal/state.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

class Solution {
public:
  Solution( void ) {}
  virtual ~Solution( void ) {}

  std::string scenario;
  unsigned index;
  double t;
  State state;

  void Print( void ) {
    printf( "scenario[%s]", scenario.c_str() );
    printf( ", index[%u]", index );
    printf( ", t[%f]", t );

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

    printf( "\n" );
  }
 
};

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_SOLUTION_H_
