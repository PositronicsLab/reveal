/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

control.h defines the control_c data-structure that contains variable length 
control data
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_CONTROL_H_
#define _REVEAL_CORE_CONTROL_H_

//-----------------------------------------------------------------------------

#include <vector>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Core {

//-----------------------------------------------------------------------------

class control_c {
public:
  control_c( void ) {}
  virtual ~control_c( void ) {}

  double u( const unsigned& i ) {
    assert( i < _u.size() );
    return _u[i];
  }

  void append( const double& u ) {
    _u.push_back( u );
  }

  void append_u( const double& u ) {
    _u.push_back( u );
  }

  unsigned size_u( void ) {
    return _u.size();
  }

  unsigned size( void ) {
    return _u.size();
  }

private:
  std::vector<double> _u;
};

//-----------------------------------------------------------------------------

}  // namespace Core

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_CONTROL_H_
