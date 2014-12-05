#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <sstream>

#include <Reveal/pointers.h>
#include <Reveal/user.h>

#include <Reveal/database.h>

int main( void ) {
  boost::shared_ptr<Reveal::DB::database_c> _db;

  _db = boost::shared_ptr<Reveal::DB::database_c>( new Reveal::DB::database_c( "localhost" ) );
  _db->open();

  Reveal::Core::user_ptr user;

  // insertions

  user = Reveal::Core::user_ptr( new Reveal::Core::user_c() );
  user->id = "alice";
  _db->insert( user );
  printf( "inserted %s into user table\n", user->id.c_str() );

  user = Reveal::Core::user_ptr( new Reveal::Core::user_c() );
  user->id = "bob";
  _db->insert( user );
  printf( "inserted %s into user table\n", user->id.c_str() );

  user = Reveal::Core::user_ptr( new Reveal::Core::user_c() );
  user->id = "charlie";
  _db->insert( user );
  printf( "inserted %s into user table\n", user->id.c_str() );

  // -- validate --

  //user = Reveal::Core::user_ptr( new Reveal::Core::user_c() );
  if( _db->query( user, "alice" ) != Reveal::DB::database_c::ERROR_NONE ) 
    printf( "ERROR: failed to find alice\n" );
  else
    printf( "SUCCESS: queried for alice and found %s\n", user->id.c_str() );
  
  if( _db->query( user, "bob" ) != Reveal::DB::database_c::ERROR_NONE ) 
    printf( "ERROR: failed to find bob\n" );
  else
    printf( "SUCCESS: queried for bob and found %s\n", user->id.c_str() );
  
  if( _db->query( user, "charlie" ) != Reveal::DB::database_c::ERROR_NONE ) 
    printf( "ERROR: failed to find charlie\n" );
  else
    printf( "SUCCESS: queried for charlie and found %s\n", user->id.c_str() );
  
  if( _db->query( user, "dave" ) != Reveal::DB::database_c::ERROR_NONE ) 
    printf( "SUCCESS: queried for dave and did not find dave\n" );
  else
    printf( "ERROR: queried for dave and found %s\n", user->id.c_str() );
  

  
  

  _db->close();
}
