#include <stdio.h>
#include <ostream>
#include <stdlib.h>

#include "Reveal/core/system.h"
#include "Reveal/db/database.h"
#include "Reveal/db/mongo/mongo.h"

#include <mongo/client/dbclient.h>

//-----------------------------------------------------------------------------
bool drop_database( Reveal::DB::Mongo::mongo_ptr mongodb ) {

  bool result;

  std::cout << "Dropping database... ";

  result = mongodb->drop_database( );
  if( !result ) {
    std::cout << "Failed" << std::endl;
    return false;
  }

  std::cout << "OK" << std::endl;
 
  return true; 
}

//-----------------------------------------------------------------------------
bool create_trial_indices( Reveal::DB::Mongo::mongo_ptr mongodb ) {

  bool result;
  std::string table = "trial";

  std::cout << "Creating indices on table: " << table << "... ";

  result = mongodb->create_index( table, BSON( "scenario_id" << 1 << "t" << 1 ) );
  if( !result ) {
    std::cout << "Failed" << std::endl;
    return false;
  }

  std::cout << "OK" << std::endl;
 
  return true; 
}

//-----------------------------------------------------------------------------
bool create_model_indices( Reveal::DB::Mongo::mongo_ptr mongodb ) {

  bool result;
  std::string table = "model";

  std::cout << "Creating indices on table: " << table << "... ";

  result = mongodb->create_index( table, BSON( "scenario_id" << 1 << "t" << 1 ) );
  if( !result ) {
    std::cout << "Failed" << std::endl;
    return false;
  }

  std::cout << "OK" << std::endl;
 
  return true; 
}

//-----------------------------------------------------------------------------
bool create_solution_indices( Reveal::DB::Mongo::mongo_ptr mongodb ) {

  bool result;
  std::string table = "solution";

  std::cout << "Creating indices on table: " << table << "... ";

  result = mongodb->create_index( table, BSON( "scenario_id" << 1 << "experiment_id" << 1 << "t" << 1 ) );
  if( !result ) {
    std::cout << "Failed" << std::endl;
    return false;
  }

  std::cout << "OK" << std::endl;
 
  return true; 
}

//-----------------------------------------------------------------------------
int main( void ) {
  std::cout << "Attempting to configure Reveal dataservice." << std::endl;

  std::cout << "Reading system parameters... ";
  Reveal::Core::system_c system( Reveal::Core::system_c::SERVER );
  if( !system.open() ) {
    std::cout << "Failed" << std::endl;
    std::cout << "ERROR: Failed to open the Reveal system." << std::endl;
    std::cout << "Make sure the Reveal environment variables are set before running this utility." << std::endl;
    return 1;
  }
  std::cout << "OK" << std::endl;

  std::cout << "Opening mongo backend service... ";
  boost::shared_ptr<Reveal::DB::database_c> db = boost::shared_ptr<Reveal::DB::database_c>( new Reveal::DB::database_c() );
  if( !db->open() ) {
    std::cout << "Failed" << std::endl;
    std::cout << "ERROR: Failed to open a mongo connection." << std::endl;
    std::cout << "Make sure the mongo service is running." << std::endl;
    return 1; 
  }
  std::cout << "OK" << std::endl;

  std::cout << "Opening Reveal/mongo interface... ";
  Reveal::DB::Mongo::mongo_ptr mongo = Reveal::DB::Mongo::mongo_c::service( db );
  if( !mongo ) {
    std::cout << "Failed" << std::endl;
    std::cout << "ERROR: Failed to get a mongo instance." << std::endl;
    return 1; 
  }
  std::cout << "OK" << std::endl;

  if( !drop_database( mongo ) ) {
    std::cout << "ERROR: Failed to drop existing database" << std::endl;
    return 1;
  }
  if( !create_trial_indices( mongo ) ) {
    std::cout << "ERROR: Failed to create indices on table!" << std::endl;
    return 1;
  }
  if( !create_model_indices( mongo ) ) {
    std::cout << "ERROR: Failed to create indices on table!" << std::endl;
    return 1;
  }
  if( !create_solution_indices( mongo ) ) {
    std::cout << "ERROR: Failed to create indices on table!" << std::endl;
    return 1;
  }

  std::cout << "Success: Reveal dataservice configured" << std::endl;

  return 0;
}

//-----------------------------------------------------------------------------
