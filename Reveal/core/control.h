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
  control_c( void ) {
    for( unsigned i = 0; i < size_u(); i++ ) _u[i] = 0.0;
  }
  virtual ~control_c( void ) {}

private:
  double _u[6];

public:
  double u( unsigned i ) {
    assert( i < size() );
    return _u[i];
  }

  void u( unsigned i, const double& u ) {
    assert( i < size() );
    _u[i] = u;
  }

  unsigned size_u( void ) const {
    return 6;
  }

  unsigned size( void ) const {
    return 6;
  }

  double linear_x( void ) { return _u[0]; }
  double linear_y( void ) { return _u[1]; }
  double linear_z( void ) { return _u[2]; }
  double angular_x( void ) { return _u[3]; }
  double angular_y( void ) { return _u[4]; }
  double angular_z( void ) { return _u[5]; }

  void linear_x( const double& x ) { _u[0] = x; }
  void linear_y( const double& y ) { _u[1] = y; }
  void linear_z( const double& z ) { _u[2] = z; }
  void angular_x( const double& x ) { _u[3] = x; }
  void angular_y( const double& y ) { _u[4] = y; }
  void angular_z( const double& z ) { _u[5] = z; }

  double& operator[]( unsigned i ) { 
    assert( i < size() );
    return _u[i];
  }

  const double& operator[]( unsigned i ) const { 
    assert( i < size() );
    return _u[i];
  }

  void print( void ) {
    printf( "u{" );
    for( unsigned i = 0; i < size(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%f", _u[i] );
    }
    printf( "}" );
  }

};

//-----------------------------------------------------------------------------

}  // namespace Core

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_CONTROL_H_
