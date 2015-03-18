#ifndef _REVEAL_CORE_ERROR_H_
#define _REVEAL_CORE_ERROR_H_

#include <iostream>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class error_c {
public:
  static void print( std::string error ) {
    std::cerr << error;
  }

  static void printline( std::string error ) {
    std::cerr << error << std::endl;
  }
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_ERROR_H_

