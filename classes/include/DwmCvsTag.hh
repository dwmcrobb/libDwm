//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2000, 2016
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  3. The names of the authors and copyright holders may not be used to
//    endorse or promote products derived from this software without
//    specific prior written permission.
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
//!  \file DwmCvsTag.hh
//!  \brief Dwm::CvsTag class definition
//---------------------------------------------------------------------------

#ifndef _DWMCVSTAG_HH_
#define _DWMCVSTAG_HH_

#include <string>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class can be used to wrap a string containing CVS keyword 
  //!  name/value pairs.  The constructor accepts a string containing
  //!  CVS keyword/value pairs.  The other public members permit the 
  //!  retrieval of the value of the particular keyword.
  //--------------------------------------------------------------------------
  class CvsTag
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    CvsTag(const std::string & tagString);

    //------------------------------------------------------------------------
    //!  Returns the value of the Name tag.
    //------------------------------------------------------------------------
    std::string Name() const;

    //------------------------------------------------------------------------
    //!  Returns the value of the Id tag.
    //------------------------------------------------------------------------
    std::string Id() const;

    //------------------------------------------------------------------------
    //!  Returns the value of the Author tag.
    //------------------------------------------------------------------------
    std::string Author() const;

    //------------------------------------------------------------------------
    //!  Returns the value of the Header tag.
    //------------------------------------------------------------------------
    std::string Header() const;

    //------------------------------------------------------------------------
    //!  Returns the value of the Date tag.
    //------------------------------------------------------------------------
    std::string Date() const;

    //------------------------------------------------------------------------
    //!  Returns the value of the RCSfile tag.
    //------------------------------------------------------------------------
    std::string RCSfile() const;
    
    //------------------------------------------------------------------------
    //!  Returns the value of the Revision tag.
    //------------------------------------------------------------------------
    std::string Revision() const;

  private:
    std::string  _tagString;

    std::string GetRCSfileFromId() const;
    std::string GetRevisionFromId() const;
    std::string GetDateFromId() const;
    std::string GetAuthorFromId() const;
    std::string GetTag(const std::string & tag) const;
  };
  
}  // namespace Dwm

#endif  // _DWMCVSTAG_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
