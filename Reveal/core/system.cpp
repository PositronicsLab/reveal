#include "Reveal/core/system.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>

#include <boost/filesystem.hpp>

//-----------------------------------------------------------------------------
bool path_exists( std::string path ) {
  boost::system::error_code ec;
  bool result = boost::filesystem::exists( path, ec );

  if( ec != boost::system::errc::success ) {
    std::cerr << ec.message() << std::endl;
  }
  return result; 
}
//-----------------------------------------------------------------------------
bool file_exists( std::string path ) {
  boost::system::error_code ec;

/*
  struct stat st;
  stat( path.c_str(), &st );
  if( S_ISREG( st.st_mode ) )
    return true;
  return false;
*/
  if( !path_exists( path ) ) return false;
  //boost::filesystem::path full_path = root_path;

  if( !boost::filesystem::is_regular_file( boost::filesystem::path(path), ec ) ) {
    std::cerr << ec.message() << std::endl;
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
bool get_directory( std::string path ) {

  boost::system::error_code ec;
  if( !boost::filesystem::exists( path, ec ) ) {
    // if the directory does not exist, create the directory
    if( !boost::filesystem::create_directory( path, ec ) ) {
      // failed to create directory.  take no further action and bail
      std::cerr << ec.message() << std::endl;
      return false;
    }
  } else {
    // verify that it is a directory
    if( !boost::filesystem::is_directory( path, ec ) ) {
      // path exists but not as a directory.  take no action and bail
      std::cerr << ec.message() << std::endl;
      return false;
    }
  }
  //printf( "created directory: %s\n", path.c_str() );

  return true;
}

//-----------------------------------------------------------------------------
std::string combine_path( std::string root_path, std::string relative_child ) {
  boost::filesystem::path full_path = root_path;
  full_path /= relative_child;
  return full_path.string();
}

//-----------------------------------------------------------------------------
bool get_temp_directory( std::string& path ) {

/*
  char tmp_path_template[] = "/tmp/reveal-XXXXXX";
  std::string tmp_path = mkdtemp( tmp_path_template );
  //printf( "tmp_path: %s\n", tmp_path.c_str() );
*/
  boost::filesystem::path tmp_path;
  boost::filesystem::path generated_tmp_path;

  try {
    tmp_path = boost::filesystem::temp_directory_path();
    generated_tmp_path = boost::filesystem::unique_path("Reveal-%%%%%%");
  } catch(const boost::system::error_code &_ex) {
    std::cerr << "Failed generating temp directory name. Reason: "
          << _ex.message() << "\n";
    return false;
  }

  boost::filesystem::path full_path = tmp_path;
  full_path /= generated_tmp_path;
  
/*
  std::cout << "tmp_path: " << tmp_path.string() << std::endl;
  std::cout << "generated_tmp_path: " << generated_tmp_path.string() << std::endl;
  std::cout << "full_path: " << full_path.string() << std::endl;
*/
  path = full_path.string();
 
  //std::cout << "temp directory: " << path << std::endl;

  if( !get_directory( path ) )
    return false;
 
  return true;
}

//-----------------------------------------------------------------------------
bool copy_file( std::string source_path, std::string target_path ) {
  boost::system::error_code ec;
  boost::filesystem::path src_path( source_path );
  boost::filesystem::path tgt_path( target_path );

  copy( src_path, tgt_path, ec );
  if( ec != boost::system::errc::success ) {
    // TODO handle
    std::cerr << ec.message() << std::endl;
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
bool copy_directory( std::string source_path, std::string target_path ) {
  boost::system::error_code ec;

  // get the directory
  if( !get_directory( target_path ) ) return false;

  // clean this directory, i.e. remove all children but not this directory
  if( !clean_directory( target_path ) ) return false;

  boost::filesystem::path src_path( source_path );
  boost::filesystem::path tgt_path( target_path );

  typedef std::vector<boost::filesystem::path> paths_t;
  paths_t src_paths;

  copy( boost::filesystem::directory_iterator( src_path ),
        boost::filesystem::directory_iterator(),
        back_inserter( src_paths ) );

  for( paths_t::const_iterator it = src_paths.begin(); it != src_paths.end(); it++ ) {

    boost::filesystem::path::iterator rel_path = --it->end();
    boost::filesystem::path next_src_path = *it;
    boost::filesystem::path next_tgt_path = tgt_path;
    next_tgt_path /= *rel_path;
    //std::cout << next_src_path << " -> " << next_tgt_path << '\n';
    
    if( boost::filesystem::is_directory( next_src_path, ec ) ) {
      // if it is a directory, recurse
      if( !copy_directory( next_src_path.string(), next_tgt_path.string() ) ) {
        return false;
      }
    } else {
      // otherwise, let boost handle the copy
      copy( next_src_path, next_tgt_path, ec );
      if( ec != boost::system::errc::success ) {
        std::cerr << ec.message() << std::endl;
        return false;
      }
    }
  }

  return true;
}
//-----------------------------------------------------------------------------
bool clean_directory( std::string path ) {
  boost::filesystem::path top_path( path );

  boost::system::error_code ec;
  if( !boost::filesystem::exists( top_path, ec ) ) {
    // if the directory does not exist, bail out
    std::cerr << ec.message() << std::endl;
    return false;
  }

  // verify that it is a directory
  if( !boost::filesystem::is_directory( top_path, ec ) ) {
    // path exists but not as a directory.  take no action and bail
    std::cerr << ec.message() << std::endl;
    return false;
  }

  typedef std::vector<boost::filesystem::path> paths_t;
  paths_t paths;

  copy( boost::filesystem::directory_iterator( top_path ),
        boost::filesystem::directory_iterator(),
        back_inserter( paths ) );

  for( paths_t::const_iterator it = paths.begin(); it != paths.end(); it++ ) {
    //std::cout << it->string() << std::endl;

    if( boost::filesystem::is_directory( *it, ec ) ) {
      if( !remove_directory( it->string() ) ) {
        return false;
      }
    } else {
      if( !boost::filesystem::remove( *it, ec ) ) {
        std::cerr << ec.message() << std::endl;
        return false;
      }
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
bool remove_directory( std::string path ) {
  boost::system::error_code ec;
  boost::filesystem::path _path( path );

  if( !boost::filesystem::exists( _path, ec ) ) {
    // if the directory does not exist, bail out
    std::cerr << ec.message() << std::endl;
    return false;
  }

  // verify that it is a directory
  if( !boost::filesystem::is_directory( _path, ec ) ) {
    // path exists but not as a directory.  take no action and bail
    std::cerr << ec.message() << std::endl;
    return false;
  }

  boost::filesystem::remove_all( _path, ec );
  if( ec != boost::system::errc::success ) {
    std::cerr << ec.message() << std::endl;
    return false;
  }

  return true;
}
//-----------------------------------------------------------------------------
// TODO: move to a class to have destructor for correct cleanup.
char* const* param_array( std::vector< std::string > params ) {

  const char** pa = (const char**)malloc( sizeof(char*) * params.size() + 1 );
  for( unsigned i = 0; i < params.size(); i++ ) {
    pa[i] = (const char*)params[i].c_str();
  }
  pa[ params.size() ] = NULL;

  return (char* const*) pa;
}

//-----------------------------------------------------------------------------
extern char **environ;
//-----------------------------------------------------------------------------
std::vector< std::string > system_environment_vars( void ) {
  std::vector<std::string> ev;

  for (char **env = environ; *env; ++env) {
    ev.push_back( *env );
  }

  return ev;
}

//-----------------------------------------------------------------------------
bool change_working_dir( std::string path ) {
  if( chdir( path.c_str() ) == -1 ) {
    if( errno == EACCES )
      std::cerr << "ERROR: Permission denied" << std::endl;
    else if( errno == EFAULT ) 
      std::cerr << "ERROR: Path outside of accessible address space" << std::endl;
    else if( errno == EIO ) 
      std::cerr << "ERROR: I/O error" << std::endl;
    else if( errno == ELOOP ) 
      std::cerr << "ERROR: Too many symbolic links were encountered in resolving path" << std::endl;
    else if( errno == ENAMETOOLONG ) 
      std::cerr << "ERROR: path too long" << std::endl;
    else if( errno == ENOENT ) 
      std::cerr << "ERROR: The file does not exist" << std::endl;
    else if( errno == ENOMEM ) 
      std::cerr << "ERROR: Insufficient kernel memory available" << std::endl;
    else if( errno == ENOTDIR ) 
      std::cerr << "ERROR: A component of path is not a directory" << std::endl;
    else
      std::cerr << "ERROR: Unspecifed" << std::endl;
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
bool split_uri( std::string uri, std::string& protocol, std::string& host, unsigned& port ) {
  size_t index1 = uri.find("://");
  size_t index2 = uri.find( ":", index1 + 3 );

  if( index1 == std::string::npos || index1 == uri.size() ) {
    // protocol not present
    return false;
  }
  
  if( index2 == std::string::npos || index2 == uri.size() ) {
    // host or port not present
    return false;
  }

  protocol = uri.substr( 0, index1 );
  host = uri.substr( index1 + 3, index2 - (index1 + 3) );
  port = (unsigned) atoi( uri.substr( index2 + 1 ).c_str() );

  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

system_c::system_c( side_e side ) {
  _side = side;
}

//-----------------------------------------------------------------------------
system_c::~system_c( void ) {
  close();
}

//-----------------------------------------------------------------------------
bool system_c::open( void ) {
  std::string protocol, host;
  unsigned port;

  // get the home directory
  std::string home = getenv("HOME");

  // build the working directory path
  std::string working_path = home + "/.Reveal";

  // if we can't get the working_path, then there is something seriously wrong
  // i.e. permissions, home directory, etc.
  if( !get_directory( working_path ) ) return false;

  // working path is valid
  _working_path = working_path;

  // get Reveal environment
  char* server_uri = getenv( "REVEAL_SERVER_URI" );
  char* database_uri = getenv( "REVEAL_DATABASE_URI" );
  char* monitor_port = getenv( "REVEAL_MONITOR_PORT" );
  char* package_path = getenv( "REVEAL_PACKAGE_PATH" );
  char* dbname = getenv( "REVEAL_DATABASE_NAME" );

  // validate Reveal environment
  if( ( _side == SERVER || _side == DATABASE ) && database_uri == NULL ) {
    // server side issue
    // no database_uri in environment.
    // TODO: error report
    return false;
  }
  if( database_uri == NULL ) {
    std::stringstream uri;
    uri << "localhost:" << DEFAULT_DATABASE_PORT;
    _database_uri = uri.str();
  } else
    _database_uri = database_uri;
  if( !split_uri( _database_uri, protocol, host, port ) ) return false;

  if( _side == CLIENT && server_uri == NULL ) {
    // client side issue.
    // no server_uri in environment.  
    // TODO: error report
    return false;
  }
  if( server_uri == NULL ) {
    std::stringstream uri;
    uri << "localhost:" << DEFAULT_SERVER_PORT;
    _server_uri = uri.str();
  } else
    _server_uri = server_uri;
  if( !split_uri( _server_uri, protocol, host, port ) ) return false;

  if( _side == CLIENT && monitor_port == NULL ) {
    // client side issue
    // no monitor port for package interfaces
    // TODO: error report
    return false;
  }
  if( monitor_port == NULL )
    _monitor_port = DEFAULT_SERVER_PORT + 1;
  else
    _monitor_port = atoi( monitor_port );

  if( package_path == NULL ) {
    // client side issue
    // no package path so use the working path
    _package_path = _working_path + "/packages";
  } else {
    _package_path = package_path;
  }
  std::cout << "package_path: " << _package_path << std::endl;

  if( _side == DATABASE && dbname == NULL ) {
    return false;
  } else
    _dbname = dbname;

  return true;
}

//-----------------------------------------------------------------------------
void system_c::close( void ) {

}

//-----------------------------------------------------------------------------
std::string system_c::server_uri( void ) {
  return _server_uri;
}

//-----------------------------------------------------------------------------
std::string system_c::database_uri( void ) {
  return _database_uri;
}

//-----------------------------------------------------------------------------
std::string system_c::package_path( void ) {
  return _package_path;
}

//-----------------------------------------------------------------------------
unsigned system_c::monitor_port( void ) {
  return _monitor_port;
}

//-----------------------------------------------------------------------------
std::string system_c::server_host( void ) {
  std::string protocol, host;
  unsigned port;

  split_uri( _server_uri, protocol, host, port );
  return host;
}

//-----------------------------------------------------------------------------
unsigned system_c::server_port( void ) {
  std::string protocol, host;
  unsigned port;

  split_uri( _server_uri, protocol, host, port );
  return port;
}

//-----------------------------------------------------------------------------
std::string system_c::database_host( void ) {
  std::string protocol, host;
  unsigned port;

  split_uri( _database_uri, protocol, host, port );
  return host;
}

//-----------------------------------------------------------------------------
unsigned system_c::database_port( void ) {
  std::string protocol, host;
  unsigned port;

  split_uri( _database_uri, protocol, host, port );
  return port;
}

//-----------------------------------------------------------------------------
std::string system_c::database_name( void ) {
  return _dbname;
}

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
