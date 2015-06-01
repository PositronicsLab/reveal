/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

control.h defines the control_c data-structure that contains variable length 
control data
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_CONTROL_H_
#define _REVEAL_CORE_CONTROL_H_

//-----------------------------------------------------------------------------

#include <stdio.h>

#include <vector>
#include "Reveal/core/serial.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class control_c : public serial_c {
public:
  /// Default constructor
  control_c( void ) {
    for( unsigned i = 0; i < size_u(); i++ ) _u[i] = 0.0;
  }
  /// Destructor
  virtual ~control_c( void ) {}

private:
  double _u[6];      //< the six vector control data

public:
  /// Gets the value of the control for the respective dimension's index.
  /// @param i the index of the control value to get.  Must be less than six.
  /// @return the value of the control in the specified dimension
  double u( unsigned i ) {
    assert( i < size() );
    return _u[i];
  }

  /// Sets the value of the control for the respective dimension's index.
  /// @param i the index of the dimension to set
  /// @param u the control values to set
  void u( unsigned i, const double& u ) {
    assert( i < size() );
    _u[i] = u;
  }

  /// Returns the size of the control data
  /// @return the size of the control data
  unsigned size_u( void ) const {
    return 6;
  }

  /// Returns the size of the control data
  /// @return the size of the control data
  unsigned size( void ) const {
    return 6;
  }

  // TODO: These need to be deprecated as there is no real reliable association
  double linear_x( void ) { return _u[0]; }
  double linear_y( void ) { return _u[1]; }
  double linear_z( void ) { return _u[2]; }
  double angular_x( void ) { return _u[3]; }
  double angular_y( void ) { return _u[4]; }
  double angular_z( void ) { return _u[5]; }

  // TODO: These need to be deprecated as there is no real reliable association
  void linear_x( const double& x ) { _u[0] = x; }
  void linear_y( const double& y ) { _u[1] = y; }
  void linear_z( const double& z ) { _u[2] = z; }
  void angular_x( const double& x ) { _u[3] = x; }
  void angular_y( const double& y ) { _u[4] = y; }
  void angular_z( const double& z ) { _u[5] = z; }

  /// Gets a reference to the control value at the specified index
  /// @param i the index to return the reference to.  Must be less than size
  /// @return the reference to the control value
  double& operator[]( unsigned i ) { 
    assert( i < size() );
    return _u[i];
  }

  /// Gets the control value (by value) at the specified index
  /// @param i the index to return the value of.  Must be less than size
  /// @return the value of the control value
  const double& operator[]( unsigned i ) const { 
    assert( i < size() );
    return _u[i];
  }

  /// Prints the contents of the vector to the console
  void print( void ) {
    printf( "u{" );
    for( unsigned i = 0; i < size(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%f", _u[i] );
    }
    printf( "}" );
  }

  // -Implementation of the serial_c interface -

  /// Serializes the control data into a stream
  /// @param stream the stream to into
  /// @param delimiter the delimiter to use between values
  /// @return the stream the data was serialized into
  virtual std::stringstream& serialize( std::stringstream& stream, char delimiter ) {
    for( unsigned i = 0; i < size(); i++ ) {
      if( i > 0 ) stream << delimiter;
      stream << _u[i];
    }
    return stream;
  }

  /// Deserializes control data from a stream
  /// @param stream the stream to deserialize from
  /// @return true if the operation was successful OR false if it failed for any
  ///         reason
  virtual bool deserialize( std::stringstream& stream ) {
    // TODO : Error detection
    for( unsigned i = 0; i < size(); i++ ) {
      stream >> _u[i];
    }
    return true;
  }
};

//-----------------------------------------------------------------------------
}  // namespace Core
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_CONTROL_H_
