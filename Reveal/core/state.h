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
#include "Reveal/core/serial.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class state_c : public serial_c {
public:
  /// Default constructor
  state_c( void ) {
    for( unsigned i = 0; i < size(); i++ ) _x[i] = 0.0;
    _x[3] = 1.0;  // normalize w coordinate
  }
  /// Destructor
  virtual ~state_c( void ) {}

private:
  double _x[13];             //< the thirteen vector state data

public:
  /// Gets a positional component of state.
  /// @param i the index of the component to get.  Must be less than seven.
  /// @return a positional dimension
  double q( unsigned i ) {
    assert( i < size_q() );
    return _x[i];
  }

  /// Gets a first order derivative component of state.
  /// @param i the index of the component to get.  Must be less than six.
  /// @return a first order derivative dimension
  double dq( unsigned i ) {
    assert( i < size_dq() );
    return _x[i+size_q()];
  }

  /// Sets a positional component of state.
  /// @param i the index of the component to set
  /// @param q the value to set the component to
  void q( unsigned i, const double& q ) {
    assert( i < size_q() );
    _x[i] = q;
  }

  /// Sets a first order derivative component of state.
  /// @param i the index of the component to set
  /// @param dq the value to set the component to
  void dq( unsigned i, const double& dq ) {
    assert( i < size_dq() );
    _x[i+size_q()] = dq;
  }

  /// Gets the total size of the state vector
  /// @return the total size of the state vector
  unsigned size( void ) const {
    return size_q() + size_dq();
  }

  /// Gets the size of the positional components of the state vector
  /// @return the size of the positional components of the state vector
  unsigned size_q( void ) const {
    return 7;
  }

  /// Gets the size of the first order derivative components of the state vector
  /// @return the size of the first order derivative components of the state 
  ///         vector
  unsigned size_dq( void ) const {
    return 6;
  }

  /// Gets the linear x position value
  /// @return the linear x position value
  double linear_x( void ) const { return _x[0]; }
  /// Gets the linear y position value
  /// @return the linear y position value
  double linear_y( void ) const { return _x[1]; }
  /// Gets the linear z position value
  /// @return the linear z position value
  double linear_z( void ) const { return _x[2]; }
  /// Gets the unit quaternion's x rotation value
  /// @return the unit quaternion's x rotation value
  double angular_x( void ) const { return _x[3]; }
  /// Gets the unit quaternion's y rotation value
  /// @return the unit quaternion's y rotation value
  double angular_y( void ) const { return _x[4]; }
  /// Gets the unit quaternion's z rotation value
  /// @return the unit quaternion's z rotation value
  double angular_z( void ) const { return _x[5]; }
  /// Gets the unit quaternion's w value
  /// @return the unit quaternion's w value
  double angular_w( void ) const { return _x[6]; }
  /// Gets the linear x first derivative value
  /// @return the linear x first derivative value
  double linear_dx( void ) const { return _x[7]; }
  /// Gets the linear y first derivative value
  /// @return the linear y first derivative value
  double linear_dy( void ) const { return _x[8]; }
  /// Gets the linear z first derivative value
  /// @return the linear z first derivative value
  double linear_dz( void ) const { return _x[9]; }
  /// Gets the angular x first derivative value
  /// @return the angular x first derivative value
  double angular_dx( void ) const { return _x[10]; }
  /// Gets the angular y first derivative value
  /// @return the angular y first derivative value
  double angular_dy( void ) const { return _x[11]; }
  /// Gets the angular z first derivative value
  /// @return the angular z first derivative value
  double angular_dz( void ) const { return _x[12]; }

  /// Sets the linear x position value
  /// @param x the value to use as the state's linear x position
  void linear_x( const double& x ) { _x[0] = x; }
  /// Sets the linear y position value
  /// @param y the value to use as the state's linear y position
  void linear_y( const double& y ) { _x[1] = y; }
  /// Sets the linear z position value
  /// @param z the value to use as the state's linear z position
  void linear_z( const double& z ) { _x[2] = z; }
  /// Sets the angular x rotation value
  /// @param x the value to use as the state's unit quaternion x rotation
  void angular_x( const double& x ) { _x[3] = x; }
  /// Sets the angular y rotation value
  /// @param y the value to use as the state's unit quaternion y rotation
  void angular_y( const double& y ) { _x[4] = y; }
  /// Sets the angular z rotation value
  /// @param z the value to use as the state's unit quaternion z rotation
  void angular_z( const double& z ) { _x[5] = z; }
  /// Sets the angular w rotation value
  /// @param w the value to use as the state's unit quaternion w value
  void angular_w( const double& w ) { _x[6] = w; }
  /// Sets the linear x first derivative value
  /// @param x the value to use as the state's linear x first derivative
  void linear_dx( const double& dx ) { _x[7] = dx; }
  /// Sets the linear y first derivative value
  /// @param y the value to use as the state's linear y first derivative
  void linear_dy( const double& dy ) { _x[8] = dy; }
  /// Sets the linear z first derivative value
  /// @param z the value to use as the state's linear z first derivative
  void linear_dz( const double& dz ) { _x[9] = dz; }
  /// Sets the angular x first derivative value
  /// @param x the value to use as the state's angular x first derivative
  void angular_dx( const double& dx ) { _x[10] = dx; }
  /// Sets the angular y first derivative value
  /// @param y the value to use as the state's angular y first derivative
  void angular_dy( const double& dy ) { _x[11] = dy; }
  /// Sets the angular z first derivative value
  /// @param z the value to use as the state's angular z first derivative
  void angular_dz( const double& dz ) { _x[12] = dz; }

  /// Gets a reference to one of the state components.
  /// @param i the index of the component to get.  Must be less than thirteen.
  /// @return one of the states components
  double& operator[]( unsigned i ) { 
    assert( i < size() );
    return _x[i];
  }

  /// Gets a value to one of the state components.
  /// @param i the index of the component to get.  Must be less than thirteen.
  /// @return one of the states components
  const double& operator[]( unsigned i ) const { 
    assert( i < size() );
    return _x[i];
  }

  /// Prints the contents of the vector to the console
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

  // serial_c interface
  virtual std::stringstream& serialize( std::stringstream& stream, char delimiter ) {
    for( unsigned i = 0; i < size(); i++ ) {
      if( i > 0 ) stream << delimiter;
      stream << _x[i];
    }
    return stream;
  }

  // TODO : Error detection
  virtual bool deserialize( std::stringstream& stream ) {
    for( unsigned i = 0; i < size(); i++ ) {
      stream >> _x[i];
    }
    return true;
  }
};

//-----------------------------------------------------------------------------
}  // namespace Core
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_STATE_H_
