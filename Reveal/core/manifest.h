#ifndef _REVEAL_CORE_MANIFEST_H_
#define _REVEAL_CORE_MANIFEST_H_
//-----------------------------------------------------------------------------

#include <vector>
#include <string>

#include "Reveal/core/xml.h"
//#include <stdio.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class manifest_c {
public: 
  manifest_c( void ) { }
  virtual ~manifest_c( void ) { }

  // TODO : add robust error handling
  bool parse( std::string path ) {
    std::string manifest_path = path + '/'+ "manifest.xml";

    xml_c xml;
    xml.read( manifest_path );

    xml_element_ptr root, element;
    root = xml.root();

    for( unsigned i = 0; i < root->elements(); i++ ) {
      element = root->element( i );
      if( element->get_name() == "Product" ) {
        for( unsigned j = 0; j < element->attributes(); j++ ) {
          xml_attribute_ptr attrib = element->attribute( j );
          if( attrib->get_name() == "file" )
            _build_products.push_back( attrib->get_value() );
        }
/*
        if( name == "target" ) {
          target = value;
        } else if( name == "type" ) {
          type = value;
        } else if( name == "file" ) {
          product = value;
          _build_products.push_back( product );
        }
*/
      }
    }
/*
    for( std::vector<std::string>::iterator it = _build_products.begin(); it != _build_products.end(); it++ )
      printf( "build product: %s\n", it->c_str() );
*/    
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

//-----------------------------------------------------------------------------
} // namespace Client
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
#endif // _REVEAL_CLIENT_MANIFEST_H_
