/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

script.h defines the script_c interface for analyzers implemented using 
uncompiled scripting interfaces
------------------------------------------------------------------------------*/

#ifndef _REVEAL_ANALYTICS_SCRIPT_H_
#define _REVEAL_ANALYTICS_SCRIPT_H_

//-----------------------------------------------------------------------------

#include "Reveal/analytics/types.h"
#include "Reveal/core/analysis.h"

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Analytics {

//-----------------------------------------------------------------------------

class script_c : public Reveal::Analytics::module_c {
public:
  script_c( void ) { }
  virtual ~script_c( void ) { }

  virtual error_e load( std::string filename ) {
    return ERROR_NONE;
  }

  virtual error_e analyze( Reveal::Core::analysis_c in, Reveal::Core::analysis_c& out ) {
    return ERROR_NONE;
  }

};

//-----------------------------------------------------------------------------

} // namespace Analytics

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_ANALYTICS_PLUGIN_H_
