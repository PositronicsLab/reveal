#include "pendulum.h"

//-----------------------------------------------------------------------------

#include <math.h>

//-----------------------------------------------------------------------------
pendulum_c::pendulum_c( double l ) {
  _l = l;
  _G = 9.8;
}

//-----------------------------------------------------------------------------
void pendulum_c::operator()( const state_type& x, state_type& dxdt, const double t ) {
  //printf( "x[%f, %f]\n", x[0], x[1] );
  dxdt[0] = x[1];
  dxdt[1] = -_G / _l * sin( x[0] );
}

//-----------------------------------------------------------------------------
bool pendulum_c::sample_trial( const unsigned& index, double& q, double& dq, double& t, double& dt ) {
  if( index >= 10 ) return false;

  switch( index ) {
  default:
  case 0:
    q = 0.5;    dq = 1.0;    t = 2.0;    dt = 0.001;
    break;
  case 1:
    q = 1.0;    dq = 2.0;    t = 1.0;    dt = 0.001;
    break;
  case 2:
    q = 0.25;    dq = 0.25;    t = 4.0;    dt = 0.001;
    break;
  case 3:
    q = -0.5;    dq = -0.5;    t = 2.5;    dt = 0.001;
    break;
  case 4:
    q = -0.25;    dq = 0.25;    t = 3.0;    dt = 0.001;
    break;
  case 5:
    q = -0.5;    dq = -0.0125;    t = 5.0;    dt = 0.001;
    break;
  case 6:
    q = 0.15;    dq = 0.005;    t = 6.0;    dt = 0.001;
    break;
  case 7:
    q = -0.05;    dq = 1.5;    t = 3.5;    dt = 0.001;
    break;
  case 8:
    q = 0.005;    dq = 0.0005;    t = 7.0;    dt = 0.001;
    break;
  case 9:
    q = -0.02;    dq = -0.25;    t = 2.75;    dt = 0.001;
    break;
  }
  return true;
}

//-----------------------------------------------------------------------------
void pendulum_c::analyze( const solution_set& theoretical, const solution_set& experimental, const std::vector<double>& realtime_duration, std::vector<double>& analysis ) {

}

//-----------------------------------------------------------------------------
