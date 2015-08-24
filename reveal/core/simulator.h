/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

-----------------------------------------------------------------------------*/
#ifndef _REVEAL_CORE_SIMULATOR_H_
#define _REVEAL_CORE_SIMULATOR_H_
//-----------------------------------------------------------------------------

#include "reveal/core/pointers.h"
#include <boost/function.hpp>
#include <dlfcn.h>

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

  typedef bool (*ui_select_configuration_f)( Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr experiment );
  typedef bool (*ui_select_tuning_f)( void );
  typedef bool (*build_package_f)(std::string src_path, std::string build_path, std::string include_path, std::string library_path, std::string library_name );
  typedef bool (*execute_f)( Reveal::Core::authorization_ptr auth, Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr experiment );
  typedef void (*set_request_trial_f)( request_trial_f );
  typedef void (*set_submit_solution_f)( submit_solution_f );
  typedef void (*set_ipc_context_f)( void* context );

private: 
  void* _handle;

  std::string _path;

public:
  ui_select_configuration_f     ui_select_configuration;
  ui_select_tuning_f            ui_select_tuning;
  build_package_f               build_package;
  execute_f                     execute;
  set_request_trial_f           set_request_trial;
  set_submit_solution_f         set_submit_solution;
  set_ipc_context_f             set_ipc_context;
  //subpackage_name_f             subpackage_name; // TODO Remove or remark fully


  simulator_c( void );
  virtual ~simulator_c( void );

  bool load( std::string path );
//  bool load( std::string path );
  void unload( void );

private:
  bool map_symbol( std::string symbol );

/*
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

  /// Returns the directory name for simulator specific package implementations
  /// @return the simulator's specific relative path name.
  virtual std::string subpackage_name( void ) = 0;
*/
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
#endif // _REVEAL_CORE_SIMULATOR_H_
