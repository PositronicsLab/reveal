/*-----------------------------------------------------------------------------
this test program assumes it is being run automatically by cmake/ctest.
pathing and parameter assumptions may not hold up if run manually
-----------------------------------------------------------------------------*/

#include "reveal/core/system.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>

bool create_test_file( std::string directory, std::string file ) {
  std::stringstream ss;

  std::string path = combine_path( directory, file );
  std::ofstream f( path.c_str() );

  ss << "0123456789" << std::endl;
  std::string s = ss.str();

  f.write( s.c_str(), s.size() );
  f.close();
  return true;
}

//-----------------------------------------------------------------------------
int main( int argc, char* argv[] ) {

  std::string path;
  std::string temp_path, root_path;

  // names of files and directories
  std::string test_file = "test.txt";
  std::string subdir1_name = "subdir1";
  std::string subdir2_name = "subdir2";
  std::string subsubdir1_name = "subsubdir1";
  std::string subdir_test_file = "subtest.txt";


  // get the current working path from the system 
  std::string working_path = get_current_path();

  // attempt to create a temporary directory
  if( !get_temp_directory( temp_path ) ) return 5;

  // temp path just reported to be successfully created.  test existence method
  if( !path_exists( temp_path ) ) return 1;

  // temp path is a path to a directory, so test is_directory
  if( !is_directory( temp_path ) ) return 2;

  // create a file in the temp path
  if( !create_test_file( temp_path, test_file ) ) return 8;

  // already know temp path is not a file, so test is_file for negative case
  if( is_file( temp_path ) ) return 3;

  // test whether the is file method reports the test file correctly
  path = combine_path( temp_path, test_file );
  if( !is_file( path ) ) return 3;

  // attempt to remove the temporary directory
  if( !remove_directory( temp_path ) ) return 6;

  //--
  
  // get another temporary directory this time call it the root path
  if( !get_temp_directory( root_path ) ) return 7;

  // create a file in the root path
  if( !create_test_file( root_path, test_file ) ) return 8;

  // create a directory in the root path
  std::string subdir1_path = combine_path( root_path, subdir1_name );
  if( !get_directory( subdir1_path ) ) return 9;

  // create another directory in the root path
  std::string subdir2_path = combine_path( root_path, subdir2_name );
  if( !get_directory( subdir2_path ) ) return 9;

  // create a directory in the first child directory of the root path
  std::string subsubdir1_path = combine_path( subdir1_path, subsubdir1_name );
  if( !get_directory( subsubdir1_path ) ) return 9;

  // create a file in the child of the child directory
  if( !create_test_file( subsubdir1_path, subdir_test_file ) ) return 8;
/*
  // get the subdirectories of the root
  std::vector<std::string> dirs = get_subdirectories( root_path );
  for( std::vector<std::string>::iterator it = dirs.begin(); it != dirs.end(); it++ ) {
    printf( "dir: %s\n", it->c_str() );
  }
*/
  // test find file without recursion.  search for the test file in the root
  if( !find_file( path, test_file, root_path ) ) return 1;

  // validate the path returned from find file
  temp_path = combine_path( root_path, test_file );
  if( temp_path != path ) return 1;

  // test find file with recursion.  search for the subtest file in the dirs
  if( !find_file( path, subdir_test_file, root_path, true ) ) return 1;
  temp_path = combine_path( subsubdir1_path, subdir_test_file );
  if( temp_path != path ) return 1; 

  // clean up the root path
  if( !remove_directory( root_path ) ) return 6;

  if( find_file( path, "gzserver", "/usr/local", true ) ) {
    printf( "path: %s\n", path.c_str() );
  }

  return 0;
}
