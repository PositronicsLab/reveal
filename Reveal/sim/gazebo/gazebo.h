#ifndef _REVEAL_SIM_GAZEBO_H_
#define _REVEAL_SIM_GAZEBO_H_

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "Reveal/core/ipc.h"
#include "Reveal/core/simulator.h"
#include "Reveal/core/package.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Sim {
//-----------------------------------------------------------------------------

class gazebo_c;
typedef boost::shared_ptr<gazebo_c> gazebo_ptr;

//-----------------------------------------------------------------------------
class gazebo_c : public Reveal::Core::simulator_c {
public:

  // implementing the simulator_c interface
  virtual bool ui_select_configuration( Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr experiment );
  virtual bool ui_select_tuning( void );
  virtual bool build_package( std::string src_path, std::string build_path );
  virtual bool execute( Reveal::Core::authorization_ptr auth, Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr experiment );

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
  Reveal::Core::package_ptr _package;

  submit_solution_f _submit_solution;
  request_trial_f _request_trial;
  void* _ipc_context;

  dynamics_e _dynamics;

  std::string _world_path;
  std::string _model_path;
  std::string _plugin_path;

  Reveal::Core::pipe_ptr _ipc; 
public:

  gazebo_c( void );
  gazebo_c( request_trial_f, submit_solution_f, void* ipc_context );
  virtual ~gazebo_c( void );

private:
  dynamics_e prompt_dynamics( void );
  std::vector< std::string > environment_keys( void );

  double _desired_time_step;
  bool child_quit;
  void child_sighandler( int signum );
  void print_tuning_menu( void ); 

  static void exit_sighandler( int signum );

  // TODO: should add a method to validate what gazebo has loaded for the world
  // with respect to the expectations of a scenario
};


//-----------------------------------------------------------------------------
} // namespace Sim
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_SIM_GAZEBO_H_
