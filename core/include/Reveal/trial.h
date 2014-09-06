/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

trial.h defines the trial_c data-structure that contains data provided to a 
client for a given scenario.  For a single scenario, there may be many trials.
A client evaluates a given trial within a scenario and submits its solution
to the server for analysis
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_TRIAL_H_
#define _REVEAL_CORE_TRIAL_H_

//-----------------------------------------------------------------------------

#include <vector>
#include <stdio.h>

//#include <Reveal/scenario.h>
#include <Reveal/state.h>
#include <Reveal/control.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Core {

//-----------------------------------------------------------------------------

class trial_c {
public:
  trial_c( void ) {}
  virtual ~trial_c( void ) {}

  //std::string scenario;
  //scenario_ptr scenario;
  unsigned scenario_id;
  unsigned trial_id;
  double t;
  double dt;
  state_c state;
  control_c control;

  void print( void ) {
    //printf( "scenario[%s]", scenario->name.c_str() );
    printf( "scenario_id[%u]", scenario_id );
    printf( ", trial_id[%u]", trial_id );
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

}  // namespace Core

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_TRIAL_H_
