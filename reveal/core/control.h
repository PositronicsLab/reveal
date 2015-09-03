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
#include "reveal/core/serial.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class control_c : public serial_c {
private:
  static const unsigned _max_size = 6;  //< absolute max size for control data

  double _u[_max_size];      //< array of max allocation to hold data
  unsigned _current_size;    // number of elements actually in array
public:
  /// Default constructor
  control_c( void ) {
    for( unsigned i = 0; i < _max_size; i++ ) _u[i] = 0.0;

    _current_size = 1; // assume the joint has one control for ease of use
                       // NOTE: must resize(...) for joint with other than 1 DOF
  }
  /// Destructor
  virtual ~control_c( void ) {}

  void resize( unsigned size ) {
    assert( size <= _max_size );
    _current_size = size;
  }

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
    return _current_size;
  }

  /// Returns the size of the control data
  /// @return the size of the control data
  unsigned size( void ) const {
    return _current_size;
  }

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
