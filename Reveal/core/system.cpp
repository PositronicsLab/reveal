#include "Reveal/core/system.h"

#include <stdlib.h>
#include <iostream>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>

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

/*
//-----------------------------------------------------------------------------
bool copy_file( std::string src, std::string dest ) {

  char buf[BUFSIZ];
  size_t size;

  int fd_src = open( src.c_str(), O_RDONLY, 0 );
  int fd_dest = open( dest.c_str(), O_WRONLY | O_CREAT, 0644 );
//  int fd_dest = open( dest.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644 );

  while( ( size = read( fd_src, buf, BUFSIZ ) ) > 0 )
    write( fd_dest, buf, size );

  close( fd_src );
  close( fd_dest );

  return true;
}
*/

//-----------------------------------------------------------------------------
std::string make_temp_dir( void ) {
  char tmp_path_template[] = "/tmp/reveal-XXXXXX";
  std::string tmp_path = mkdtemp( tmp_path_template );
  //printf( "tmp_path: %s\n", tmp_path.c_str() );

  return tmp_path;
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
// TODO:  for platform independence, replace stat with boost filesystem
bool file_exists( std::string path ) {
  struct stat st;
  stat( path.c_str(), &st );
  if( S_ISREG( st.st_mode ) )
    return true;
  return false;
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
  }

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
bool system_c::get_directory( std::string path ) {
  DIR *dir = opendir( path.c_str() );
  if( !dir ) {
    if( mkdir( path.c_str(), S_IRWXU | S_IRGRP | S_IROTH ) == -1 )
      return false;
  } else {
    closedir( dir );
  }
  return true;
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
