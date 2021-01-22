//===========================================================================
// @(#) $Name$
// @(#) $Id: DwmXmlElement.cc 8388 2016-04-17 03:23:45Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2008
//  All rights reserved.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmXmlElement.cc
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <string>

#include "DwmXmlElement.hh"
#include "DwmSvnTag.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmXmlElement.cc 8388 $");

using namespace std;

namespace Dwm {

  namespace Xml {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Element::Element(const string & name, const string & value)
        : _name(name), _value(value), _attributes(), _children()
    {}
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const string & Element::Name() const
    {
      return(_name);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const string & Element::Value() const
    {
      return(_value);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const string & Element::Value(const string & value)
    {
      _value = value;
      return(_value);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const map<string,Attribute> & Element::Attributes() const
    {
      return(_attributes);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const multimap<string,Element> & Element::Children() const
    {
      return(_children);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Element::AddAttribute(const Attribute & attribute)
    {
      bool  rc = false;
      if (! attribute.Name().empty()) {
        if (_attributes.find(attribute.Name()) == _attributes.end()) {
          _attributes[attribute.Name()] = attribute;
          rc = true;
        }
      }
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Element::RemoveAttribute(const string & attributeName)
    {
      bool  rc = false;
      map<string,Attribute>::iterator  i = 
        _attributes.find(attributeName);
      if (i != _attributes.end()) {
        _attributes.erase(i);
        rc = true;
      }
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Element::AddElement(const Element & element)
    {
      bool  rc = false;
      if (! element.Name().empty()) {
        pair<string,Element>  value(element.Name(),element);
        _children.insert(value);
        rc = true;
      }
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ostream & operator << (ostream & os, const Element & element)
    {
      if (os) {
        os << "<" << element.Name();
        if (! element._attributes.empty()) {
          map<string,Attribute>::const_iterator  ai;
          for (ai = element._attributes.begin();
               ai != element._attributes.end(); ++ai) {
            os << " " << ai->second;
          }
        }
        
        if (element._children.empty()) {
          os << "/>\n";
        }
        else {
          os << ">";
          multimap<string,Element>::const_iterator  ci;
          for (ci = element._children.begin();
               ci != element._children.end(); ++ci) {
            os << ci->second;
          }
          os << "<" << element._name << ">\n";
        }
      }
      return(os);
    }
    
        
    
  }  // namespace Xml

}  // namespace Dwm
