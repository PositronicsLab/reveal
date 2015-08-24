#include "reveal/core/system.h"

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

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

//-----------------------------------------------------------------------------
bool path_exists( std::string path ) {
  boost::system::error_code ec;
  bool result = boost::filesystem::exists( path, ec );
/*
  if( ec != boost::system::errc::success ) {
    std::cerr << ec.message() << std::endl;
  }
*/
  return result; 
}
//-----------------------------------------------------------------------------
bool file_exists( std::string path ) {
//  boost::system::error_code ec;

/*
  struct stat st;
  stat( path.c_str(), &st );
  if( S_ISREG( st.st_mode ) )
    return true;
  return false;
*/
  if( !path_exists( path ) ) return false;
  //boost::filesystem::path full_path = root_path;

  return is_file( path );
/*
  if( !boost::filesystem::is_regular_file( boost::filesystem::path(path), ec ) ) {
    std::cerr << ec.message() << std::endl;
    return false;
  }

  return true;
*/
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
std::string get_stem( std::string path ) {
  boost::filesystem::path _path = path;
  return _path.stem().string();
}

//-----------------------------------------------------------------------------
std::string get_extension( std::string path ) {
  boost::filesystem::path _path = path;
  return _path.extension().string();
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
bool is_file( const std::string& path ) {
  boost::system::error_code ec;
  boost::filesystem::path p( path );
  return boost::filesystem::is_regular_file( p, ec );
}

//-----------------------------------------------------------------------------
bool is_directory( const std::string& path ) {
  boost::system::error_code ec;
  boost::filesystem::path p( path );
  return boost::filesystem::is_directory( p, ec );
}

//-----------------------------------------------------------------------------
bool find_file( std::string& path_to_file, const std::string& file_to_find, const std::string& search_path, bool recursive ) {
  if( !path_exists( search_path ) ) return false; // search_path doesn't exist
  if( !is_directory( search_path ) ) return false;// search_path not a directory
  
  // search_path exists and is a directory
  
  // check the current path for the file
  std::string path = combine_path( search_path, file_to_find );
  if( file_exists( path ) ) {
    path_to_file = path;
    return true;
  }

  // if not recursive, then failed to find file.
  if( !recursive ) return false;

  // otherwise enumerate all the subdirectories in the directory and search 
  // within those locations.
  std::vector<std::string> child_dirs = get_subdirectories( search_path );

  for( std::vector<std::string>::iterator it = child_dirs.begin(); it != child_dirs.end(); it++ ) {
    std::string sub_path = *it;
    try {
      bool result = find_file( path_to_file, file_to_find, sub_path, true );
      if( result ) return result;
    } catch( std::exception ex ) {
 
    }
  }
 
  // otherwise failed to find file.
  return false;
}

//-----------------------------------------------------------------------------
std::string get_current_path( void ) {
  return boost::filesystem::absolute( boost::filesystem::current_path() ).string();
}

//-----------------------------------------------------------------------------
std::vector<std::string> get_subdirectories( const std::string& path ) {
  std::vector<std::string> subdirs;
  // validate that the path exists and that the path is a directory
  if( !path_exists( path ) || !is_directory( path ) ) return subdirs;

  std::vector<boost::filesystem::path> v;

  copy( boost::filesystem::directory_iterator(path), boost::filesystem::directory_iterator(), back_inserter(v) );

  for( std::vector<boost::filesystem::path>::iterator it = v.begin(); it != v.end(); it++ ) {
    std::string p = it->string();
    if( is_directory( p ) ) subdirs.push_back( p );
  }
  return subdirs;
}

//-----------------------------------------------------------------------------
std::vector<std::string> split_multipath_string( std::string paths ) {
/*
  std::string token, delim = ":";
  std::string::size_type pos = 0, start = 0;
  std::vector<std::string> paths;

  do {
    pos = path_str.find( delim, start );
    if( pos == std::string::npos ) {
      token = path_str.substr( start );
    } else {
      token = path_str.substr( start, pos - start );
      start = pos + 1;
    }
    paths.push_back( token );
  } while( pos != std::string::npos );
  return paths;
*/
  return split_multipart_string( paths, ":" );
}

//-----------------------------------------------------------------------------
std::vector<std::string> split_multipart_string( std::string string, std::string delim ) {
  std::string token;
  std::string::size_type pos = 0, start = 0;
  std::vector<std::string> strings;

  do {
    pos = string.find( delim, start );
    if( pos == std::string::npos ) {
      token = string.substr( start );
    } else {
      token = string.substr( start, pos - start );
      start = pos + 1;
    }
    strings.push_back( token );
  } while( pos != std::string::npos );
  return strings;
}

//-----------------------------------------------------------------------------
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
std::string generate_uuid( void ) {

  std::stringstream ss;
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  ss << uuid;
  return ss.str();
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
  std::string working_path = home + "/.reveal";

  // if we can't get the working_path, then there is something seriously wrong
  // i.e. permissions, home directory, etc.
  if( !get_directory( working_path ) ) return false;

  // working path is valid
  _working_path = working_path;

  // get Reveal environment
  char* analytics_db_name = getenv( "REVEAL_ANALYTICS_DATABASE_NAME" );
  char* samples_db_name = getenv( "REVEAL_SAMPLES_DATABASE_NAME" );
  char* database_uri = getenv( "REVEAL_DATABASE_URI" );
  char* analytics_uri = getenv( "REVEAL_ANALYTICS_URI" );
  char* samples_uri = getenv( "REVEAL_SAMPLES_URI" );
  char* monitor_port = getenv( "REVEAL_MONITOR_PORT" );
  char* analyzer_port = getenv( "REVEAL_ANALYZER_PORT" );
  char* packages_path = getenv( "REVEAL_PACKAGES_PATH" );
  char* sims_path = getenv( "REVEAL_SIMS_PATH" );
  char* sims_bin_search_path = getenv( "REVEAL_BIN_SEARCH_PATH" );
  char* sims_lib_search_path = getenv( "REVEAL_LIB_SEARCH_PATH" );

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

  if( _side == CLIENT && samples_uri == NULL ) {
    // client side issue.
    // no server_uri in environment.  
    // TODO: error report
    return false;
  }
  if( samples_uri == NULL ) {
    std::stringstream uri;
    uri << "localhost:" << DEFAULT_SAMPLES_PORT;
    _samples_uri = uri.str();
  } else
    _samples_uri = samples_uri;
  if( !split_uri( _samples_uri, protocol, host, port ) ) return false;

  if( analytics_uri == NULL ) {
    std::stringstream uri;
    uri << "localhost:" << DEFAULT_ANALYTICS_PORT;
    _analytics_uri = uri.str();
  } else
    _analytics_uri = analytics_uri;
  if( !split_uri( _analytics_uri, protocol, host, port ) ) return false;

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

  if( analyzer_port == NULL )
    _analyzer_port = DEFAULT_SERVER_PORT + 2;
  else
    _analyzer_port = atoi( monitor_port );

  if( packages_path == NULL ) {
    // client side issue
    // no package path so use the working path
    _packages_path = _working_path + "/packages";
  } else {
    _packages_path = packages_path;
  }
  //std::cout << "package_path: " << _package_path << std::endl;

  if( sims_path == NULL ) {
    // client side issue
    // no sims path so use the working path
    _sims_path = _working_path + "/sims";
  } else {
    _sims_path = sims_path;
  }
  //std::cout << "sims_path: " << _sims_path << std::endl;

  if( sims_bin_search_path == NULL ) {
    // client side issue
    // no sims path so use the working path
    _sims_bin_search_path = _working_path + "/sims";
  } else {
    _sims_bin_search_path = sims_bin_search_path;
  }
  //std::cout << "sims_bin_search_path: " << _sims_bin_search_path << std::endl;

  if( sims_lib_search_path == NULL ) {
    // client side issue
    // no sims path so use the working path
    _sims_lib_search_path = _working_path + "/sims";
  } else {
    _sims_lib_search_path = sims_lib_search_path;
  }
  //std::cout << "sims_lib_search_path: " << _sims_lib_search_path << std::endl;

  if( _side == DATABASE && samples_db_name == NULL && analytics_db_name == NULL ) {
    return false;
  } else {
    _samples_db_name = samples_db_name;
    _analytics_db_name = analytics_db_name;
  }

  //std::cout << "database_name: " << _dbname << std::endl;

  return true;
}

//-----------------------------------------------------------------------------
void system_c::close( void ) {

}

//-----------------------------------------------------------------------------
std::string system_c::analytics_uri( void ) {
  return _analytics_uri;
}

//-----------------------------------------------------------------------------
std::string system_c::samples_uri( void ) {
  return _samples_uri;
}

//-----------------------------------------------------------------------------
std::string system_c::database_uri( void ) {
  return _database_uri;
}

//-----------------------------------------------------------------------------
std::string system_c::packages_path( void ) {
  return _packages_path;
}

//-----------------------------------------------------------------------------
std::string system_c::sims_path( void ) {
  return _sims_path;
}

//-----------------------------------------------------------------------------
std::string system_c::sims_bin_search_path( void ) {
  return _sims_bin_search_path;
}

//-----------------------------------------------------------------------------
std::string system_c::sims_lib_search_path( void ) {
  return _sims_lib_search_path;
}

//-----------------------------------------------------------------------------
unsigned system_c::monitor_port( void ) {
  return _monitor_port;
}

//-----------------------------------------------------------------------------
unsigned system_c::analyzer_port( void ) {
  return _analyzer_port;
}

//-----------------------------------------------------------------------------
std::string system_c::analytics_host( void ) {
  std::string protocol, host;
  unsigned port;

  split_uri( _analytics_uri, protocol, host, port );
  return host;
}

//-----------------------------------------------------------------------------
unsigned system_c::analytics_port( void ) {
  std::string protocol, host;
  unsigned port;

  split_uri( _analytics_uri, protocol, host, port );
  return port;
}

//-----------------------------------------------------------------------------
std::string system_c::samples_host( void ) {
  std::string protocol, host;
  unsigned port;

  split_uri( _samples_uri, protocol, host, port );
  return host;
}

//-----------------------------------------------------------------------------
unsigned system_c::samples_port( void ) {
  std::string protocol, host;
  unsigned port;

  split_uri( _samples_uri, protocol, host, port );
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
std::string system_c::samples_database_name( void ) {
  return _samples_db_name;
}

//-----------------------------------------------------------------------------
std::string system_c::analytics_database_name( void ) {
  return _analytics_db_name;
}

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
