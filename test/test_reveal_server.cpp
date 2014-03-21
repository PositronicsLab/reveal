#include <Reveal/server.h>

int main( int argc, char* argv[] ) {
  Reveal::server_c server;

  server.init();
  server.run();
  server.terminate();

  return 0;
}
