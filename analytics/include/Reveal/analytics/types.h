/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

types.h defines various types necessary for interfacing with analytics
------------------------------------------------------------------------------*/

#ifndef _REVEAL_ANALYTICS_TYPES_H_
#define _REVEAL_ANALYTICS_TYPES_H_

//-----------------------------------------------------------------------------

#include <boost/shared_ptr.hpp>

#include <Reveal/analysis.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Analytics {

//-----------------------------------------------------------------------------

enum error_e {
  ERROR_NONE,
  ERROR_UNSPECIFIED, 
  ERROR_LOAD,
  ERROR_LINK,
  ERROR_OPEN
};

//-----------------------------------------------------------------------------

enum status_e {
  STATUS_IDLE,
  STATUS_BUSY,
  STATUS_ERROR
};

//-----------------------------------------------------------------------------

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
