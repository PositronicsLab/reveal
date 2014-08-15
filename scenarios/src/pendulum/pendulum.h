#ifndef _PENDULUM_H_
#define _PENDULUM_H_

#include <vector>
#include <boost/numeric/odeint.hpp>
#include <Reveal/pointers.h>

//-----------------------------------------------------------------------------

#define EXPERIMENTAL_PENDULUM_L 100.0
//#define PENDULUM_VALIDATION_EPSILON 1E-3
//#define PENDULUM_VALIDATION_EPSILON 1E-5
#define PENDULUM_VALIDATION_EPSILON 0.5E-7
//#define PENDULUM_VALIDATION_EPSILON 1E-8

//-----------------------------------------------------------------------------

typedef std::vector< double > state_type;

#ifdef ODEINT_RUNGEKUTTA_STEPPER
  #ifdef ODEINT_V1
    typedef boost::numeric::odeint::stepper_rk78_fehlberg< state_type, double > stepper_type;
  #elif defined( ODEINT_V2 )
    typedef boost::numeric::odeint::controlled_runge_kutta< boost::numeric::odeint::runge_kutta_cash_karp54< state_type > > stepper_type;
  #endif
#elif defined( ODEINT_EULER_STEPPER)
  #ifdef ODEINT_V1
    typedef boost::numeric::odeint::stepper_euler< state_type, double > stepper_type;
  #elif defined( ODEINT_V2 )
    typedef boost::numeric::odeint::euler< state_type, double, state_type, double > stepper_type;
  #endif
#endif

//-----------------------------------------------------------------------------

typedef std::vector<Reveal::Core::solution_ptr> solution_set;

//-----------------------------------------------------------------------------

class pendulum_c {
private:
  double _l;
  double _G;
public:
  pendulum_c( double l );

  void operator()( const state_type& x, state_type& dx, const double t );
  
  static bool sample_trial( const unsigned& index, double& q, double& dq, double& t, double& dt );

  void analyze( const solution_set& theoretical, const solution_set& experimental, const std::vector<double>& realtime_duration, std::vector<double>& analysis );
};

//-----------------------------------------------------------------------------

#endif // _PENDULUM_H_
