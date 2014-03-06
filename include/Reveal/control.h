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

class Control {
public:
  Control( void ) {}
  virtual ~Control( void ) {}

  double u( const unsigned& i ) {
    assert( i < _u.size() );
    return _u[i];
  }

  void Append( const double& u ) {
    _u.push_back( u );
  }

  unsigned Size( void ) {
    return _u.size();
  }

private:
  std::vector<double> _u;
};

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CONTROL_H_
