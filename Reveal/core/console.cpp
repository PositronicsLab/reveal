#include "Reveal/core/console.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <limits>
#include <cmath>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void console_c::print( std::string msg ) {
  printf( "%s", msg.c_str() );
}

//-----------------------------------------------------------------------------
void console_c::print( const std::stringstream& msg ) {
  printf( "%s", msg.str().c_str() );
}

//-----------------------------------------------------------------------------
void console_c::printline( std::string msg ) {
  printf( "%s\n", msg.c_str() );
}

//-----------------------------------------------------------------------------
void console_c::printline( const std::stringstream& msg ) {
  printf( "%s\n", msg.str().c_str() );
}

//-----------------------------------------------------------------------------
void console_c::error( std::string err ) {
  std::cerr << "ERROR: " << err << std::endl;
}

//-----------------------------------------------------------------------------
void console_c::error( const std::stringstream& err ) {
  std::cerr << "ERROR: " << err.str() << std::endl;
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
    std::cin.clear();
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
unsigned console_c::prompt_unsigned( std::string prompt, bool allow_zero ) {
  std::string input;
  
  // infinite validation loop.  only way out is for the user to enter a valid
  // unsigned value
  do {
    // print the prompt to the console
    printf( "%s: ", prompt.c_str() );
    // clear the input stream
    std::cin.clear();
    // get input from standard in
    std::getline( std::cin, input );

    // assume input is valid and invalidate if erroneous values
    bool valid = true;
    // iterate over the input buffer
    for( unsigned i = 0; i < input.size(); i++ ) {
      // read the current character
      char c = input[i];
      // if EOF and not first character then pass
      if( i > 0 && c == 0 ) break;
      // if not allow_zero zero is only character then fail
      if( !allow_zero && i == 0 && c == '0' && input.size() == 1 ) {
        valid = false;
        break;
      }
      // if not numeric, invalidate and fail
      if( c < '0' || c > '9' ) {
        valid = false;
        break;
      }
    }

    // if valid, return numeric value of input
    if( valid ) return (unsigned)atoi( input.c_str() );
    
    // if not valid, report and recycle
    printf( "ERROR: Invalid Input. Enter an unsigned integer\n" );

  } while( true );

  return 0; // unreachable. suppresses warning on return statement 
}

//-----------------------------------------------------------------------------
float console_c::prompt_float( std::string prompt, float& epsilon, bool allow_negative ) {
  std::string input;
  float value;
  float machine_epsilon = std::numeric_limits<float>::epsilon();
  
  // infinite validation loop.  only way out is for the user to enter a valid
  // float value
  do {
    // print the prompt to the console
    printf( "%s: ", prompt.c_str() );
    // clear the input stream
    std::cin.clear();
    // get input from standard in
    std::getline( std::cin, input );

    // send the input to an input stream for validation
    std::istringstream ss(input);

    if( ss >> value ) {
      // if valid conversion of input to value
      if( !allow_negative && value < 0.0 ) {
        // if negatives are invalid and input was negative, error and recycle
        printf( "ERROR: Invalid Input. Enter a floating point number\n" );
      } else {
        // if otherwise valid, compute epsilon and return value
        if( value > 10.0)
          epsilon = machine_epsilon * pow( 10.0, (int)log10( value ) );
        else
          epsilon = machine_epsilon * 10.0;
        return value;
      }
    } else {
      // the user entered invalid input
      printf( "ERROR: Invalid Input. Enter an unsigned floating point number\n" );
    }
  } while( true );

  return 0.0; // unreachable. suppresses warning on return statement 
}

//-----------------------------------------------------------------------------
double console_c::prompt_double( std::string prompt, double& epsilon, bool allow_negative ) {
  std::string input;
  double value;
  double machine_epsilon = std::numeric_limits<double>::epsilon();
  
  // infinite validation loop.  only way out is for the user to enter a valid
  // double value
  do {
    // print the prompt to the console
    printf( "%s: ", prompt.c_str() );
    // clear the input stream
    std::cin.clear();
    // get input from standard in
    std::getline( std::cin, input );

    // send the input to an input stream for validation
    std::istringstream ss(input);

    if( ss >> value ) {
      // if valid conversion of input to value
      if( !allow_negative && value < 0.0 ) {
        // if negatives are invalid and input was negative, error and recycle
        printf( "ERROR: Invalid Input. Enter a floating point number\n" );
      } else {
        // if otherwise valid, compute epsilon and return value
        if( value > 10.0)
          epsilon = machine_epsilon * pow( 10.0, (int)log10( value ) );
        else
          epsilon = machine_epsilon * 10.0;
        return value;
      }
    } else {
      // the user entered invalid input
      printf( "ERROR: Invalid Input. Enter an unsigned floating point number\n" );
    }
  } while( true );

  return 0.0; // unreachable. suppresses warning on return statement 
}

//-----------------------------------------------------------------------------
std::string console_c::prompt( std::string prompt ) {
  std::string input;

  // print the prompt to the console
  printf( "%s ", prompt.c_str() );
  // clear the input stream
  std::cin.clear();
  // get input from standard in
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
    std::cin.clear();
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
} // namespace Core
//-----------------------------------------------------------------------------

