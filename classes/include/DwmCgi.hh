//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmCgi.hh 8401 $
// @(#) $Id: DwmCgi.hh 8401 2016-04-17 06:44:31Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 1999-2001, 2016
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
//!  \file DwmCgi.hh
//!  \brief Dwm::Cgi class definition
//---------------------------------------------------------------------------

#ifndef _CGI_HH_
#define _CGI_HH_

#include <string>
#include <map>

namespace Dwm {

  //--------------------------------------------------------------------------
  //                          class Cgi
  //--------------------------------------------------------------------------
  //!  This class encapsulates CGI input.  It is used for storing
  //!  name/value pairs from CGI input (usually HTML forms) sent via an
  //!  HTTP POST or GET operation.  The constructor does the work of
  //!  getting the data, hence you should likely only have a single Cgi
  //!  object in a program.  I haven't made this class a true singleton,
  //!  but may do so in the future.
  //!
  //!  Under the hood... the implementation uses a trivial flex lexer to
  //!  get CGI data from the environment (in the case of a GET operation)
  //!  or stdin (in the case of a POST operation).  It also unescapes any
  //!  escape codes in the input values.
  //--------------------------------------------------------------------------
  class Cgi
  {
  public:
    typedef std::map<std::string,std::string>  valueMap_t;
    typedef valueMap_t::iterator               iterator;
    typedef valueMap_t::const_iterator         const_iterator;
    
    //------------------------------------------------------------------------
    //!  Constructor.
    //!  Retrieves data from CGI and stores name/value pairs (as strings) in
    //!  a map<string,string,less<string> >.  The map is accessible via
    //!  the QueryStrings() method.
    //------------------------------------------------------------------------
    Cgi();
    
    //------------------------------------------------------------------------
    //!  destructor
    //------------------------------------------------------------------------
    ~Cgi();

    //------------------------------------------------------------------------
    //!  Returns the HTTP request method.
    //------------------------------------------------------------------------
    const std::string & RequestMethod() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the HTTP request method.
    //------------------------------------------------------------------------
    const std::string & RequestMethod(const std::string & requestMethod);
    
    //------------------------------------------------------------------------
    //!  Returns a reference to the query values sent by the client.  These
    //!  are the name/value pairs from CGI, typically from HTML form fields.
    //------------------------------------------------------------------------
    valueMap_t & QueryValues();

    valueMap_t & EnvValues();

    //------------------------------------------------------------------------
    //!  Encodes a URL string \c src into a string \c dst, with special
    //!  characters escaped.
    //------------------------------------------------------------------------
    static void UrlEncode(const std::string & src, std::string & dst);

    //------------------------------------------------------------------------
    //!  Returns a URL-escaped version of \c src.
    //------------------------------------------------------------------------
    static std::string UrlEncode(const std::string & src);

    //------------------------------------------------------------------------
    //!  Encodes a string \c src into an HTML string \c dst, with special
    //!  characters escaped.
    //------------------------------------------------------------------------
    static void HtmlEncode(const std::string & src, std::string & dst);

    //------------------------------------------------------------------------
    //!  Returns an HTML-escaped version of \c src.
    //------------------------------------------------------------------------
    static std::string HtmlEncode(const std::string & src);
    
  private:
    std::string   _requestMethod;
    valueMap_t    _queryValues;
    valueMap_t    _envValues;
    
    static std::map<char,std::string>  _urlEncodingMap;
    static void LoadUrlEncodingMap();
    static std::map<char,std::string>  _htmlEncodingMap;
    static void LoadHtmlEncodingMap();
  };


}  // namespace Dwm
  
#endif  // _CGI_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
