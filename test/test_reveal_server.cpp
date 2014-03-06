#include <Reveal/server.h>

int main( int argc, char* argv[] ) {
  Reveal::Server server;

  server.Init();
  server.Run();
  server.Terminate();

  return 0;
}
