#include <reveal/client.h>

int main( int argc, char* argv[] ) {
  reveal::Client client;

  client.Init();
  client.Connect();

  return 0;
}
