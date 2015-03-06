#include "Reveal/client/system.h"

#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>

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
