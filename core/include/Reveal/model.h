/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_MODEL_H_
#define _REVEAL_CORE_MODEL_H_

//-----------------------------------------------------------------------------

#include <vector>
#include <string>
#include <assert.h>

#include <Reveal/pointers.h>
#include <Reveal/link.h>
#include <Reveal/joint.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Core {

//-----------------------------------------------------------------------------

class model_c {
public:
  std::string id;
  std::vector<link_ptr> links;
  std::vector<joint_ptr> joints;

  model_c( void ) {}
  virtual ~model_c( void ) {}

  void print( void ) {
    printf( "model[%s] { ", id.c_str() );
    for( unsigned i = 0; i < links.size(); i++ ) {
      links[i]->print();
    }
    for( unsigned i = 0; i < joints.size(); i++ ) {
      joints[i]->print();
    }
    printf( " }" );
  }

};

//-----------------------------------------------------------------------------

}  // namespace Core

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_MODEL_H_
