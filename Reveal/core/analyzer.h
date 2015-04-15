/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

analyzer.h defines the analyzer_c interface that defines custom analytic
programs to run on a specified scenario
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_ANALYZER_H_
#define _REVEAL_CORE_ANALYZER_H_

//-----------------------------------------------------------------------------

#include <string>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class analyzer_c;
typedef boost::shared_ptr<Reveal::Core::analyzer_c> analyzer_ptr;

//-----------------------------------------------------------------------------

class analyzer_c {
public:
  enum type_e {
    PLUGIN,
    SCRIPT
  };

  analyzer_c( void ) { }
  virtual ~analyzer_c( void ) { }

  std::string            scenario_id;
  std::string            filename;
  type_e 	         type;

  // ?
  std::vector<std::string>  keys;
  std::vector<std::string>  labels; 
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_ANALYZER_H_
