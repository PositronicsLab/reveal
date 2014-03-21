#include <Reveal/client.h>

int main( int argc, char* argv[] ) {
  Reveal::client_c client;

  client.init();
  client.go();
  client.terminate();

  return 0;
}
