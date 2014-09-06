/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

pointer.h forward declares classes required across Reveal libraries and declares
boost shared pointers for Reveal interfaces
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_POINTERS_H_
#define _REVEAL_CORE_POINTERS_H_

//-----------------------------------------------------------------------------

#include <boost/shared_ptr.hpp>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Core {

//-----------------------------------------------------------------------------

class digest_c;
typedef boost::shared_ptr<digest_c> digest_ptr;

class scenario_c;
typedef boost::shared_ptr<scenario_c> scenario_ptr;

class trial_c;
typedef boost::shared_ptr<trial_c> trial_ptr;

class solution_c;
typedef boost::shared_ptr<solution_c> solution_ptr;

class solution_set_c;
typedef boost::shared_ptr<solution_set_c> solution_set_ptr;

//-----------------------------------------------------------------------------

} // namespace Core

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_POINTERS_H_
