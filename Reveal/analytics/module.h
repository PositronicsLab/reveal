/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

module.h defines the module_c abstract interface that facilitates the dynamic 
binding of the various specialized analyzers required for a given scenario
------------------------------------------------------------------------------*/

#ifndef _REVEAL_ANALYTICS_MODULE_H_
#define _REVEAL_ANALYTICS_MODULE_H_

//-----------------------------------------------------------------------------

#include "Reveal/core/pointers.h"
#include "Reveal/core/solution_set.h"
#include "Reveal/analytics/types.h"
#include "Reveal/core/analysis.h"

#include <boost/enable_shared_from_this.hpp>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Analytics {
//-----------------------------------------------------------------------------

class module_c : public boost::enable_shared_from_this<module_c>  {
public:

  virtual Reveal::Analytics::error_e load( std::string file ) = 0;
  virtual Reveal::Analytics::error_e analyze( Reveal::Core::solution_set_ptr in, Reveal::Core::analysis_ptr& out ) = 0;
};

//-----------------------------------------------------------------------------
} // namespace Analytics
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_ANALYTICS_MODULE_H_
