#ifndef _PENDULUM_H_
#define _PENDULUM_H_

#include <vector>
#include <boost/numeric/odeint.hpp>

//----------------------------------------------------------------
// Experimental Configuration
//----------------------------------------------------------------
#ifdef _REVEAL_CLIENT_H_
//  #define RUNGEKUTTA_STEPPER
  #define EULER_STEPPER
#elif defined(_REVEAL_SERVER_H_)
  #define RUNGEKUTTA_STEPPER
#endif

#define EXPERIMENTAL_PENDULUM_L 100.0
//#define PENDULUM_VALIDATION_EPSILON 1E-3
//#define PENDULUM_VALIDATION_EPSILON 1E-5
#define PENDULUM_VALIDATION_EPSILON 0.5E-7
//#define PENDULUM_VALIDATION_EPSILON 1E-8

//----------------------------------------------------------------

typedef std::vector< double > state_type;

#ifdef RUNGEKUTTA_STEPPER
typedef boost::numeric::odeint::controlled_runge_kutta< boost::numeric::odeint::runge_kutta_cash_karp54< state_type > > stepper_type;
#endif
#ifdef EULER_STEPPER
typedef boost::numeric::odeint::euler< state_type, double, state_type, double > stepper_type;
#endif

//----------------------------------------------------------------

class pendulum_c {
private:
  double _l;
  double _G;
public:
  pendulum_c( const double& l );

  void operator()( const state_type& x, state_type& dx, const double t );
  
  static bool sample_trial( const unsigned& index, double& q, double& dq, double& t, double& dt );
};

//----------------------------------------------------------------

#endif // _PENDULUM_H_
