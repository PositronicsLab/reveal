#ifndef _REVEAL_CORE_EXPORTER_H_
#define _REVEAL_CORE_EXPORTER_H_

#include "Reveal/core/scenario.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class exporter_c {
public:
  exporter_c( void );
  virtual ~exporter_c( void );

  // static?
  bool write_scenario( scenario_ptr scenario, std::string filename );
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_EXPORTER_H_
