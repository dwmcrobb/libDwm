%{
  //===========================================================================
  // @(#) $DwmPath$
  //===========================================================================
  //  Copyright (c) Daniel W. McRobb 2005-2007
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

  #include <cassert>
  #include <iostream>
  #include <fstream>

  #include "DwmRegistry.hh"

  using namespace std;
%}

m_equal		[ \t]*[=][ \t]*

%x x_key
%x x_equal
%x x_value
%x x_quotedValue
%x x_error
%x x_valueEnd

%option noyywrap
%option nounput
%option yylineno
%option prefix="Registry"

%%

<INITIAL>^[#].*\n
<INITIAL>^[ \t]*\n
<INITIAL>[^ \t\n=]+/{m_equal}	{ BEGIN(x_equal); return(x_key); }
<INITIAL>.						{ BEGIN(x_error); yyless(0); }
<x_equal>{m_equal}["]			{ BEGIN(x_quotedValue); }
<x_equal>{m_equal}				{ BEGIN(x_value); }
<x_quotedValue>([^"]|[\\]["])*/["]	{ return(x_value); }
<x_quotedValue>["][ \t]*\n		{ BEGIN(INITIAL); return(x_valueEnd); }
<x_quotedValue>[\n]				{ BEGIN(INITIAL); }
<x_value>[^\n]*					{ return(x_value); }
<x_value>[\n]					{ BEGIN(INITIAL); }
<x_error>.						{ return(x_error); }
<<EOF>>							{ return(0); }
%%

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static void Unescape(string & s)
  {
    if (! s.empty()) {
      string::iterator  iter = s.begin();
      while (iter != s.end()) {
        if (*iter == '\\') {
          string::iterator  iter2 = iter;
          ++iter2;
          if (iter2 != s.end())
            if (*iter2 == '"')
              iter = s.erase(iter);
        }
        ++iter;
      }
    }
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static void Escape(string & s)
  {
    if (! s.empty()) {
      string  sc;
      string::iterator  iter = s.begin();
      while (iter != s.end()) {
        if (*iter == '"') {
          sc += "\\\"";
        }
        else
          sc += *iter;
        ++iter;
      }
      s = sc;
    }
    return;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Registry::Registry(const std::string & filename)
    : _filename(filename), _map()
  {
    Reload();
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool Registry::Reload()
  {
    bool  rc = false;
    if (! _map.empty())
      _map.clear();
    Registryin = fopen(_filename.c_str(),"r");
    if (Registryin) {
      string  key, value;
      while (int lexrc = Registrylex()) {
        if (lexrc == x_error) {
          std::cerr << "Parse error at " << _filename.c_str() << " line "
                    << yylineno << ": '" << yytext << "' unexpected" 
                    << endl;
          break;
        }
      
        switch (lexrc) {
          case  x_key:
            key = yytext;
            value = "";
            break;
          case x_value:
            value = yytext;
            Unescape(value);
            if (! key.empty())
              _map[key] = value;
            key = "";
            break;
		  case x_valueEnd:
			_map[key] = value;
			break;
          default:
            break;
        }

      }
      fclose(Registryin);
      rc = true;
    }
    return(rc);
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool Registry::Save() const
  {
    bool  rc = false;
    ofstream  os(_filename.c_str());
    if (os) {
      map<string,string>::const_iterator  iter;
      for (iter = _map.begin(); iter != _map.end(); ++iter) {
        os << iter->first << "\t";
        string  value = iter->second;
        Escape(value);
        os << "=\t\"" << value << "\"" << endl;
      }
      rc = true;
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  string Registry::Get(const string & key) const
  {
    string  rc("");
    
    map<string,string>::const_iterator  iter = _map.find(key);
    if (iter != _map.end())
      rc = iter->second;
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  const string & Registry::Set(const string & key, const string & value)
  {
    assert(! key.empty());
    _map[key] = value;
    return(value);
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
