%{
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
