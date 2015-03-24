#ifndef _REVEAL_CORE_IMPORTER_H_
#define _REVEAL_CORE_IMPORTER_H_

#include "Reveal/core/scenario.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class importer_c {
public:
  importer_c( void );
  virtual ~importer_c( void );

  // static?
  bool read_scenario( std::string filename, scenario_ptr& scenario );
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_IMPORTER_H_
