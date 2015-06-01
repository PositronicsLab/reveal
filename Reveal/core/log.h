/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

-----------------------------------------------------------------------------*/
#ifndef _REVEAL_CORE_LOG_H_
#define _REVEAL_CORE_LOG_H_

#include <iostream>
#include <fstream>
#include <string>
#include <boost/shared_ptr.hpp>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class log_c;
typedef boost::shared_ptr< log_c > log_ptr;

//-----------------------------------------------------------------------------

class log_c 
{
private:
  std::string _filename;      //< the path to the log file
  std::ofstream _file;        //< the writeonly output stream

public:
  /// Parameterized constructor that requires the path to the output file
  /// @param filename the path to the output file
  log_c( std::string filename ) { 
    _filename = filename;
  }

  /// Destructor
  virtual ~log_c( void ) { 
    if( _file.is_open() ) close();
  }

  /// Opens the file for writing.  If the file already exists, it is overwritten
  /// @return true if the file is successfully open for writing OR false if the
  ///         operation failed for any reason
  bool open( void ) {  
    _file.open( _filename.c_str(), std::ios::out | std::ios::trunc );
    return _file.is_open();
  }
  
  /// Closes the output file
  void close( void ) {
    _file.close();
  }

  /// Immediately writes a message to the output file
  /// @param packet the message to write to the output file
  void write( std::string packet ) {
    _file << packet;
  }

};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_LOG_H_
