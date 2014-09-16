#include <uuid/uuid.h>
#include <stdio.h>


int main( void ) {
//  void uuid_generate(uuid_t out);void uuid_generate_random(uuid_t out);void
//    uuid_generate_time(uuid_t out);int uuid_generate_time_safe(uuid_t out);

  uuid_t uuid;
  uuid_generate( uuid );

  //std::cout << uuid << std::endl;
  printf( "%X\n", uuid );

  return 0;
}
