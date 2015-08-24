#ifndef _REVEAL_DB_MONGO_LINK_H_
#define _REVEAL_DB_MONGO_LINK_H_

#include "reveal/core/pointers.h"
#include "reveal/core/link.h"
#include "reveal/db/mongo/mongo.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------

class link_c {
public:
  /// inserts a vector of links into a bson object builder
  /// @param bson_parent the parent bson object builder to insert into
  /// @param links the vector of Reveal links to map into the bson object
  /// @return true if insertion succeeded OR false if failed for any reason
  static bool insert( mongo::BSONObjBuilder& bson_parent, const std::vector<Reveal::Core::link_ptr>& links );

  /// inserts a link into a bson array builder
  /// @param bson_array the bson array builder to insert into
  /// @param link the Reveal link to map into the bson array
  /// @return true if insertion succeeded OR false if failed for any reason
  static bool insert( mongo::BSONArrayBuilder& bson_array, Reveal::Core::link_ptr link );

  /// fetches the link data from the bson object and maps into a Reveal model
  /// @param model the Reveal model to map link data into
  /// @param bson_model the bson object to map link data from
  /// @return true if fetch succeeded OR false if failed for any reason
  static bool fetch( Reveal::Core::model_ptr model, mongo::BSONObj bson_model );

  /// fetches the link data from the bson object and maps into a Reveal link
  /// @param model the Reveal link to map link data into
  /// @param bson_model the bson object to map link data from
  /// @return true if fetch succeeded OR false if failed for any reason
  static bool fetch( Reveal::Core::link_ptr link, mongo::BSONObj bson );
};

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_MONGO_LINK_H_
