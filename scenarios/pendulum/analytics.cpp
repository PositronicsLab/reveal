#define ODEINT_V1                  // odeint versioning
#define ODEINT_RUNGEKUTTA_STEPPER  // which stepper to use

//-----------------------------------------------------------------------------

#include "pendulum.h"

//-----------------------------------------------------------------------------

int main( int argv, char* argc[] ) {
  pendulum_c pendulum( EXPERIMENTAL_PENDULUM_L );

  printf( "Analyzed pendulum\n" );

  return 0;
}

//-----------------------------------------------------------------------------
