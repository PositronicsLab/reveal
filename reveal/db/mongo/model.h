#ifndef _REVEAL_DB_MONGO_MODEL_H_
#define _REVEAL_DB_MONGO_MODEL_H_

#include "reveal/core/pointers.h"
#include "reveal/core/model.h"
#include "reveal/core/trial.h"
#include "reveal/core/solution.h"
#include "reveal/db/mongo/mongo.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------
namespace Mongo {
//-----------------------------------------------------------------------------

class model_c {
public:
  /// inserts a vector of models into a bson object builder
  /// @param bson_parent the parent bson object builder to insert into
  /// @param models the vector of Reveal models to map into the bson object
  /// @param insert_controls determines if control data from the models should
  ///        be mapped into the bson object along with state data
  /// @return true if insertion succeeded OR false if failed for any reason
  static bool insert( mongo::BSONObjBuilder& bson_parent, const std::vector<Reveal::Core::model_ptr>& models, bool insert_controls = false );

  /// inserts a model into a bson array builder
  /// @param bson_array the bson array builder to insert into
  /// @param model the Reveal models to map into the bson object
  /// @param insert_controls determines if control data from the model should
  ///        be mapped into the bson object along with state data
  /// @return true if insertion succeeded OR false if failed for any reason
  static bool insert( mongo::BSONArrayBuilder& bson_array, Reveal::Core::model_ptr model, bool insert_controls = false );

  /// fetches all model data from the bson object and maps into a Reveal trial
  /// @param trial the Reveal trial to map model data into
  /// @param record the bson object to map model data from
  /// @return true if fetch succeeded OR false if failed for any reason
  static bool fetch( Reveal::Core::trial_ptr trial, mongo::BSONObj record );

  /// fetches all models from the bson object and maps into a Reveal solution
  /// @param solution the Reveal solution to map model data into
  /// @param record the bson object to map model data from
  /// @return true if fetch succeeded OR false if failed for any reason
  static bool fetch( Reveal::Core::solution_ptr solution, mongo::BSONObj record );

  /// fetches model data from the bson object and maps into a Reveal model
  /// @param model the Reveal model to map model data into
  /// @param bson_model the bson object to map model data from
  /// @param fetch_controls determines if control data exists for the bson 
  ///        record and should be mapped into the model along with state data
  /// @return true if fetch succeeded OR false if failed for any reason
  static bool fetch( Reveal::Core::model_ptr model, mongo::BSONObj bson_model, bool fetch_controls );

};

//-----------------------------------------------------------------------------
} // namespace Mongo
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_MONGO_MODEL_H_
