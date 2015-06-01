/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_MODEL_H_
#define _REVEAL_CORE_MODEL_H_

//-----------------------------------------------------------------------------

#include <vector>
#include <string>
#include <assert.h>

#include "Reveal/core/pointers.h"
#include "Reveal/core/link.h"
#include "Reveal/core/joint.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class model_c {
public:
  std::string id;                 //< the unique model identifier
  std::vector<link_ptr> links;    //< the set of links in the model
  std::vector<joint_ptr> joints;  //< the set of joints in the model

  /// Default constructor
  model_c( void ) {}
  /// Destructor
  virtual ~model_c( void ) {}

  /// Appends a link to this model
  /// @param link the link to append to this model
  void insert( link_ptr link ) {
    links.push_back( link );
  }

  /// Appends a joint to this model
  /// @param joint the joint to append to this model
  void insert( joint_ptr joint ) {
    joints.push_back( joint );
  }

  /// Prints the instance data to the console
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
