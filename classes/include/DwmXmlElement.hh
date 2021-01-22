//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmXmlElement.hh 8401 $
// @(#) $Id: DwmXmlElement.hh 8401 2016-04-17 06:44:31Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2008
//  All rights reserved.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmXmlElement.hh
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMXMLELEMENT_HH_
#define _DWMXMLELEMENT_HH_

#include <iostream>
#include <map>
#include <string>

#include "DwmXmlAttribute.hh"

namespace Dwm {

  namespace Xml {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    class Element
    {
    public:
      Element(const std::string & name, const std::string & value = "");
      const std::string & Name() const;
      const std::string & Value() const;
      const std::string & Value(const std::string & value);
      const std::map<std::string,Attribute> & Attributes() const;
      const std::multimap<std::string,Element> & Children() const;
      
      bool AddAttribute(const Attribute & attribute);
      bool RemoveAttribute(const std::string & attributeName);
      bool AddElement(const Element & element);

      friend std::ostream & operator << (std::ostream & os,
                                         const Element & element);
      
    protected:
      std::string                         _name;
      std::string                         _value;
      std::map<std::string,Attribute>     _attributes;
      std::multimap<std::string,Element>  _children;
    };
    
      
  }  // namespace Xml

}  // namespace Dwm

#endif  // _DWMXMLELEMENT_HH_
