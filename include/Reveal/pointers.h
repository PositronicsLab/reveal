#ifndef _REVEAL_POINTERS_H_
#define _REVEAL_POINTERS_H_

#include <boost/shared_ptr.hpp>

namespace Reveal {

class Scenario;
typedef boost::shared_ptr<Scenario> ScenarioPtr;

class Trial;
typedef boost::shared_ptr<Trial> TrialPtr;

class Solution;
typedef boost::shared_ptr<Solution> SolutionPtr;

} // namespace Reveal

#endif // _REVEAL_POINTERS_H_
