/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

analysis.h defines the analysis_c data-structure that contains data produced by
running analytics on a client's evaluation of a scenario
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_ANALYSIS_H_
#define _REVEAL_CORE_ANALYSIS_H_

//-----------------------------------------------------------------------------

//#include <Reveal/solution.h>
//#include <Reveal/trial.h>
//#include <Reveal/scenario.h>

#include <boost/shared_ptr.hpp>
#include <vector>
#include <Reveal/core/pointers.h>
#include <Reveal/core/experiment.h>

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
  std::vector< std::vector<double> > _values;
  std::vector<std::string> _keys;

public:
  Reveal::Core::experiment_ptr experiment;

  analysis_c( void ) { }
  virtual ~analysis_c( void ) { }

  void add_row( std::vector<double> row ) {
    assert( row.size() == _keys.size() );
    _values.push_back( row );
  }

  void set_keys( std::vector<std::string> keys ) {
    clear_keys();
    for( unsigned i = 0; i < keys.size(); i++ )
      add_key( keys[i] );
  }

  unsigned count_keys( void ) {
    return _keys.size();
  }

  void clear_keys( void ) {
    _keys.clear();
  }

  void add_key( std::string key ) {
    _keys.push_back( key );
  }

  std::string& key( unsigned i ) {
    assert( i < _keys.size() );
    return _keys[i];
  }

  std::string key( unsigned i ) const {
    assert( i < _keys.size() );
    return _keys[i];
  }

  unsigned count_rows( void ) {
    return _values.size();
  }


  double& value( unsigned row, unsigned key_idx ) {
    assert( row < _values.size() );
    assert( key_idx < _keys.size() );
    assert( key_idx < _values[row].size() );

    return _values[row][key_idx];
  }

  double value( unsigned row, unsigned key_idx ) const {
    assert( row < _values.size() );
    assert( key_idx < _keys.size() );
    assert( key_idx < _values[row].size() );

    return _values[row][key_idx];
  }

  // TODO : add session data
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_ANALYSIS_H_
