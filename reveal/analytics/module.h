/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

module.h defines the module_c abstract interface that facilitates the dynamic 
binding of the various specialized analyzers required for a given scenario
------------------------------------------------------------------------------*/

#ifndef _REVEAL_ANALYTICS_MODULE_H_
#define _REVEAL_ANALYTICS_MODULE_H_

//-----------------------------------------------------------------------------

#include "reveal/core/pointers.h"
#include "reveal/core/solution_set.h"
#include "reveal/core/analysis.h"
#include "reveal/analytics/types.h"

#include <boost/enable_shared_from_this.hpp>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Analytics {
//-----------------------------------------------------------------------------

class module_c : public boost::enable_shared_from_this<module_c>  {
public:

  /// loads a analytics module
  /// @param file the path to the module to load
  /// @return on success returns ERROR_NONE otherwise returns an enumerated
  ///         value that gives some information on the error 
  virtual Reveal::Analytics::error_e load( std::string file ) = 0;

  /// executes the exposed analytics function for a loaded module
  /// @param in the solution set submitted for analysis
  /// @param out the result of the module's analytics computation
  /// @return on success returns ERROR_NONE otherwise returns an enumerated
  ///         value that gives some information on the error 
  virtual Reveal::Analytics::error_e analyze( Reveal::Core::solution_set_ptr in, Reveal::Core::analysis_ptr& out ) = 0;
};

//-----------------------------------------------------------------------------
} // namespace Analytics
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_ANALYTICS_MODULE_H_
