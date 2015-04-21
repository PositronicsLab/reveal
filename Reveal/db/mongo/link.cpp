#include "Reveal/db/mongo/link.h"

#include "Reveal/core/model.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------

bool link_c::insert( mongo::BSONObjBuilder& bson_parent, const std::vector<Reveal::Core::link_ptr>& links ) {
  mongo::BSONArrayBuilder bab;

  for( unsigned i = 0; i < links.size(); i++ ) 
    insert( bab, links[i] ); 
  bson_parent.appendArray( "links", bab.arr() );

  return true;
}

//-----------------------------------------------------------------------------
bool link_c::insert( mongo::BSONArrayBuilder& bson_array, Reveal::Core::link_ptr link ) {
  mongo::BSONObjBuilder bob;
  bob.append( "id", link->id );

  mongo::BSONArrayBuilder bab_q;
  for( unsigned i = 0; i < link->state.size_q(); i++ )
    bab_q.append( link->state.q(i) );
  bob.appendArray( "state_q", bab_q.arr() );
  
  mongo::BSONArrayBuilder bab_dq;
  for( unsigned i = 0; i < link->state.size_dq(); i++ ) 
    bab_dq.append( link->state.dq(i) );
  bob.appendArray( "state_dq", bab_dq.arr() );

  bson_array.append( bob.obj() );

  return true;
}

//-----------------------------------------------------------------------------
bool link_c::fetch( Reveal::Core::model_ptr model, mongo::BSONObj bson_model ) {

  Reveal::Core::link_ptr link;
  mongo::BSONObj bson_links;
  std::vector<mongo::BSONElement> v_links;

  bson_links = bson_model.getObjectField( "links" );
  bson_links.elems( v_links );

  for( unsigned i = 0; i < v_links.size(); i++ ) {
    link = Reveal::Core::link_ptr( new Reveal::Core::link_c() );

    fetch( link, v_links[i].Obj() );

    model->links.push_back( link );
  }

  return true;
}
//-----------------------------------------------------------------------------
bool link_c::fetch( Reveal::Core::link_ptr link, mongo::BSONObj bson ) {

    assert( link );

    link->id = bson.getField( "id" ).String();

    mongo::BSONObj bson_q = bson.getObjectField( "state_q" );
    std::vector<mongo::BSONElement> v_q;
    bson_q.elems( v_q );

    for( unsigned i = 0; i < v_q.size(); i++ )
      link->state.q( i, v_q[i].Double() );

    mongo::BSONObj bson_dq = bson.getObjectField( "state_dq" );

    std::vector<mongo::BSONElement> v_dq;
    bson_dq.elems( v_dq );

    for( unsigned i = 0; i < v_dq.size(); i++ )
      link->state.dq( i, v_dq[i].Double() );

  return true;
}

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------


