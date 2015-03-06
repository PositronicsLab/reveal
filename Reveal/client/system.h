#ifndef _REVEAL_CLIENT_SYSTEM_H_
#define _REVEAL_CLIENT_SYSTEM_H_

//-----------------------------------------------------------------------------

#include <string>
#include <vector>

//-----------------------------------------------------------------------------
std::vector< std::string > system_environment_vars( void );
//-----------------------------------------------------------------------------
//bool copy_file( std::string src, std::string dest );
//-----------------------------------------------------------------------------
// TODO: refactor to boost::filesystem
std::string make_temp_dir( void );
//-----------------------------------------------------------------------------
// TODO: move to a class to have destructor for correct cleanup.
char* const* param_array( std::vector< std::string > params );
//-----------------------------------------------------------------------------
// TODO: refactor to boost::filesystem
bool file_exists( std::string path );
//-----------------------------------------------------------------------------
bool change_working_dir( std::string path );

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Client {
//-----------------------------------------------------------------------------

class sighandler_c {
public:
  virtual void install( void ) = 0; 
  virtual void uninstall( void ) = 0;
};

//-----------------------------------------------------------------------------
} // namespace Client
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CLIENT_SYSTEM_H_
