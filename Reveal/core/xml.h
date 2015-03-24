#ifndef _REVEAL_CORE_XML_H_
#define _REVEAL_CORE_XML_H_

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class xml_attribute_c;
typedef boost::shared_ptr<xml_attribute_c> xml_attribute_ptr;
class xml_element_c;
typedef boost::shared_ptr<xml_element_c> xml_element_ptr;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class xml_attribute_c : public boost::enable_shared_from_this<xml_attribute_c> {
private:
  std::string   _name;
  std::string   _value;

public:
  xml_attribute_c( void );
  virtual ~xml_attribute_c( void );
  
  xml_attribute_ptr ptr( void );

  std::string get_name( void );
  void set_name( std::string name );

  std::string get_value( void );
  void set_value( std::string value );
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class xml_element_c : public boost::enable_shared_from_this<xml_element_c> {
private:
  std::vector<xml_attribute_ptr> _attributes;
  std::vector<xml_element_ptr> _elements;
  std::string _name;
  std::string _value;

public:
  xml_element_c( void );
  virtual ~xml_element_c( void );

  xml_element_ptr ptr( void );

  unsigned attributes( void );
  xml_attribute_ptr attribute( unsigned i );
  void append( xml_attribute_ptr attribute );

  unsigned elements( void );
  xml_element_ptr element( unsigned i );
  void append( xml_element_ptr element );

  std::string get_name( void );
  void set_name( std::string name );
  std::string get_value( void );
  void set_value( std::string value );
  void set_value( unsigned value );
  void set_value( int value );
  void set_value( double value );

};
 
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class xml_c {
private:
  xml_element_ptr _root;
  
public:
  xml_c( void );
  virtual ~xml_c( void );

  bool read( std::string filename );
  bool write( std::string filename );

  void root( xml_element_ptr element );
  xml_element_ptr root( void );

private:
  // recursively process the tinyxml tree.  void pointer used here to mask
  // the tiny xml library from the header.
  bool process_tixml_element( void* tixml_element, xml_element_ptr element );
  // recursively build the tinyxml tree.  void pointer used here to mask
  // the tiny xml library from the header.
  bool build_tixml_element( void* tixml_element, xml_element_ptr element );
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_XML_H_
