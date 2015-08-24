#include "reveal/db/mongo/model.h"

#include "reveal/db/mongo/link.h"
#include "reveal/db/mongo/joint.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------

bool model_c::insert( mongo::BSONObjBuilder& bson_parent, const std::vector<Reveal::Core::model_ptr>& models, bool insert_controls ) {

  mongo::BSONArrayBuilder bab;

  for( unsigned i = 0; i < models.size(); i++ ) 
    insert( bab, models[i], insert_controls );
  
  bson_parent.appendArray( "models", bab.arr() );

  return true;
}
//-----------------------------------------------------------------------------
bool model_c::insert( mongo::BSONArrayBuilder& bson_array, Reveal::Core::model_ptr model, bool insert_controls ) {
  mongo::BSONObjBuilder bob;
  bob.append( "id", model->id );

  link_c::insert( bob, model->links );

  if( insert_controls ) 
    joint_c::insert( bob, model->joints );
  
  bson_array.append( bob.obj() );

  return true;
}

//-----------------------------------------------------------------------------
bool model_c::fetch( Reveal::Core::trial_ptr trial, mongo::BSONObj record ) {

  Reveal::Core::model_ptr model;
  std::vector<mongo::BSONElement> v_models;
  mongo::BSONObj bson_models;

  bson_models = record.getObjectField( "models" );
  bson_models.elems( v_models );

  for( unsigned i = 0; i < v_models.size(); i++ ) {
    model = Reveal::Core::model_ptr( new Reveal::Core::model_c() );
    mongo::BSONObj bson_model = v_models[i].Obj();

    fetch( model, bson_model, true );

    trial->models.push_back( model );
  }

  return true;
}

//-----------------------------------------------------------------------------
bool model_c::fetch( Reveal::Core::solution_ptr solution, mongo::BSONObj record ) {

  Reveal::Core::model_ptr model;
  std::vector<mongo::BSONElement> v_models;
  mongo::BSONObj bson_models;

  bson_models = record.getObjectField( "models" );
  bson_models.elems( v_models );

  for( unsigned i = 0; i < v_models.size(); i++ ) {
    model = Reveal::Core::model_ptr( new Reveal::Core::model_c() );
    mongo::BSONObj bson_model = v_models[i].Obj();

    fetch( model, bson_model, false );

    solution->models.push_back( model );
  }

  return true;
}

//-----------------------------------------------------------------------------
bool model_c::fetch( Reveal::Core::model_ptr model, mongo::BSONObj bson_model, bool fetch_controls ) {
  assert( model );

  model->id = bson_model.getField( "id" ).String();
  
  link_c::fetch( model, bson_model );

  if( fetch_controls )
    joint_c::fetch( model, bson_model );
  
  return true;
}

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


