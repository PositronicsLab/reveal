// odeint versioning
#define ODEINT_V1
//#define ODEINT_V2

// which stepper to use
#define ODEINT_EULER_STEPPER
//#define ODEINT_RUNGEKUTTA_STEPPER


//-----------------------------------------------------------------------------

#include "pendulum.h"
#include <stdio.h>

//-----------------------------------------------------------------------------
int main( void ) {
  pendulum_c pendulum( EXPERIMENTAL_PENDULUM_L );
  state_type x( 2 );
  double t, dt;
#if defined( ODEINT_V1 )
  stepper_type stepper;
  stepper.adjust_size( x );
#endif
 
  for( unsigned i = 0; i < 10; i++ ) {
    pendulum.sample_trial( i, x[0], x[1], t, dt );

#if defined( ODEINT_V1 )
    stepper.do_step( pendulum, x, t, dt );
#elif defined( ODEINT_V2 )
    double tf = t + dt;
    boost::numeric::odeint::integrate_adaptive( stepper_type(), pendulum, x, t, tf, dt );
#endif

    printf( "%d: t[%f], dt[%f] x[%.9f, %.9f]\n", i, t, dt, x[0], x[1] );
  }
  return 0;
}

//-----------------------------------------------------------------------------
