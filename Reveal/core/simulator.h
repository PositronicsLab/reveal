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
  // TODO: expand error enum and convert return values of interfaces to use enum
  // instead of bool

  enum error_e {
    ERROR_NONE = 0,
    ERROR_UNSPECIFIED
  };

  typedef boost::function<bool(Reveal::Core::authorization_ptr&, Reveal::Core::experiment_ptr, Reveal::Core::trial_ptr&)> request_trial_f;
  typedef boost::function<bool(Reveal::Core::authorization_ptr&, Reveal::Core::experiment_ptr, Reveal::Core::solution_ptr&)> submit_solution_f;

  // simulator specific configuration menu(s)
  virtual bool ui_select_configuration( Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr experiment ) = 0;
  // simulator tuning menu
  virtual bool ui_select_tuning( void ) = 0;
  // build package command
  virtual bool build_package(std::string src_path, std::string build_path) = 0;
  // execute the simulation command
  virtual bool execute( Reveal::Core::authorization_ptr auth, Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr experiment ) = 0;

  // set the request_trial function (supplied by client)
  virtual void set_request_trial( request_trial_f ) = 0;
  // set the submit_solution function (supplied by client)
  virtual void set_submit_solution( submit_solution_f ) = 0;
  // set the interprocess communication context (supplied by client)
  virtual void set_ipc_context( void* ) = 0;
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
#endif // _REVEAL_CORE_SIMULATOR_H_
