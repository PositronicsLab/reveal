#ifndef _REVEAL_CLIENT_CONSOLE_H_
#define _REVEAL_CLIENT_CONSOLE_H_

#include <string>
#include <vector>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Client {
//-----------------------------------------------------------------------------

class console_c {
public:
  static void print( std::string data ); 
  static void printline( std::string data ); 
  static void print( std::vector<std::string> data ); 
  static void print_param_array( char* const* array );
  static bool prompt_yes_no( std::string prompt );
  static std::string prompt( std::string prompt );
  static unsigned menu( std::string title, std::string prompt, std::vector<std::string> list );

};

//-----------------------------------------------------------------------------
} // namespace Client
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CLIENT_CONSOLE_H_
