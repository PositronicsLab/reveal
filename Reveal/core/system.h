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
/// Allocates and builds a parameter array compatible for use as args passed to 
/// a main function
/// @param params the set of strings to copy into the parameter array
/// @return the allocated parameter array
// TODO: move to a class to have destructor for correct cleanup.
char* const* param_array( std::vector< std::string > params );

//-----------------------------------------------------------------------------
/// Queries the system for the current environment variables
/// @return the set of environment variables returned by the system
std::vector< std::string > system_environment_vars( void );

//-----------------------------------------------------------------------------
/// Changes the process's current working directory to the specified path
/// @param path the path to set as the working directory
/// @return true if the working directory was changed OR false if the operation 
///         failed for any reason
/// Note: Currently POSIX only
bool change_working_dir( std::string path );

//-----------------------------------------------------------------------------
/// Split's a uri into constituent elements
/// @param uri the uri to split
/// @param protocol returned protocol on a successful split
/// @param host returned host on a successful split
/// @param port return port on a successful split
/// @return true if the uri was successfully split OR false if the operation 
///         failed for any reason
bool split_uri( std::string uri, std::string& protocol, std::string& host, unsigned& port );

//-----------------------------------------------------------------------------
/// Generates a unique identifier
/// @return the returned unique identifier
std::string generate_uuid( void );

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class sighandler_c {
public:
  /// Interface to install a signal handler
  virtual void install( void ) = 0; 
  /// Interface to uninstall a signal handler
  virtual void uninstall( void ) = 0;
};
//-----------------------------------------------------------------------------

#define DEFAULT_DATABASE_PORT 27017
#define DEFAULT_SERVER_PORT 20700

class system_c {
public:
  /// The enumerated set of Reveal components that require system knowledge
  enum side_e {
    CLIENT,    //< indicates client only side
    SERVER,    //< indicates at least server side
    DATABASE   //< indicates database component side
  };

  /// Parameterized constructor must be notified of what role this system has
  /// @param side the reveal component requesting system information
  system_c( side_e side );

  /// Destructor
  virtual ~system_c( void );

  /// Opens the system
  /// @return true if the system is successfully opened OR false if the 
  ///         operation fails for any reason
  bool open( void );

  /// Closes the system
  void close( void );

  /// Gets the server's uri from the system environment.  Typically a client 
  /// side request
  /// @return the server's uri
  std::string server_uri( void );

  /// Gets the database's uri from the system environment.  Typically a server
  /// side request
  /// @return the database's uri
  std::string database_uri( void );

  /// Gets the package installation path from the system environment
  /// @return the the package installation path
  std::string package_path( void );

  /// Gets the monitor port used to support monitoring of a simulator by the 
  /// client.  Typically a client side request
  /// @return the monitor port
  unsigned monitor_port( void );

  /// Gets the server host name or IP from the system environment.  Typically a
  /// server side request
  /// @return the host name of the server
  std::string server_host( void );

  /// Gets the server port number from the system environment.  Typically a 
  /// server side request
  /// @return the server port number  
  unsigned server_port( void );

  /// Gets the database host name or IP.  Typically a server side request
  /// @return the database host name or IP
  std::string database_host( void );

  /// Gets the database port number.  Typically a server side request
  /// @return the database port number
  unsigned database_port( void );

  /// Gets the database name.  Typically a server side request
  /// @return the name of the database
  std::string database_name( void );

private:
  std::string _working_path; //< Path to the working directory
  std::string _server_uri;   //< Server host uri
  std::string _database_uri; //< Database host uri
  std::string _package_path; //< Path to the local packages
  std::string _dbname;       //< Name of the database
  unsigned _monitor_port;    //< Monitor port used in ipc between client and sim
  side_e _side;              //< The service the system is supporting
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_SYSTEM_H_
