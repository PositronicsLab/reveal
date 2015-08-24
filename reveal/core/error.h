/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

handles the printing of error messages to the console
-----------------------------------------------------------------------------*/
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
  /// Prints an error to the console
  /// @param error the error message to print
  static void print( std::string error ) {
    std::cerr << error;
  }

  /// Prints an error to the console
  /// @param error the error message to print
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

