#ifndef _REVEAL_ANALYTICS_MODULE_H_
#define _REVEAL_ANALYTICS_MODULE_H_

//-----------------------------------------------------------------------------

#include <Reveal/pointers.h>
#include <Reveal/solution_set.h>
#include <Reveal/analytics/types.h>
#include <Reveal/analysis.h>

#include <boost/enable_shared_from_this.hpp>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Analytics {

//-----------------------------------------------------------------------------

class module_c : public boost::enable_shared_from_this<module_c>  {
public:

  module_c( void ) {}
  virtual ~module_c( void ) {}

  virtual Reveal::Analytics::error_e load( std::string file ) { return ERROR_NONE; };
  virtual Reveal::Analytics::error_e analyze( Reveal::Core::solution_set_ptr in, Reveal::Core::analysis_ptr& out ) { return ERROR_NONE; }
};

//-----------------------------------------------------------------------------

} // namespace Analytics

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_ANALYTICS_MODULE_H_
