#ifndef _REVEAL_CORE_SYSTEM_H_
#define _REVEAL_CORE_SYSTEM_H_

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
bool split_uri( std::string uri, std::string& protocol, std::string& host, unsigned& port );

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class sighandler_c {
public:
  virtual void install( void ) = 0; 
  virtual void uninstall( void ) = 0;
};
//-----------------------------------------------------------------------------

#define DEFAULT_DATABASE_PORT 27017
#define DEFAULT_SERVER_PORT 20700

class system_c {
public:
  enum side_e {
    CLIENT,
    SERVER,
    DATABASE
  };

  system_c( side_e side );
  virtual ~system_c( void );

  bool open( void );
  void close( void );

  std::string server_uri( void );
  std::string database_uri( void );
  std::string package_path( void );
  unsigned monitor_port( void );

  std::string server_host( void );
  unsigned server_port( void );
  std::string database_host( void );
  unsigned database_port( void );

  std::string database_name( void );

private:
  /// validates the existance of the directory or creates it if it doesn't exist
  /// @param path the path to the directory to validate
  /// @return true if the directory is valid OR false if unable to create or 
  ///         validate the directory
  bool get_directory( std::string path );
private:
  /// The path to the working directory.  Root of all local data.
  std::string _working_path;

  std::string _server_uri;
  std::string _database_uri;
  std::string _package_path;

  std::string _dbname;

  unsigned _monitor_port;

  side_e _side;
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_SYSTEM_H_
