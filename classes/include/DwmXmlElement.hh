//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2008
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//  3. The names of the authors and copyright holders may not be used to
//     endorse or promote products derived from this software without
//     specific prior written permission.
//
//  IN NO EVENT SHALL DANIEL W. MCROBB BE LIABLE TO ANY PARTY FOR
//  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
//  INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE,
//  EVEN IF DANIEL W. MCROBB HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
//
//  THE SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND
//  DANIEL W. MCROBB HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
//  UPDATES, ENHANCEMENTS, OR MODIFICATIONS. DANIEL W. MCROBB MAKES NO
//  REPRESENTATIONS AND EXTENDS NO WARRANTIES OF ANY KIND, EITHER
//  IMPLIED OR EXPRESS, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE,
//  OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmXmlElement.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::Xml::Element class declaration
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


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
