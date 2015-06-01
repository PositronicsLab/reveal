/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

-----------------------------------------------------------------------------*/
#ifndef _REVEAL_CORE_XML_H_
#define _REVEAL_CORE_XML_H_

#include <string>
#include <vector>
#include <map>
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
  std::string   _name;      //< the xml attribute name
  std::string   _value;     //< the xml attribute value

public:
  /// Default constructor
  xml_attribute_c( void );
  /// Destructor
  virtual ~xml_attribute_c( void );
  
  /// Gets a pointer to this instance
  /// @return a pointer to this instance
  xml_attribute_ptr ptr( void );

  /// Gets the name of the attribute
  /// @return the name of this attribute
  std::string get_name( void );

  /// Sets the name of the attribute
  /// @param name the name to set as the attribute name
  void set_name( std::string name );

  /// Gets the value of the attribute
  /// @return the value of the attribute as a string
  std::string get_value( void );

  /// Sets the value of the attribute
  /// @param value a string representing the attribute value
  void set_value( std::string value );

  /// Sets the value of the attribute
  /// @param value an unsigned representing the attribute value
  void set_value( unsigned value );

  /// Sets the value of the attribute
  /// @param value an integer representing the attribute value
  void set_value( int value );

  /// Sets the value of the attribute
  /// @param value a double representing the attribute value
  void set_value( double value );
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class xml_element_c : public boost::enable_shared_from_this<xml_element_c> {
private:
  std::vector<xml_element_ptr> _elements;   //< the set of child elements
  std::string _name;                        //< the name of this xml element
  std::string _value;                       //< the value of this xml element

  std::map<std::string,unsigned> _attribute_map; //< the map of attributes
  std::vector<xml_attribute_ptr> _attributes;    //< the set of attributes

public:
  /// Default constructor
  xml_element_c( void );
  /// Destructor
  virtual ~xml_element_c( void );

  /// Gets a pointer to this instance
  /// @return a pointer to this instance
  xml_element_ptr ptr( void );

  /// Gets the number of attributes assigned to this element
  /// @return the number of attributes assigned to this element
  unsigned attributes( void );

  /// Gets an attribute by position
  /// @param i the index of the attribute to find.  Must be less than attributes
  /// @return a pointer to the attribute requested
  xml_attribute_ptr attribute( unsigned i );

  /// Gets an attribute by key name
  /// @param name the name of the attribute to find.
  /// @return a pointer to the attribute requested.  If the attribute is not 
  ///         found, an empty pointer is returned
  xml_attribute_ptr attribute( std::string name );

  /// Appends an attribute to the set of attributes assigned to this element
  /// @param attribute the attribute to append to this element
  void append( xml_attribute_ptr attribute );

  /// Gets the number of child elements assigned to this element
  /// @return the number of child elements assigned to this element
  unsigned elements( void );

  /// Gets an element by position
  /// @param the index of the element to find.  Must be less than elements
  /// @return a pointer to the element requested
  xml_element_ptr element( unsigned i );

  /// Appends an element to the set of child elements assigned to this element
  /// @param element the element to append to this element
  void append( xml_element_ptr element );

  /// Gets the name of this element
  /// @return the name of this element
  std::string get_name( void );

  /// Sets the name of this element
  /// @param the name to assign to this element
  void set_name( std::string name );

  /// Gets the value of this element
  /// @return the value of this element
  std::string get_value( void );

  /// Sets the value of this element
  /// @param value a string representing the element value
  void set_value( std::string value );

  /// Sets the value of this element
  /// @param value an unsigned value representing the element value
  void set_value( unsigned value );

  /// Sets the value of this element
  /// @param value an integer value representing the element value
  void set_value( int value );

  /// Sets the value of this element
  /// @param value a double value representing the element value
  void set_value( double value );

};
 
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class xml_c {
private:
  xml_element_ptr _root;    //< the root of the xml document
  
public:
  /// Default constructor
  xml_c( void );
  /// Destructor
  virtual ~xml_c( void );

  /// Reads an xml document from disk
  /// @param filename the path to the file to read
  /// @return true if the document was successfully read (and parsed as xml) OR
  ///         false if the operation failed for any reason
  bool read( std::string filename );

  /// Writes an xml hierarchy pointed to by root to disk
  /// @param filename the path to the file to write
  /// @return true if the document was successfully written OR false if the 
  ///         operation failed for any reason
  bool write( std::string filename );

  /// Sets the root element of the xml document
  /// @param element the element to set as root
  void root( xml_element_ptr element );

  /// Gets the root element of the xml document
  /// @return the root element of the xml document
  xml_element_ptr root( void );

private:
  /// Recursively process the tinyxml tree.  void pointer used here to mask
  /// the tiny xml library from the header and dependencies.
  /// @param tixml_element the tiny xml element to process
  /// @param element the xml element to copy data to
  /// @return true if processing succeeded OR false if the operation failed for
  ///         any reason
  bool process_tixml_element( void* tixml_element, xml_element_ptr element );

  /// Recursively build the tinyxml tree.  void pointer used here to mask
  /// the tiny xml library from the header and dependencies.
  /// @param tixml_element the tiny xml element to construct
  /// @param element the xml element to copy data from
  /// @return true if building succeeded OR false if the operation failed for
  ///         any reason
  bool build_tixml_element( void* tixml_element, xml_element_ptr element );
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_XML_H_
