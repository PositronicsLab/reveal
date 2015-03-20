/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

state.h defines the state_c data-structure that contains the variable lenght 
state component 0f a scenario  
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_STATE_H_
#define _REVEAL_CORE_STATE_H_

//-----------------------------------------------------------------------------

#include <assert.h>
#include <stdio.h>
#include <vector>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Core {

//-----------------------------------------------------------------------------

class state_c {
public:
  state_c( void ) {
    for( unsigned i = 0; i < size(); i++ ) _x[i] = 0.0;
    _x[3] = 1.0;  // normalize w coordinate
  }
  virtual ~state_c( void ) {}

private:
  double _x[13];

public:
  double q( unsigned i ) {
    assert( i < size_q() );
    return _x[i];
  }

  double dq( unsigned i ) {
    assert( i < size_dq() );
    return _x[i+size_q()];
  }

  void q( unsigned i, const double& q ) {
    assert( i < size_q() );
    _x[i] = q;
  }

  void dq( unsigned i, const double& dq ) {
    assert( i < size_dq() );
    _x[i+size_q()] = dq;
  }

  unsigned size( void ) const {
    return size_q() + size_dq();
  }

  unsigned size_q( void ) const {
    return 7;
  }

  unsigned size_dq( void ) const {
    return 6;
  }

  double linear_x( void ) const { return _x[0]; }
  double linear_y( void ) const { return _x[1]; }
  double linear_z( void ) const { return _x[2]; }
  double angular_x( void ) const { return _x[3]; }
  double angular_y( void ) const { return _x[4]; }
  double angular_z( void ) const { return _x[5]; }
  double angular_w( void ) const { return _x[6]; }
  double linear_dx( void ) const { return _x[7]; }
  double linear_dy( void ) const { return _x[8]; }
  double linear_dz( void ) const { return _x[9]; }
  double angular_dx( void ) const { return _x[10]; }
  double angular_dy( void ) const { return _x[11]; }
  double angular_dz( void ) const { return _x[12]; }

  void linear_x( const double& x ) { _x[0] = x; }
  void linear_y( const double& y ) { _x[1] = y; }
  void linear_z( const double& z ) { _x[2] = z; }
  void angular_x( const double& x ) { _x[3] = x; }
  void angular_y( const double& y ) { _x[4] = y; }
  void angular_z( const double& z ) { _x[5] = z; }
  void angular_w( const double& w ) { _x[6] = w; }
  void linear_dx( const double& dx ) { _x[7] = dx; }
  void linear_dy( const double& dy ) { _x[8] = dy; }
  void linear_dz( const double& dz ) { _x[9] = dz; }
  void angular_dx( const double& dx ) { _x[10] = dx; }
  void angular_dy( const double& dy ) { _x[11] = dy; }
  void angular_dz( const double& dz ) { _x[12] = dz; }

  double& operator[]( unsigned i ) { 
    assert( i < size() );
    return _x[i];
  }

  const double& operator[]( unsigned i ) const { 
    assert( i < size() );
    return _x[i];
  }

  void print( void ) {
    printf( "q{" );
    for( unsigned i = 0; i < size_q(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%f", _x[i] );
    }
    printf( "}" );

    printf( "dq{" );
    for( unsigned i = 0; i < size_dq(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%f", _x[i+size_q()] );
    }
    printf( "}" );
  }

};

//-----------------------------------------------------------------------------

}  // namespace Core

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_STATE_H_
