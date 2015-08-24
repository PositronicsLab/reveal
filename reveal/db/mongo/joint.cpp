#include "reveal/db/mongo/joint.h"

#include "reveal/core/model.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------

bool joint_c::insert( mongo::BSONObjBuilder& bson_parent, const std::vector<Reveal::Core::joint_ptr>& joints ) {
  mongo::BSONArrayBuilder bab;

  for( unsigned i = 0; i < joints.size(); i++ ) 
    insert( bab, joints[i] );
  bson_parent.appendArray( "joints", bab.arr() );

  return true;
}

//-----------------------------------------------------------------------------
bool joint_c::insert( mongo::BSONArrayBuilder& bson_array, Reveal::Core::joint_ptr joint ) {
  mongo::BSONObjBuilder bob;
  bob.append( "id", joint->id );

  mongo::BSONArrayBuilder bab;
  for( unsigned i = 0; i < joint->control.size_u(); i++ ) 
    bab.append( joint->control.u(i) ); 
  bob.appendArray( "control_u", bab.arr() );

  bson_array.append( bob.obj() );

  return true;
}

//-----------------------------------------------------------------------------
bool joint_c::fetch( Reveal::Core::model_ptr model, mongo::BSONObj bson_model ) {

  Reveal::Core::joint_ptr joint;
  mongo::BSONObj bson_joints;
  std::vector<mongo::BSONElement> v_joints;

  bson_joints = bson_model.getObjectField( "joints" );
  bson_joints.elems( v_joints );

  for( unsigned i = 0; i < v_joints.size(); i++ ) {
    joint = Reveal::Core::joint_ptr( new Reveal::Core::joint_c() );

    fetch( joint, v_joints[i].Obj() );

    model->joints.push_back( joint );
  }

  return true;
}
//-----------------------------------------------------------------------------
bool joint_c::fetch( Reveal::Core::joint_ptr joint, mongo::BSONObj bson ) {

  assert( joint );

  joint->id = bson.getField( "id" ).String();

  mongo::BSONObj bson_u = bson.getObjectField( "control_u" );
  std::vector<mongo::BSONElement> v_u;
  bson_u.elems( v_u );

  for( unsigned i = 0; i < v_u.size(); i++ )
    joint->control.u( i, v_u[i].Double() );

  return true;
}

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


