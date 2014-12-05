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
  std::string _filename; 
  std::ofstream _file;

public:
  log_c( std::string filename ) { 
    _filename = filename;
  }

  virtual ~log_c( void ) { 
    if( _file.is_open() ) close();
  }

  bool open( void ) {  
    _file.open( _filename.c_str(), std::ios::out | std::ios::trunc );
    return _file.is_open();
  }
  
  void close( void ) {
    _file.close();
  }

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
