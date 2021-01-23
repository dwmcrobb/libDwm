//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2000-2007
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

#include "DwmCvsTag.hh"

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  CvsTag::CvsTag(const string & tagString)
      : _tagString(tagString)
  {
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string CvsTag::GetRCSfileFromId() const
  {
    string  rc;
    string::size_type  idPos = this->_tagString.find("$Id: ");
    if (idPos != this->_tagString.npos) {
      string::size_type  endPos;
      // parse the RCSfile
      idPos += 5;
      endPos = this->_tagString.find_first_of(" ",idPos);
      rc.assign(this->_tagString,idPos,endPos - idPos);
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string CvsTag::GetRevisionFromId() const
  {
    string  rc;
    string::size_type  idPos = this->_tagString.find("$Id: ");
    if (idPos != this->_tagString.npos) {
      string::size_type  endPos;
      // skip the RCSfile
      idPos += 5;
      endPos = this->_tagString.find_first_of(" ",idPos);
      idPos = endPos + 1;
      // grab the revision
      endPos = this->_tagString.find_first_of(" ",idPos);
      rc.assign(this->_tagString,idPos,endPos - idPos);
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string CvsTag::GetDateFromId() const
  {
    string  rc;
    string::size_type  idPos = this->_tagString.find("$Id: ");
    if (idPos != this->_tagString.npos) {
      string::size_type  endPos;
      // skip the RCSfile
      idPos += 5;
      endPos = this->_tagString.find_first_of(" ",idPos);
      idPos = endPos + 1;
      // skip the revision
      endPos = this->_tagString.find_first_of(" ",idPos);
      idPos = endPos + 1;
      // snarf date and time
      endPos = this->_tagString.find_first_of(" ",idPos);
      ++endPos;
      endPos = this->_tagString.find_first_of(" ",endPos);
      rc.assign(this->_tagString,idPos,endPos - idPos);
    }
    return(rc);
  }
    
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string CvsTag::GetAuthorFromId() const
  {
    string  rc;
    string::size_type  idPos = this->_tagString.find("$Id: ");
    if (idPos != this->_tagString.npos) {
      string::size_type  endPos;
      // skip the RCSfile
      idPos += 5;
      endPos = this->_tagString.find_first_of(" ",idPos);
      idPos = endPos + 1;
      // skip the revision
      endPos = this->_tagString.find_first_of(" ",idPos);
      idPos = endPos + 1;
      // skip date and time
      endPos = this->_tagString.find_first_of(" ",idPos);
      idPos = endPos + 1;
      endPos = this->_tagString.find_first_of(" ",idPos);
      idPos = endPos + 1;
      endPos = this->_tagString.find_first_of(" ",idPos);
      // snarf the author
      rc.assign(this->_tagString,idPos,endPos - idPos);
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
    string CvsTag::GetTag(const string & tag) const
  {
    string             rc;
    string::size_type  startPos = this->_tagString.find(tag);
    if (startPos != this->_tagString.npos) {
      startPos += tag.length();
      string::size_type  endPos = startPos;
      while ((endPos < this->_tagString.length()) && 
             (this->_tagString[endPos] != '$'))
        ++endPos;
      --endPos;
      rc.assign(this->_tagString,startPos,endPos-startPos);
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string CvsTag::Name() const
  {
    string rc = this->GetTag("$Name: ");
    if (rc.find_first_not_of(" ") == rc.npos)
      rc = "";
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string CvsTag::Id() const
  {
    return(this->GetTag("$Id: "));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string CvsTag::Author() const
  {
    string rc = this->GetTag("$Author: ");
    if (rc.empty())
      rc = this->GetAuthorFromId();
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string CvsTag::Header() const
  {
    return(this->GetTag("$Header: "));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string CvsTag::Date() const
  {
    string rc = this->GetTag("$Date: ");
    if (rc.empty())
      rc = this->GetDateFromId();
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string CvsTag::RCSfile() const
  {
    string rc = this->GetTag("$RCSfile: ");
    if (rc.empty())
      rc = this->GetRCSfileFromId();
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string CvsTag::Revision() const
  {
    string rc = this->GetTag("$Revision: ");
    if (rc.empty())
      rc = this->GetRevisionFromId();
    return(rc);
  }
  
}  // namespace Dwm


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
