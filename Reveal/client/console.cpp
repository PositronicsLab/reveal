#include "Reveal/client/console.h"

#include <stdio.h>
#include <iostream>
#include <sstream>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Client {
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void console_c::print( std::string data ) {
  printf( "%s", data.c_str() );
}

//-----------------------------------------------------------------------------
void console_c::printline( std::string data ) {
  printf( "%s\n", data.c_str() );
}

//-----------------------------------------------------------------------------
void console_c::print( std::vector<std::string> data ) {
  for( std::vector<std::string>::iterator it = data.begin(); it != data.end(); it++ )
    printf( "%s\n", it->c_str() );
}

//-----------------------------------------------------------------------------
void console_c::print_param_array( char* const* array ) {
  for( char* const* ptr = (char* const*)array; *ptr != NULL; ptr++ )
    printf( "%s\n", *ptr );
}

//-----------------------------------------------------------------------------
bool console_c::prompt_yes_no( std::string prompt ) {
  std::string input;
  
  do {
    printf( "%s ", prompt.c_str() );
    std::getline( std::cin, input );

    if( input == "y" || input == "Y" ) {
      return true;
    } else if( input == "n" || input == "N" ) {
      return false;
    }
    printf( "ERROR: Invalid Input. Enter y or n\n" );

  } while( true );

  return false;
}

//-----------------------------------------------------------------------------
std::string console_c::prompt( std::string prompt ) {
  std::string input;
  
  printf( "%s ", prompt.c_str() );
  std::getline( std::cin, input );

  return input;
}

//-----------------------------------------------------------------------------
// Note: the 1-base offset in choice exists because stringstream(input) sets 
// choice to 0 if an invalid value, e.g. a string of characters, is entered via
// cin.
unsigned console_c::menu( std::string title, std::string prompt, std::vector< std::string > list ) {
  unsigned choice;
  unsigned range = list.size();
  std::string input;
  
  printf( "%s\n", title.c_str() );

  for( unsigned i = 0; i < list.size(); i++ )
    printf( "%d: %s\n", i+1, list[i].c_str() );

  do {
    printf( "%s: ", prompt.c_str() );
    std::getline( std::cin, input );

    std::stringstream( input ) >> choice;

    if( choice == 0 || choice > range )
      printf( "ERROR: Invalid Input. Enter a value in the range [1,%u]\n", range );

  } while( choice == 0 || choice > range );

  choice--;  // to fix the 1-base offset back to 0-base
  return choice;
}


//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
} // namespace Client
//-----------------------------------------------------------------------------

