/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

system.h provides various interfaces for interacting with the underlying 
operating system, with generalized systems, and with the Reveal system itself. 
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_SYSTEM_H_
#define _REVEAL_CORE_SYSTEM_H_

//-----------------------------------------------------------------------------

#include <string>
#include <vector>

//-----------------------------------------------------------------------------
/// validates whether or not a path exists within the filesystem
/// @param path the path to validate
/// @return true if the path exists OR false if the path does not exist
bool path_exists( std::string path );
//-----------------------------------------------------------------------------
/// validates whether or not a file exists within the filesystem
/// @param path the path to the file to validate
/// @return true if the file exists OR false if the file does not exist
bool file_exists( std::string path );
//-----------------------------------------------------------------------------
/// validates the existance of the directory or creates it if it doesn't exist
/// @param path the path to the directory to validate
/// @return true if the directory is valid OR false if unable to create or 
///         validate the directory
bool get_directory( std::string path );
//-----------------------------------------------------------------------------
/// concatenates a relative path to a root path
/// @param root_path a system path defining the root of the new path 
/// @param relative_child a relative path to a location to be considered a 
///        child of the root
/// @return the concatenated path formed by appending the child to the end of 
///         the root
std::string combine_path( std::string root_path, std::string relative_child );
//-----------------------------------------------------------------------------
/// given a path, returns the file stem including path
/// @param path a path to a file
/// @return the stem of the file including the path
std::string get_stem( std::string path );
//-----------------------------------------------------------------------------
/// given a path, returns the file extension
/// @param path a path to a file
/// @return the file extension of the file specified by path including the 
///         preceding dot
std::string get_extension( std::string path );
//-----------------------------------------------------------------------------
/// creates a temporary directory and returns the path to the directory in the 
/// path parameter
/// @param path the newly created temporary path.  a return value.
/// @return true if the temporary directory was created OR false if the 
///         operation failed in any way.
bool get_temp_directory( std::string& path );
//-----------------------------------------------------------------------------
/// copies a file from the source path to the target path
/// @param source_path the path to copy the file from
/// @param target_path the path to copy the file to
/// @return true if the file was copied OR false if the operation failed in 
///         any way.
bool copy_file( std::string source_path, std::string target_path );
//-----------------------------------------------------------------------------
/// recursively copies a directory from the source path to the target path
/// @param source_path the path to copy the directory from
/// @param target_path the path to copy the directory to
/// @return true if the directory was copied OR false if the operation failed 
///         in any way.
bool copy_directory( std::string source_path, std::string target_path );
//-----------------------------------------------------------------------------
/// recursively removes everything within a directory without removing the 
/// directory itself.
/// @param path the path to the directory to clean
/// @return true if the directory was cleaned OR false if the operation failed 
///         in any way
bool clean_directory( std::string path );
//-----------------------------------------------------------------------------
/// recursively removes everything within a directory and the directory itself.
/// @param path the path to the directory to remove
/// @return true if the directory was removed OR false if the operation failed 
///         in any way
bool remove_directory( std::string path );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// TODO: move to a class to have destructor for correct cleanup.
char* const* param_array( std::vector< std::string > params );
//-----------------------------------------------------------------------------
std::vector< std::string > system_environment_vars( void );

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
