/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CONTROL_H_
#define _REVEAL_CONTROL_H_

//-----------------------------------------------------------------------------

#include <vector>

//-----------------------------------------------------------------------------

namespace Reveal {

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

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CONTROL_H_
