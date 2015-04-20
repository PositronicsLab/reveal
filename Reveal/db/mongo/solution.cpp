#include "Reveal/db/mongo/solution.h"

#include <mongo/client/dbclient.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
bool solution_c::insert( Reveal::DB::database_ptr db, Reveal::Core::solution_ptr solution ) {
  std::string table;

  mongo::BSONObjBuilder bob;
  if( solution->type == Reveal::Core::solution_c::CLIENT )
    bob.append( "experiment_id", solution->experiment_id );
  bob.append( "scenario_id", solution->scenario_id );
  bob.append( "trial_id", solution->trial_id );
  bob.append( "t", solution->t );
  bob.append( "dt", solution->dt );

  mongo::BSONArrayBuilder bab_models;
  for( unsigned i = 0; i < solution->models.size(); i++ ) {
    Reveal::Core::model_ptr model = solution->models[i];
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

    bab_models.append( bob_model.obj() );
  }
  bob.appendArray( "models", bab_models.arr() );

  if( solution->type == Reveal::Core::solution_c::CLIENT )
    table = "solution";
  else if( solution->type == Reveal::Core::solution_c::MODEL )
    table = "model";

  // get mongo service and verify
  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  return mongo->insert( table, bob.obj() );
}

//-----------------------------------------------------------------------------
bool solution_c::fetch( Reveal::Core::solution_ptr& solution, Reveal::DB::database_ptr db, Reveal::Core::solution_c::type_e type, std::string scenario_id, unsigned trial_id ) {
  std::auto_ptr<mongo::DBClientCursor> cursor;
  Reveal::Core::scenario_ptr ptr;
  std::string table;

  if( type == Reveal::Core::solution_c::CLIENT )
    table = "solution";
  else if( type == Reveal::Core::solution_c::MODEL )
    table = "model";

  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  mongo->fetch( cursor, table, QUERY( "scenario_id" << scenario_id << "trial_id" << trial_id ) );

  if( !cursor->more() ) return false;

  // add error handling
  mongo::BSONObj record = cursor->next();

  solution = Reveal::Core::solution_ptr( new Reveal::Core::solution_c( type ) );

  if( type == Reveal::Core::solution_c::CLIENT )
    solution->experiment_id = record.getField( "experiment_id" ).String();
  solution->scenario_id = record.getField( "scenario_id" ).String();
  solution->trial_id = record.getField( "trial_id" ).Int();
  solution->t = record.getField( "t" ).Double();
  solution->dt = record.getField( "dt" ).Double();

  mongo::BSONObj bson_models = record.getObjectField( "models" );
  std::vector<mongo::BSONElement> vec_models;
  bson_models.elems( vec_models );
  for( unsigned i = 0; i < vec_models.size(); i++ ) {
    Reveal::Core::model_ptr model = Reveal::Core::model_ptr( new Reveal::Core::model_c() );
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
    solution->models.push_back( model );
  }

  return true;
}

//-----------------------------------------------------------------------------
bool solution_c::fetch( Reveal::Core::solution_ptr& solution, Reveal::DB::database_ptr db, std::string experiment_id, std::string scenario_id, unsigned trial_id ) {
  std::auto_ptr<mongo::DBClientCursor> cursor;
  Reveal::Core::scenario_ptr ptr;
  std::string table = "solution";

  mongo_ptr mongo = mongo_c::service( db );
  if( !mongo ) return false;

  mongo->fetch( cursor, table, QUERY( "experiment_id" << experiment_id << "scenario_id" << scenario_id << "trial_id" << trial_id ) );

  if( !cursor->more() ) return false;

  // add error handling
  mongo::BSONObj record = cursor->next();

  solution = Reveal::Core::solution_ptr( new Reveal::Core::solution_c( Reveal::Core::solution_c::CLIENT ) );

  solution->experiment_id = record.getField( "experiment_id" ).String();
  solution->scenario_id = record.getField( "scenario_id" ).String();
  solution->trial_id = record.getField( "trial_id" ).Int();
  solution->t = record.getField( "t" ).Double();
  solution->dt = record.getField( "dt" ).Double();

  mongo::BSONObj bson_models = record.getObjectField( "models" );
  std::vector<mongo::BSONElement> vec_models;
  bson_models.elems( vec_models );
  for( unsigned i = 0; i < vec_models.size(); i++ ) {
    Reveal::Core::model_ptr model = Reveal::Core::model_ptr( new Reveal::Core::model_c() );
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
    solution->models.push_back( model );
  }

  return true;
}
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


