/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

console.h defines methods to print and methods to validate prompts from users 
in the console
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_CONSOLE_H_
#define _REVEAL_CORE_CONSOLE_H_

#include <string>
#include <sstream>
#include <vector>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class console_c {
public:
  /// prints an unmodified string to the console.
  /// @param msg the string to print
  static void print( std::string msg );

  /// prints an unmodified stringstream to the console.
  /// @param msg the stringstream to print
  static void print( const std::stringstream& msg );

  /// prints a string with a trailing newline character to the console 
  /// @param msg the string to print
  static void printline( std::string msg );

  /// prints a stringstream with a trailing newline character to the console 
  /// @param msg the stringstream to print
  static void printline( const std::stringstream& msg );

  /// prints an error string message with trailing newline character to the 
  /// console (stderr).  Formats the message to include preceding ERROR tag
  /// @param err the string to print
  static void error( std::string err );

  /// prints an error stringstream message with trailing newline character
  /// to the console (stderr).  Formats the message to include preceding ERROR 
  /// tag
  /// @param err the stringstream to print
  static void error( const std::stringstream& err );

  /// prints a vector of strings to the console.  Each string will have a 
  /// newline character printed following it.
  /// @param data the vector of strings to print
  static void print( std::vector<std::string> data );

  /// prints a parameter array (a set of c strings) to the console.  Each 
  /// c-string will have a newline character printed following it. 
  /// @param array the array of c-strings to print
  static void print_param_array( char* const* array );

  /// prompts the user to answer a yes or no question.  If the input is invalid
  /// the prompt will recycle until a valid input is provided
  /// @param prompt the prompt to print preceding release of control to input
  /// @return true if the user answered yes OR false if the user answered no
  static bool prompt_yes_no( std::string prompt );

  /// prompts the user to answer a question.  The input is not validated
  /// @param prompt the prompt to print preceding release of control to input
  /// @return the user's unformatted input
  static std::string prompt( std::string prompt );

  /// prompts the user to enter an unsigned integer.  If the input is invalid
  /// the prompt will recycle until a valid input is provided
  /// @param prompt the prompt to print preceding release of control to input
  /// @param allow_zero defines whether a zero value is acceptable as input
  /// @return the validated unsigned input
  static unsigned prompt_unsigned( std::string prompt, bool allow_zero = false );

  /// prompts the user to enter a floating point number.  If the input is 
  /// invalid the prompt will recycle until a valid input is provided
  /// @param prompt the prompt to print preceding release of control to input
  /// @param allow_negative defines whether a negative value is acceptable as
  ///        input
  /// @return the validated floating point input
  static float prompt_float( std::string prompt, bool allow_negative = false );

  /// prompts the user to select from a menu.  If the input is invalid the 
  /// prompt will recycle until a valid input is provided
  /// @param title the title of the menu
  /// @param prompt the prompt to print following the menu
  /// @param list the vector of strings to select from
  /// @return the valid numeric index associated with the list provided
  static unsigned menu( std::string title, std::string prompt, std::vector<std::string> list );

};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_CONSOLE_H_
