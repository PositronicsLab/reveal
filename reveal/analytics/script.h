/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

script.h defines the script_c interface for analyzers implemented using 
uncompiled scripting interfaces
------------------------------------------------------------------------------*/

#ifndef _REVEAL_ANALYTICS_SCRIPT_H_
#define _REVEAL_ANALYTICS_SCRIPT_H_

//-----------------------------------------------------------------------------

#include "reveal/core/analysis.h"

#include "reveal/analytics/module.h"
#include "reveal/analytics/types.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Analytics {
//-----------------------------------------------------------------------------

class script_c : public Reveal::Analytics::module_c {
public:
  script_c( void );
  virtual ~script_c( void );

  virtual Reveal::Analytics::error_e load( std::string file );
  virtual Reveal::Analytics::error_e analyze( Reveal::Core::solution_set_ptr in, Reveal::Core::analysis_ptr& out );

};

//-----------------------------------------------------------------------------
} // namespace Analytics
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_ANALYTICS_SCRIPT_H_
