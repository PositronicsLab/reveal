#ifndef _REVEAL_CLIENT_MANIFEST_H_
#define _REVEAL_CLIENT_MANIFEST_H_

#include <tinyxml.h>
#include <vector>
#include <string>

namespace Reveal {
namespace Client {

class manifest_c {
public: 
  manifest_c( void ) { }
  virtual ~manifest_c( void ) { }

  // TODO : add robust error handling
  bool parse( std::string path ) {
    std::string manifest_path = path + '/'+ "manifest.xml";
    printf( "manifest_path: %s\n", manifest_path.c_str() );

    TiXmlDocument doc( manifest_path.c_str() );
    bool result = doc.LoadFile();
    if( !result ) return false;
    
    TiXmlHandle hdoc( &doc );
    TiXmlHandle root( 0 ); 
    TiXmlElement* e;
    TiXmlHandle h( 0 );

    e = hdoc.FirstChildElement().Element();
    root = TiXmlHandle( e );

    h = root.FirstChild( "Product" );
    TiXmlElement* echild = h.ToElement();

    //TiXmlElement* echild = root.FirstChildElement();
 
    for( echild; echild; echild = echild->NextSiblingElement() ) {
      TiXmlAttribute* a = echild->FirstAttribute();
      std::string target, type, product; 
      while( a!= NULL ) {
        std::string name = a->Name();
        std::string value = a->Value();
        //printf("name: %s, value: %s\n", name.c_str(), value.c_str() );

        if( name == "target" ) {
          target = value;
        } else if( name == "type" ) {
          type = value;
        } else if( name == "file" ) {
          product = value;
          _build_products.push_back( product );
        }
        a = a->Next();
      }
    }

    for( std::vector<std::string>::iterator it = _build_products.begin(); it != _build_products.end(); it++ )
      printf( "build product: %s\n", it->c_str() );
    
    return true;
  }

  std::vector<std::string> build_products( void ) {
    std::vector<std::string> products;
    for( std::vector<std::string>::iterator it = _build_products.begin(); it != _build_products.end(); it++ )
      products.push_back( *it );
    return products;
  }

private:
  std::vector<std::string> _build_products;
  
};

} // namespace Client
} // namespace Reveal

#endif // _REVEAL_CLIENT_MANIFEST_H_
