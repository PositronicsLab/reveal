#ifndef _REVEAL_CLIENT_SYSTEM_H_
#define _REVEAL_CLIENT_SYSTEM_H_

//-----------------------------------------------------------------------------

#include <string>
#include <vector>

#include "Reveal/core/pointers.h"
#include "Reveal/core/digest.h"

//-----------------------------------------------------------------------------
std::vector< std::string > system_environment_vars( void );
//bool copy_file( std::string src, std::string dest );
std::string make_temp_dir( void );
//-----------------------------------------------------------------------------
// TODO: move to utilities
void print_vector_of_strings( std::vector<std::string> v ); 
//-----------------------------------------------------------------------------
// TODO: move to utilities
void print_param_array( char* const* a );
//-----------------------------------------------------------------------------
// TODO: move to a class to have destructor for correct cleanup.
char* const* param_array( std::vector< std::string > params );
//-----------------------------------------------------------------------------
bool prompt_yes_no( std::string prompt, bool default_value );
//-----------------------------------------------------------------------------
void print_digest_menu( Reveal::Core::digest_ptr digest ); 
//-----------------------------------------------------------------------------
unsigned prompt_digest( Reveal::Core::digest_ptr digest );
//-----------------------------------------------------------------------------
unsigned prompt_select_string_from_list( std::string prompt, std::vector<std::string> list, unsigned default_value );

//-----------------------------------------------------------------------------
/*
class sighandler_c{
protected:
  static bool _quit;

public:
  sighandler_c( void ); 
  ~sighandler_c( void );

  static void handler( int signum );

  static bool quit( void ); 
  static void set( bool quit_ );

  void install( void );
  void uninstall( void );
  static void trip( void );
};
*/
//-----------------------------------------------------------------------------
class sighandler_c {
public:
  virtual void install( void ) = 0; 
  virtual void uninstall( void ) = 0;
};

//-----------------------------------------------------------------------------
class child_exit_handler_c : public sighandler_c {
protected:
  static bool _quit;

public:
  child_exit_handler_c( void ); 
  ~child_exit_handler_c( void );

  static void handler( int signum );

  static bool quit( void ); 
  static void set( bool quit_ );
  static void reset( void );

  virtual void install( void );
  virtual void uninstall( void );

  static void trip( void );
};

//-----------------------------------------------------------------------------
void* make_worker( void* args );
//-----------------------------------------------------------------------------
void* cmake_worker( void* args );
//-----------------------------------------------------------------------------
bool make_package( char* const* args );
//-----------------------------------------------------------------------------
bool make_package( std::string build_path );
//-----------------------------------------------------------------------------
bool cmake_package( char* const* args );
//-----------------------------------------------------------------------------
bool cmake_package( std::string source_path, std::string build_path );

/*
//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Client {
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
} // namespace Client
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
*/
#endif // _REVEAL_CLIENT_SYSTEM_H_
