#include <reveal/server.h>

int main( int argc, char* argv[] ) {
  reveal::Server server;

  server.Init();
  server.Run();
  server.Terminate();

  return 0;
}
