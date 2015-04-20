#include "Reveal/db/mongo/trial.h"

#include <mongo/client/dbclient.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
unsigned trial_c::count( Reveal::DB::database_ptr db ) {
  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return 0;

  // query mongo for count
  return mongo->count( "trial" );
}
//-----------------------------------------------------------------------------
bool trial_c::insert( Reveal::DB::database_ptr db, Reveal::Core::trial_ptr trial ) {

  mongo::BSONObjBuilder bob;
  bob.append( "scenario_id", trial->scenario_id );
  bob.append( "trial_id", trial->trial_id );
  bob.append( "t", trial->t );
  bob.append( "dt", trial->dt );

  mongo::BSONArrayBuilder bab_models;
  for( unsigned i = 0; i < trial->models.size(); i++ ) {
    Reveal::Core::model_ptr model = trial->models[i];
    mongo::BSONObjBuilder bob_model;
    bob_model.append( "id", model->id );

    mongo::BSONArrayBuilder bab_links;
    for( unsigned j = 0; j < model->links.size(); j++ ) {
      Reveal::Core::link_ptr link = model->links[j];
      mongo::BSONObjBuilder bob_link;
      bob_link.append( "id", link->id );

      mongo::BSONArrayBuilder bab_state_q;
      for( unsigned k = 0; k < link->state.size_q(); k++ ) {
        bab_state_q.append( link->state.q(k) );
      }
      bob_link.appendArray( "state_q", bab_state_q.arr() );
      
      mongo::BSONArrayBuilder bab_state_dq;
      for( unsigned k = 0; k < link->state.size_dq(); k++ ) {
        bab_state_dq.append( link->state.dq(k) );
      }
      bob_link.appendArray( "state_dq", bab_state_dq.arr() );

      bab_links.append( bob_link.obj() );
    }
    bob_model.appendArray( "links", bab_links.arr() );

    mongo::BSONArrayBuilder bab_joints;
    for( unsigned j = 0; j < model->joints.size(); j++ ) {
      Reveal::Core::joint_ptr joint = model->joints[j];
      mongo::BSONObjBuilder bob_joint;
      bob_joint.append( "id", joint->id );

      mongo::BSONArrayBuilder bab_control_u;
      for( unsigned k = 0; k < joint->control.size_u(); k++ ) {
        bab_control_u.append( joint->control.u(k) );
      }
      bob_joint.appendArray( "control_u", bab_control_u.arr() );

      bab_joints.append( bob_joint.obj() );
    }
    bob_model.appendArray( "joints", bab_joints.arr() );

    bab_models.append( bob_model.obj() );
  }
  bob.appendArray( "models", bab_models.arr() );

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  return mongo->insert( "trial", bob.obj() );
}

//-----------------------------------------------------------------------------
bool trial_c::fetch( Reveal::Core::trial_ptr& trial, Reveal::DB::database_ptr db, std::string scenario_id, unsigned trial_id ) {
  std::auto_ptr<mongo::DBClientCursor> cursor;
//  Reveal::DB::query_c query;
  Reveal::Core::scenario_ptr ptr;

//  query.trial( scenario_id, trial_id );
//  fetch( cursor, "trial", query() );

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  mongo->fetch( cursor, "trial", QUERY( "scenario_id" << scenario_id << "trial_id" << trial_id ) );

  if( !cursor->more() ) return false;

  // add error handling
  mongo::BSONObj record = cursor->next();

  trial = Reveal::Core::trial_ptr( new Reveal::Core::trial_c() );
  trial->scenario_id = record.getField( "scenario_id" ).String();
  trial->trial_id = record.getField( "trial_id" ).Int();
  trial->t = record.getField( "t" ).Double();
  trial->dt = record.getField( "dt" ).Double();

  // TODO : replace the below with parsing the models

  mongo::BSONObj bson_models = record.getObjectField( "models" );
  std::vector<mongo::BSONElement> vec_models;
  bson_models.elems( vec_models );
  for( unsigned i = 0; i < vec_models.size(); i++ ) {
    Reveal::Core::model_ptr model = Reveal::Core::model_ptr( new Reveal::Core::model_c() );
    //mongo::BSONObj bson_model = vec_models[i].Obj().getObjectField( "model" );
    mongo::BSONObj bson_model = vec_models[i].Obj();
    model->id = bson_model.getField( "id" ).String();

    mongo::BSONObj bson_links = bson_model.getObjectField( "links" );
    std::vector<mongo::BSONElement> vec_links;
    bson_links.elems( vec_links );
    for( unsigned j = 0; j < vec_links.size(); j++ ) {
      Reveal::Core::link_ptr link = Reveal::Core::link_ptr( new Reveal::Core::link_c() );
      link->id = vec_links[j].Obj().getField( "id" ).String();

      mongo::BSONObj bson_state_q = vec_links[j].Obj().getObjectField( "state_q" );
      std::vector<mongo::BSONElement> vec_state_q;
      bson_state_q.elems( vec_state_q );
      for( unsigned k = 0; k < vec_state_q.size(); k++ )
        link->state.q( k, vec_state_q[k].Double() );

      mongo::BSONObj bson_state_dq = vec_links[j].Obj().getObjectField( "state_dq" );
      std::vector<mongo::BSONElement> vec_state_dq;
      bson_state_dq.elems( vec_state_dq );
      for( unsigned k = 0; k < vec_state_dq.size(); k++ )
        link->state.dq( k, vec_state_dq[k].Double() );

      model->links.push_back( link );
    }

    mongo::BSONObj bson_joints = bson_model.getObjectField( "joints" );
    std::vector<mongo::BSONElement> vec_joints;
    bson_joints.elems( vec_joints );
    for( unsigned j = 0; j < vec_joints.size(); j++ ) {
      Reveal::Core::joint_ptr joint = Reveal::Core::joint_ptr( new Reveal::Core::joint_c() );
      joint->id = vec_joints[j].Obj().getField( "id" ).String();

      mongo::BSONObj bson_control_u = vec_joints[j].Obj().getObjectField( "control_u" );
      std::vector<mongo::BSONElement> vec_control_u;
      bson_control_u.elems( vec_control_u );
      for( unsigned k = 0; k < vec_control_u.size(); k++ )
        joint->control.u( k, vec_control_u[k].Double() );

      model->joints.push_back( joint );
    }

    trial->models.push_back( model );
  }

  return true;
}

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


