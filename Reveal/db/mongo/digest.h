#ifndef _REVEAL_DB_MONGO_DIGEST_H_
#define _REVEAL_DB_MONGO_DIGEST_H_

#include "Reveal/core/pointers.h"
#include "Reveal/core/digest.h"
#include "Reveal/db/mongo/mongo.h"
#include "Reveal/db/database.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------

class digest_c {
public:

  /// fetches the set of all scenarios from the database
  /// @param digest instance populated with data on success
  /// @param db the reveal/mongo database to fetch the record from
  /// @return true if the instance was populated OR false if fetch failed
  static bool fetch( Reveal::Core::digest_ptr& digest, database_ptr db );
};

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_MONGO_DIGEST_H_
