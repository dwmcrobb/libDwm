//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmRegistry.hh 8401 $
// @(#) $Id: DwmRegistry.hh 8401 2016-04-17 06:44:31Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2016
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
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmRegistry.hh
//!  \brief Dwm::Registry class definition
//---------------------------------------------------------------------------

#ifndef _DWMREGISTRY_HH_
#define _DWMREGISTRY_HH_

#include <map>
#include <string>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class encapsulates a simple registry file, which contains
  //!  lines of the form:
  //!    key = "value"
  //!  If value contains quotes, they must be escaped in the registry file.
  //!  For example:
  //!    dwm = "Daniel \"Dano\" McRobb"
  //!  The Save() member will take care of escaping quotes for the caller.
  //!  Comments lines are permitted in the registry file; they must
  //!  start with a '#' in the first column.  Note that comments will
  //!  not be saved by Save(); this may be fixed at a later date.
  //--------------------------------------------------------------------------
  class Registry
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    Registry(const std::string & filename);

    bool Reload();

    //------------------------------------------------------------------------
    //!  Save the registry to its file.  Returns true on success.
    //------------------------------------------------------------------------
    bool Save() const;

    //------------------------------------------------------------------------
    //!  Gets the value for \c key in the registry.  If \ key does not exist
    //!  in the registry, returns an empty string.
    //------------------------------------------------------------------------
    std::string Get(const std::string & key) const;

    //------------------------------------------------------------------------
    //!  Sets the \c value for \c key in the registry.  Returns the
    //!  value.
    //------------------------------------------------------------------------
    const std::string & Set(const std::string & key, 
                            const std::string & value);
    
  protected:
    std::string                        _filename;
    std::map<std::string,std::string>  _map;
  };
  
  
}  // namespace Dwm

#endif  // _DWMREGISTRY_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
