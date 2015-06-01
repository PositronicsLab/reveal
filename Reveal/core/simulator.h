/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

-----------------------------------------------------------------------------*/
#ifndef _REVEAL_CORE_SIMULATOR_H_
#define _REVEAL_CORE_SIMULATOR_H_
//-----------------------------------------------------------------------------

#include "Reveal/core/pointers.h"
#include <boost/function.hpp>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class simulator_c;
typedef boost::shared_ptr<simulator_c> simulator_ptr;

//-----------------------------------------------------------------------------
class simulator_c {
public:

  typedef boost::function<bool(Reveal::Core::authorization_ptr&, Reveal::Core::experiment_ptr, Reveal::Core::trial_ptr&)> request_trial_f;
  typedef boost::function<bool(Reveal::Core::authorization_ptr&, Reveal::Core::experiment_ptr, Reveal::Core::solution_ptr&)> submit_solution_f;

  /// Presents a user interface specific to the selected simulator for 
  /// configuring the simulator itself
  /// @param scenario the scenario to be simulated
  /// @param experiment the experiment to be simulated
  /// @return true if the user interface operation was successful OR false if 
  ///         the operation failed for any reason
  virtual bool ui_select_configuration( Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr experiment ) = 0;

  /// Presents a user interface specific to the selected simulator for tuning 
  /// the simulation
  /// @return true if the user interface operation was successful OR false if 
  ///         the operation failed in any way
  virtual bool ui_select_tuning( void ) = 0;

  /// Commands the simulator to build the packages in a compatible format
  /// @param src_path the path to the package source code
  /// @param build_path the path to the build directory
  /// @param true if the package is successfully built OR false if the operation
  ///        failed for any reason
  virtual bool build_package(std::string src_path, std::string build_path) = 0;

  /// Commands the simulator to execute the simulation
  /// @param auth the authorization received from logging into the server
  /// @param scenario the scenario to be simulated
  /// @param experiment the experiment to be simulated
  /// @return true if execution was successful OR false if the operation failed
  ///         for any reason
  virtual bool execute( Reveal::Core::authorization_ptr auth, Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr experiment ) = 0;

  /// Sets the request_trial function (supplied by client)
  /// @param request_trial the request trial function pointer
  virtual void set_request_trial( request_trial_f ) = 0;

  /// Sets the submit_solution function (supplied by client)
  /// @param submit_solution the submit solution function pointer
  virtual void set_submit_solution( submit_solution_f ) = 0;

  /// Sets the interprocess communication context (supplied by client)
  /// @param context the process's communication context
  virtual void set_ipc_context( void* ) = 0;
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
#endif // _REVEAL_CORE_SIMULATOR_H_
