/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

analysis.h defines the analysis_c data-structure that contains data produced by
running analytics on a client's evaluation of a scenario
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_ANALYSIS_H_
#define _REVEAL_CORE_ANALYSIS_H_

//-----------------------------------------------------------------------------

#include <boost/shared_ptr.hpp>
#include <vector>
#include <reveal/core/pointers.h>
#include <reveal/core/experiment.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class analysis_c;
typedef boost::shared_ptr<Reveal::Core::analysis_c> analysis_ptr;
//-----------------------------------------------------------------------------

class analysis_c {
private:
  std::vector< std::vector<double> > _values;  //< The table of values produced by an analysis where each element of values is a vector, i.e. row, that is indexed in same order as keys.
  std::vector< std::string > _keys;  //< The list of search keys defining the fields of an analysis.

public:
  // TODO: this should be set by constructor or set and retrieved by get
  Reveal::Core::experiment_ptr experiment;

  /// Default constructor
  analysis_c( void ) { }
  /// Destructor
  virtual ~analysis_c( void ) { }

  /// Adds a set of values as a new row to an analysis.  Must have same size as
  /// the key definition
  /// @param row the set of values to add to the table of values
  void add_row( std::vector<double> row ) {
    assert( row.size() == _keys.size() );
    _values.push_back( row );
  }

  /// Sets the keys of the analysis
  /// @param keys the set of keys indexed in the same order as a row will be 
  ///        defined for the table of values.  keys can be considered to define
  ///        the column ids and the column order of the table
  void set_keys( std::vector<std::string> keys ) {
    // Note: this is not safe if there are values already in the table
    clear_keys();
    for( unsigned i = 0; i < keys.size(); i++ )
      add_key( keys[i] );
  }

  /// Returns the number of keys currently contained in the table
  /// @return the number of keys currently contained in the table
  unsigned count_keys( void ) {
    return _keys.size();
  }

  /// Clears the set of keys defined for the table
  void clear_keys( void ) {
    // Note: like set keys, this is not safe if there are values already in the 
    // table
    _keys.clear();
  }

  /// Adds a single key to the end of the list of keys, i.e. pushes.
  /// @param key the key value to add to the list of keys
  void add_key( std::string key ) {
    _keys.push_back( key );
  }

  /// Gets a key (by reference which may therefore be modified) by its position
  /// in the list of keys
  /// @param i the index of the key to retrieve (must be less than the key 
  ///        count)
  /// @return the reference to the key requested
  std::string& key( unsigned i ) {
    assert( i < _keys.size() );
    return _keys[i];
  }

  /// Gets a key (by value) by its position in the list of keys
  /// @param i the index of the key to retrieve (must be less than the key 
  ///        count)
  /// @return the key value requested
  std::string key( unsigned i ) const {
    assert( i < _keys.size() );
    return _keys[i];
  }

  /// Returns the number of sets of values, i.e. rows, currently contained in 
  /// the table
  /// @return the number of rows currently contained in the table
  unsigned count_rows( void ) {
    return _values.size();
  }

  /// Gets a value (by reference which may therefore be modified) by its row 
  /// number and its positional (key) index 
  /// @param row the index of the row to retrieve (must be less than count_rows)
  /// @param key_idx the index of the column to retrieve (must be less than 
  ///        count_keys)
  /// @return the reference to the value requested
  double& value( unsigned row, unsigned key_idx ) {
    assert( row < _values.size() );
    assert( key_idx < _keys.size() );
    assert( key_idx < _values[row].size() );

    return _values[row][key_idx];
  }

  /// Gets a value (by value) by its row number and its positional (key) index 
  /// @param row the index of the row to retrieve (must be less than count_rows)
  /// @param key_idx the index of the column to retrieve (must be less than 
  ///        count_keys)
  /// @return the reference to the value requested
  double value( unsigned row, unsigned key_idx ) const {
    assert( row < _values.size() );
    assert( key_idx < _keys.size() );
    assert( key_idx < _values[row].size() );

    return _values[row][key_idx];
  }
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_ANALYSIS_H_
