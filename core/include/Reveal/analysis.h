#ifndef _REVEAL_CORE_ANALYSIS_H_
#define _REVEAL_CORE_ANALYSIS_H_

//-----------------------------------------------------------------------------

#include <Reveal/solution.h>
#include <Reveal/trial.h>
#include <Reveal/scenario.h>

#include <boost/shared_ptr.hpp>
#include <vector>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Core {

//-----------------------------------------------------------------------------

class analysis_c;
typedef boost::shared_ptr<Reveal::Core::analysis_c> analysis_ptr;
//-----------------------------------------------------------------------------

class analysis_c {
public:
  analysis_c( void ) { }
  virtual ~analysis_c( void ) { }

  // TODO : add session data

  std::vector<double> data;

//  Reveal::Core::scenario_c 	scenario;
//  Reveal::Core::trial_c 	trial;             // set of trials
//  Reveal::Core::solution_c 	solution;
};

//-----------------------------------------------------------------------------

} // namespace Core

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_ANALYSIS_H_
