#ifndef _REVEAL_SIM_GAZEBO_HELPERS_H_
#define _REVEAL_SIM_GAZEBO_HELPERS_H_

#include <string>
#include <vector>

#include "Reveal/core/pointers.h"
#include "Reveal/core/state.h"

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Sim {
//-----------------------------------------------------------------------------
namespace Gazebo {
//-----------------------------------------------------------------------------

class helpers_c {
public:
  // write a reveal model into gazebo to update the simulator
  static bool write_model( Reveal::Core::model_ptr model, gazebo::physics::WorldPtr world );

  // write a reveal trial into gazebo to update the simulator
  static bool write_trial( Reveal::Core::trial_ptr trial, gazebo::physics::WorldPtr world );

  // read out a gazebo state as a gazebo trial

  // read out a gazebo model as a reveal model
  static Reveal::Core::model_ptr read_model( gazebo::physics::WorldPtr world, std::string model_name, std::map<std::string,double> control );

  // read out a gazebo state as a reveal model solution
  static Reveal::Core::solution_ptr read_model_solution( gazebo::physics::WorldPtr world, std::vector<std::string> model_list, std::string scenario_id, unsigned trial_id );

  // read out a gazebo state as a reveal client solution
  static Reveal::Core::solution_ptr read_client_solution( gazebo::physics::WorldPtr world, std::vector<std::string> model_list, std::string scenario_id, unsigned trial_id );

  // map a reveal state's angular velocity into a gazebo vector
  static gazebo::math::Vector3 position( const Reveal::Core::state_c& state );
  // map a reveal state's rotation into a gazebo quaternion
  static gazebo::math::Quaternion rotation( const Reveal::Core::state_c& state );
  // map a reveal state's linear velocity into a gazebo vector
  static gazebo::math::Vector3 linear_velocity( const Reveal::Core::state_c& state );
  // map a reveal state's angular velocity into a gazebo vector
  static gazebo::math::Vector3 angular_velocity( const Reveal::Core::state_c& state );
  // map a reveal state into a gazebo pose structure
  static gazebo::math::Pose pose( const Reveal::Core::state_c& state );

  // map a gazebo position into a reveal state
  static void position( const gazebo::math::Vector3 pos, Reveal::Core::state_c& state );
  // map a gazebo rotation into a reveal state
  static void rotation( const gazebo::math::Quaternion rot, Reveal::Core::state_c& state );
  // map a gazebo linear velocity into a reveal state
static void linear_velocity( const gazebo::math::Vector3 lvel, Reveal::Core::state_c& state );
  // map a gazebo angular velocity into a reveal state
  static void angular_velocity( const gazebo::math::Vector3 avel, Reveal::Core::state_c& state );
  // map a gazebo pose into a reveal state  
  static void pose( const gazebo::math::Pose pose, Reveal::Core::state_c& state );

  // get a gazebo world's simulation time
  static double sim_time( gazebo::physics::WorldPtr world );
  // set a gazebo world's simulation time
  static void sim_time( double t, gazebo::physics::WorldPtr world );
  // get a gazebo world's step size
  static double step_size( gazebo::physics::WorldPtr world );
  // get a gazebo world's real time
  static double real_time( gazebo::physics::WorldPtr world );
  // reset a gazebo world
  static void reset( gazebo::physics::WorldPtr world );

};

//-----------------------------------------------------------------------------
} // namespace Gazebo
//-----------------------------------------------------------------------------
} // namespace Sim
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_SIM_GAZEBO_HELPERS_H_