%{
  //===========================================================================
  //  Copyright (c) Daniel W. McRobb 2026
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
  //!  @file DwmEtcProtocolsLex.lex
  //!  @author Daniel W. McRobb
  //!  @brief /etc/protocols lexer
  //---------------------------------------------------------------------------

  #include <iostream>
  
  #include "DwmEtcProtocolsParse.hh"

  extern "C" {
    #include <stdarg.h>

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void etcprotoserror(const char *arg, ...)
    {
      va_list  ap;
      va_start(ap, arg);
      char  *s = nullptr;
      if (vasprintf(&s, arg, ap) != -1) {
        std::cerr << s << '\n';
      }
      return;
    }
  }

%}

%option noyywrap
%option prefix="etcprotos"

%x x_number
%x x_aliases

%%

<INITIAL>[#].*\n
<INITIAL>[^ \t\n#]+  {
   BEGIN(x_number);
   etcprotoslval.stringVal = new std::string(yytext);
   return PROTONAME;
}
<INITIAL>[ \t\n]+

<x_number>[0-9]+  {
  BEGIN(x_aliases);
  etcprotoslval.intVal = atoi(yytext);
  return PROTONUM;
}
<x_number>[ \t]

<x_aliases>[#][^\n]*
<x_aliases>[^ \t\n#]+  {
  etcprotoslval.stringVal = new std::string(yytext);
  return PROTOALIAS;
}

<x_aliases>[ \t#]+
<x_aliases>\n  {
  BEGIN(INITIAL);
}

%%
