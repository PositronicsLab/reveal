/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

script.h defines the script_c interface for analyzers implemented using 
uncompiled scripting interfaces
------------------------------------------------------------------------------*/

#ifndef _REVEAL_ANALYTICS_SCRIPT_H_
#define _REVEAL_ANALYTICS_SCRIPT_H_

//-----------------------------------------------------------------------------

#include "Reveal/analytics/module.h"
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

  virtual Reveal::Analytics::error_e load( std::string file ) {
    assert( file != "" );
    return ERROR_NONE;
  }
  virtual Reveal::Analytics::error_e analyze( Reveal::Core::solution_set_ptr in, Reveal::Core::analysis_ptr& out ) {
    assert( in );
    out = Reveal::Core::analysis_ptr( new Reveal::Core::analysis_c() );
    return ERROR_NONE;
  }

};

//-----------------------------------------------------------------------------
} // namespace Analytics
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_ANALYTICS_SCRIPT_H_
