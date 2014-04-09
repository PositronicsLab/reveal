#ifndef _REVEAL_CORE_POINTERS_H_
#define _REVEAL_CORE_POINTERS_H_

//-----------------------------------------------------------------------------

#include <boost/shared_ptr.hpp>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Core {

//-----------------------------------------------------------------------------

class scenario_c;
typedef boost::shared_ptr<scenario_c> scenario_ptr;

class trial_c;
typedef boost::shared_ptr<trial_c> trial_ptr;

class solution_c;
typedef boost::shared_ptr<solution_c> solution_ptr;

class model_solution_c;
typedef boost::shared_ptr<model_solution_c> model_solution_ptr;

//-----------------------------------------------------------------------------

} // namespace Core

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_POINTERS_H_
