/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

state.h defines the state_c data-structure that contains the variable lenght 
state component 0f a scenario  
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_STATE_H_
#define _REVEAL_CORE_STATE_H_

//-----------------------------------------------------------------------------

#include <vector>
#include <assert.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Core {

//-----------------------------------------------------------------------------

class state_c {
public:
  state_c( void ) {}
  virtual ~state_c( void ) {}

  double q( const unsigned& i ) {
    assert( i < _q.size() );
    return _q[i];
  }

  double dq( const unsigned& i ) {
    assert( i < _dq.size() );
    return _dq[i];
  }

  void append_q( const double& q ) {
    _q.push_back( q );
  }

  void append_dq( const double& dq ) {
    _dq.push_back( dq );
  }

  unsigned size( void ) const {
    return _q.size() + _dq.size();
  }

  unsigned size_q( void ) const {
    return _q.size();
  }

  unsigned size_dq( void ) const {
    return _dq.size();
  }

private:
  std::vector<double> _q;
  std::vector<double> _dq;
};

//-----------------------------------------------------------------------------

}  // namespace Core

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_STATE_H_
