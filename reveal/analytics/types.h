/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

types.h defines various types necessary for interfacing with analytics
------------------------------------------------------------------------------*/

#ifndef _REVEAL_ANALYTICS_TYPES_H_
#define _REVEAL_ANALYTICS_TYPES_H_

//-----------------------------------------------------------------------------

#include <boost/shared_ptr.hpp>

#include "reveal/core/pointers.h"
#include "reveal/core/solution_set.h"
#include "reveal/core/analysis.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Analytics {
//-----------------------------------------------------------------------------

/// enumerated error codes used in analytics interfaces
enum error_e {
  ERROR_NONE,
  ERROR_UNSPECIFIED, 
  ERROR_LOAD,
  ERROR_LINK,
  ERROR_OPEN
};

//-----------------------------------------------------------------------------

/// defines the analyzer function signature as a function pointer 
/// matches the definition in module.h
typedef error_e (*analyzer_f)( Reveal::Core::solution_set_ptr input, Reveal::Core::analysis_ptr& output );

//-----------------------------------------------------------------------------

class worker_c;
typedef boost::shared_ptr< Reveal::Analytics::worker_c > worker_ptr;

//-----------------------------------------------------------------------------
} // namespace Analytics
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_ANALYTICS_TYPES_H_
