/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_STATE_H_
#define _REVEAL_STATE_H_

//-----------------------------------------------------------------------------

#include <vector>
#include <assert.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

class State {
public:
  State( void ) {}
  virtual ~State( void ) {}

  double q( const unsigned& i ) {
    assert( i < _q.size() );
    return _q[i];
  }
  double dq( const unsigned& i ) {
    assert( i < _dq.size() );
    return _dq[i];
  }

  void Append_q( const double& q ) {
    _q.push_back( q );
  }

  void Append_dq( const double& dq ) {
    _dq.push_back( dq );
  }

  unsigned Size( void ) const {
    return _q.size() + _dq.size();
  }

  unsigned Size_q( void ) const {
    return _q.size();
  }

  unsigned Size_dq( void ) const {
    return _dq.size();
  }

private:
  std::vector<double> _q;
  std::vector<double> _dq;
};

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_STATE_H_
