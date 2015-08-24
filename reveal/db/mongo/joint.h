#ifndef _REVEAL_DB_MONGO_JOINT_H_
#define _REVEAL_DB_MONGO_JOINT_H_

#include "reveal/core/pointers.h"
#include "reveal/core/joint.h"
#include "reveal/db/mongo/mongo.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------

class joint_c {
public:
  /// inserts a vector of joints into a bson object builder
  /// @param bson_parent the parent bson object builder to insert into
  /// @param joints the vector of Reveal joints to map into the bson object
  /// @return true if insertion succeeded OR false if failed for any reason
  static bool insert( mongo::BSONObjBuilder& bson_parent, const std::vector<Reveal::Core::joint_ptr>& joints );

  /// inserts a joint into a bson array builder
  /// @param bson_array the bson array builder to insert into
  /// @param joint the Reveal joint to map into the bson array
  /// @return true if insertion succeeded OR false if failed for any reason
  static bool insert( mongo::BSONArrayBuilder& bson_array, Reveal::Core::joint_ptr joint );

  /// fetches the joint data from the bson object and maps into a Reveal model
  /// @param model the Reveal model to map joint data into
  /// @param bson_model the bson object to map joint data from
  /// @return true if fetch succeeded OR false if failed for any reason
  static bool fetch( Reveal::Core::model_ptr model, mongo::BSONObj bson_model );

  /// fetches the joint data from the bson object and maps into a Reveal joint
  /// @param model the Reveal joint to map joint data into
  /// @param bson_model the bson object to map joint data from
  /// @return true if fetch succeeded OR false if failed for any reason
  static bool fetch( Reveal::Core::joint_ptr joint, mongo::BSONObj bson );
};

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_MONGO_JOINT_H_
