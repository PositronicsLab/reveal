#ifndef _REVEAL_CLIENT_GAZEBO_H_
#define _REVEAL_CLIENT_GAZEBO_H_

#include <string>
#include <vector>

#include "Reveal/core/ipc.h"
#include "Reveal/core/state.h"
#include "Reveal/client/simulator.h"

#include "Reveal/client/package.h"
#include "Reveal/client/client.h"

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Client {
//-----------------------------------------------------------------------------

class gazebo_c : public simulator_c {
public:

  virtual bool ui_select_configuration( void );
  virtual bool ui_select_tuning( void );
  virtual bool build_package( std::string src_path, std::string build_path );
  virtual bool execute( Reveal::Core::authorization_ptr auth, Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr experiment );

//  virtual bool experiment( Reveal::Core::authorization_ptr auth );

  virtual void set_request_trial( request_trial_f request_trial );
  virtual void set_submit_solution( submit_solution_f submit_solution );
  virtual void set_ipc_context( void* context );

  enum dynamics_e {
    DYNAMICS_ODE,
    DYNAMICS_BULLET,
    DYNAMICS_DART,
    DYNAMICS_SIMBODY
  };

private:
  package_ptr _package;

  submit_solution_f _submit_solution;
  request_trial_f _request_trial;
  void* _ipc_context;

  dynamics_e _dynamics;

  std::string _world_path;
  std::string _model_path;
  std::string _plugin_path;

  Reveal::Core::pipe_ptr _ipc; 
  Reveal::Core::pipe_ptr _exit_write;
  Reveal::Core::pipe_ptr _exit_read;
public:

  gazebo_c( void );
  gazebo_c( request_trial_f, submit_solution_f, void* ipc_context );
  virtual ~gazebo_c( void );

private:
  dynamics_e prompt_dynamics( void );
  std::vector< std::string > environment_keys( void );

  bool child_quit;
  void child_sighandler( int signum );
  void print_tuning_menu( void ); 

  static void exit_sighandler( int signum );

  // TODO: should add a method to validate what gazebo has loaded for the world
  // with respect to the expectations of a scenario

  // write a reveal model into gazebo to update the simulator
  static bool write_model( Reveal::Core::model_ptr model, gazebo::physics::WorldPtr world );

  // write a reveal trial into gazebo to update the simulator
  static bool write_trial( Reveal::Core::trial_ptr trial, gazebo::physics::WorldPtr world );

  // read out a gazebo model as a reveal model
  static Reveal::Core::model_ptr read_model( gazebo::physics::WorldPtr world, std::string model_name, std::map<std::string,double> control );

  // read out a gazebo state as a reveal model solution
  static Reveal::Core::solution_ptr read_model_solution( gazebo::physics::WorldPtr world, std::vector<std::string> model_list, std::string scenario_id, unsigned trial_id );

  // read out a gazebo state as a reveal client solution
  static Reveal::Core::solution_ptr read_client_solution( gazebo::physics::WorldPtr world, std::vector<std::string> model_list, std::string scenario_id, unsigned trial_id );

  //* helpers *
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
} // namespace Client
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CLIENT_GAZEBO_H_
