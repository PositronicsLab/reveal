#include <Reveal/client.h>

int main( int argc, char* argv[] ) {
  Reveal::Client client;

  client.Init();
  client.Go();
  client.Terminate();

  return 0;
}
