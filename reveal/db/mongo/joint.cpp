#include "reveal/db/mongo/joint.h"

#include "reveal/core/model.h"

//-----------------------------------------------------------------------------
#define DOCUMENT "joints"
#define FIELD_ID "id"
#define FIELD_STATE_POSITION "q"
#define FIELD_STATE_VELOCITY "dq"
#define FIELD_CONTROL "u"

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
  bson_parent.appendArray( DOCUMENT, bab.arr() );

  return true;
}

//-----------------------------------------------------------------------------
bool joint_c::insert( mongo::BSONArrayBuilder& bson_array, Reveal::Core::joint_ptr joint ) {
  mongo::BSONObjBuilder bob;
  bob.append( FIELD_ID, joint->id );

  mongo::BSONArrayBuilder bab_q;
  for( unsigned i = 0; i < joint->state.size_q(); i++ )
    bab_q.append( joint->state.q(i) );
  bob.appendArray( FIELD_STATE_POSITION, bab_q.arr() );
  
  mongo::BSONArrayBuilder bab_dq;
  for( unsigned i = 0; i < joint->state.size_dq(); i++ ) 
    bab_dq.append( joint->state.dq(i) );
  bob.appendArray( FIELD_STATE_VELOCITY, bab_dq.arr() );

  mongo::BSONArrayBuilder bab_u;
  for( unsigned i = 0; i < joint->control.size_u(); i++ ) 
    bab_u.append( joint->control.u(i) ); 
  bob.appendArray( FIELD_CONTROL, bab_u.arr() );

  bson_array.append( bob.obj() );

  return true;
}

//-----------------------------------------------------------------------------
bool joint_c::fetch( Reveal::Core::model_ptr model, mongo::BSONObj bson_model ) {

  Reveal::Core::joint_ptr joint;
  mongo::BSONObj bson_joints;
  std::vector<mongo::BSONElement> v_joints;

  bson_joints = bson_model.getObjectField( DOCUMENT );
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

  joint->id = bson.getField( FIELD_ID ).String();

  mongo::BSONObj bson_q = bson.getObjectField( FIELD_STATE_POSITION );
  std::vector<mongo::BSONElement> v_q;
  bson_q.elems( v_q );

  for( unsigned i = 0; i < v_q.size(); i++ )
    joint->state.q( i, v_q[i].Double() );

  mongo::BSONObj bson_dq = bson.getObjectField( FIELD_STATE_VELOCITY );

  std::vector<mongo::BSONElement> v_dq;
  bson_dq.elems( v_dq );

  for( unsigned i = 0; i < v_dq.size(); i++ )
    joint->state.dq( i, v_dq[i].Double() );

  mongo::BSONObj bson_u = bson.getObjectField( FIELD_CONTROL );
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


