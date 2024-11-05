%{
  //===========================================================================
  // @(#) $DwmPath$
  //===========================================================================
  //  Copyright (c) Daniel W. McRobb 1999, 2000, 2001
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

  extern "C" {
    #include <sys/types.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
  }

  #include "DwmCgi.hh"

  using namespace std;
%}

%x EQUAL
%x KEY
%x VALUE

%option noyywrap
%option nounput
%option prefix="querystring"

%%

<INITIAL>[^=]+           { BEGIN(EQUAL); return(KEY); }
<INITIAL>[=]
<EQUAL>[=]               { BEGIN(VALUE); }
<VALUE>[^&]+             { return(VALUE); }
<VALUE>[&]               { BEGIN(INITIAL); }

%%

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void FixEscapes(string & value)
{
  size_t  percentPosition = 0;
  string  validEscapeChars = string("0123456789ABCDEFabcdef");
  string  escapeValueString;
  char    escapeValue;

  while ((percentPosition = value.find("%",percentPosition)) != value.npos) {
    if ((validEscapeChars.find(value[percentPosition+1]) !=
         validEscapeChars.npos) &&
        (validEscapeChars.find(value[percentPosition+2]) !=
         validEscapeChars.npos)) {
      //  It's an escaped code starting at percentPosition.
      //  I need to replace it with the real value.
      escapeValueString = value.substr(percentPosition+1,2);
      escapeValue = (char)strtoul(escapeValueString.c_str(),(char **)NULL,16);
      escapeValueString = string("") + escapeValue;
      value.replace(percentPosition,3,escapeValueString,0,1);
    }
    percentPosition++;
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static int GetQueryString(Dwm::Cgi & cgi)
{
  int                  rc;
  string               key;
  string               value;
  YY_BUFFER_STATE      bufferState = (YY_BUFFER_STATE)0;
  int                  contentLength;
  
  if (cgi.RequestMethod() == string("POST")) {
    contentLength = atoi(cgi.EnvValues()["CONTENT_LENGTH"].c_str());
    yyin = stdin;
  }
  else {
    if (cgi.RequestMethod() == string("GET")) {
      cgi.EnvValues()[string("QUERY_STRING")] = getenv("QUERY_STRING");
      bufferState = yy_scan_string(cgi.EnvValues()["QUERY_STRING"].c_str());
    }
    else {
      return(-1);
    }
  }
  
  while ((rc = querystringlex()) != 0) {
    switch (rc) {
      case KEY:
        key = string(yytext);
        break;
      case VALUE:
        value = string(yytext);
        FixEscapes(value);
        (cgi.QueryValues())[key] = value;
        break;
      default:
        break;
    }
  }
  if (cgi.RequestMethod() == string("GET"))
    yy_delete_buffer(bufferState);

  return(0);
}

namespace Dwm {

  std::map<char,std::string>  Cgi::_urlEncodingMap;
  std::map<char,std::string>  Cgi::_htmlEncodingMap;
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Cgi::Cgi()
      : _requestMethod(), _queryValues(), _envValues()
  {
    LoadUrlEncodingMap();
    LoadHtmlEncodingMap();
    
    char  *env = getenv("REQUEST_METHOD");

    if (env)
      (this->EnvValues())[string("REQUEST_METHOD")] = env;
    
    if ((env = getenv("AUTH_TYPE")))
      (this->EnvValues())[string("AUTH_TYPE")] = env;
    if ((env = getenv("REMOTE_USER")))
      (this->EnvValues())[string("REMOTE_USER")] = env;
    if ((env = getenv("REMOTE_ADDR")))
      (this->EnvValues())[string("REMOTE_ADDR")] = env;
    if ((env = getenv("REMOTE_HOST")))
      (this->EnvValues())[string("REMOTE_HOST")] = env;
    if ((env = getenv("SERVER_PROTOCOL")))
      (this->EnvValues())[string("SERVER_PROTOCOL")] = env;
    if ((env = getenv("SERVER_PORT")))
      (this->EnvValues())[string("SERVER_PORT")] = env;
    if ((env = getenv("CONTENT_TYPE")))
      (this->EnvValues())[string("CONTENT_TYPE")] = env;
    if ((env = getenv("CONTENT_LENGTH")))
      (this->EnvValues())[string("CONTENT_LENGTH")] = env;
    if ((env = getenv("HTTP_ACCEPT")))
      (this->EnvValues())[string("HTTP_ACCEPT")] = env;
    if ((env = getenv("HTTP_COOKIE")))
      (this->EnvValues())[string("HTTP_COOKIE")] = env;
    
    GetQueryString(*this);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Cgi::~Cgi()
  { }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const string & Cgi::RequestMethod() const
  {
    static const string empty("");
    
    valueMap_t::const_iterator iter = this->_envValues.find("REQUEST_METHOD");
    if (iter != this->_envValues.end())
      return(iter->second);
    return(empty);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Cgi::valueMap_t & Cgi::QueryValues()
  {
    return(this->_queryValues);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Cgi::valueMap_t & Cgi::EnvValues()
  {
    return(this->_envValues);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Cgi::UrlEncode(const std::string & src, std::string & dst)
  {
    if (! dst.empty())
      dst.clear();
    string::size_type  i = 0;
    for (i = 0; i < src.length(); ++i) {
      std::map<char,std::string>::const_iterator  iter = 
        _urlEncodingMap.find(src[i]);
      if (iter != _urlEncodingMap.end())
        dst += iter->second;
      else
        dst += src[i];
    }
    return;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string Cgi::UrlEncode(const std::string & src)
  {
    std::string  rc;
    UrlEncode(src, rc);
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Cgi::HtmlEncode(const std::string & src, std::string & dst)
  {
    if (! dst.empty())
      dst.clear();
    string::size_type  i = 0;
    for (i = 0; i < src.length(); ++i) {
      std::map<char,std::string>::const_iterator  iter = 
        _htmlEncodingMap.find(src[i]);
      if (iter != _htmlEncodingMap.end())
        dst += iter->second;
      else
        dst += src[i];
    }
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string Cgi::HtmlEncode(const std::string & src)
  {
    std::string  rc;
    HtmlEncode(src, rc);
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Cgi::LoadUrlEncodingMap()
  {
    if (_urlEncodingMap.empty()) {
      _urlEncodingMap[' ']  = "%20";
      _urlEncodingMap['!']  = "%21";
      _urlEncodingMap['"']  = "%22";
      _urlEncodingMap['#']  = "%23";
      _urlEncodingMap['$']  = "%24";
      _urlEncodingMap['%']  = "%25";
      _urlEncodingMap['&']  = "%26";
      _urlEncodingMap['\''] = "%27";
      _urlEncodingMap['(']  = "%28";
      _urlEncodingMap[')']  = "%29";
      _urlEncodingMap['*']  = "%2A";
      _urlEncodingMap['+']  = "%2B";
      _urlEncodingMap[',']  = "%2C";
      _urlEncodingMap['-']  = "%2D";
      _urlEncodingMap['.']  = "%2E";
      _urlEncodingMap['/']  = "%2F";
      _urlEncodingMap[':']  = "%3A";
      _urlEncodingMap[';']  = "%3B";
      _urlEncodingMap['<']  = "%3C";
      _urlEncodingMap['=']  = "%3D";
      _urlEncodingMap['>']  = "%3E";
      _urlEncodingMap['?']  = "%3F";
      _urlEncodingMap['@']  = "%40";
      _urlEncodingMap['[']  = "%5B";
      _urlEncodingMap['\\'] = "%5C";
      _urlEncodingMap[']']  = "%5D";
      _urlEncodingMap['^']  = "%5E";
      _urlEncodingMap['_']  = "%5F";
      _urlEncodingMap['`']  = "%60";
    }
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Cgi::LoadHtmlEncodingMap()
  {
    if (_htmlEncodingMap.empty()) {
      _htmlEncodingMap['<']  = "&#060;";
      _htmlEncodingMap['<']  = "&#062;";
      _htmlEncodingMap['@']  = "&#064;";
      _htmlEncodingMap['[']  = "&#091;";
      _htmlEncodingMap[']']  = "&#093;";
      _htmlEncodingMap['`']  = "&#096;";
      _htmlEncodingMap['{']  = "&#123;";
      _htmlEncodingMap['|']  = "&#124;";
      _htmlEncodingMap['{']  = "&#125;";
      _htmlEncodingMap['~']  = "&#126;";
      _htmlEncodingMap['`']  = "&#145;";
      _htmlEncodingMap['\''] = "&#146;";
      _htmlEncodingMap['"']  = "&#148;";
    }
    return;
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
