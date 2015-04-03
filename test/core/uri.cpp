#include <Reveal/core/system.h>

#include <string>
#include <stdio.h>

int main( int argc, char* argv[] ) {

  std::string good_uri = "http://localhost:12345";
  std::string no_protocol_uri = "localhost:12345";
  std::string no_port_uri = "http://localhost";

  std::string protocol, host;
  unsigned port;

  if( !split_uri( good_uri, protocol, host, port ) ) {
    printf( "FAILED to split good_uri\n" );
  } else {
    printf( "PASSED split good_uri[%s] into protocol[%s], host[%s], port[%u]\n", good_uri.c_str(), protocol.c_str(), host.c_str(), port );
  }

  if( !split_uri( no_protocol_uri, protocol, host, port ) ) {
    printf( "PASSED failed to split no_protocol_uri\n" );
  } else {
    printf( "FAILED split a uri that is missing protocol, uri[%s]\n", no_protocol_uri.c_str() );
  }

  if( !split_uri( no_port_uri, protocol, host, port ) ) {
    printf( "PASSED failed to split no_port_uri\n" );
  } else {
    printf( "FAILED split a uri that is missing port, uri[%s]\n", no_port_uri.c_str() );
  }


  return 0;
}
