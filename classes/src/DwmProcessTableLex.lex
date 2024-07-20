%{
  //===========================================================================
  // @(#) $DwmPath$
  //===========================================================================
  //  Copyright (c) Daniel W. McRobb 2024
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
  //!  \file proclex.lex
  //!  \author Daniel W. McRobb
  //!  \brief lexer for 'ps' output
  //---------------------------------------------------------------------------

  #include <cstring>
  #include <ctime>
  #include <unordered_map>
  
  #include "DwmProcessTableParse.hh"
  
  extern "C" {
    #include <stdarg.h>

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void psparseerror(const char *arg, ...)
    {
      va_list  ap;
      va_start(ap, arg);
      char  *s = nullptr;
      if (vasprintf(&s, arg, ap) != -1) {
        //        Syslog(LOG_ERR, "%s: '%s' at line %d of %s\n",
        //               s, yytext, yylineno, g_configPath.c_str());
      }
      return;
    }
  }

  static inline const std::unordered_map<int,std::pair<int,int>>::const_iterator
    GetNextNumericState(int curState);
%}

m_wkdy (Sun|Mon|Tue|Wed|Thu|Fri|Sat)
m_mon  (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dev)
m_mday ([1-9]|1[0-9]|2[0-9]|3[01])
m_tod  ([01][0-9]|2[0-3])[:]([0-5][0-9])[:]([0-5][0-9])

%option noyywrap
%option prefix="psparse"

%x x_user
%x x_ruser
%x x_pid
%x x_ppid
%x x_gid
%x x_uid
%x x_ruid
%x x_svuid
%x x_rgid
%x x_svgid
%x x_rss
%x x_vsz
%x x_pcpu
%x x_pmem
%x x_startTime
%x x_tty
%x x_command

%%

<INITIAL>^[ ]*USER.*\n {
  BEGIN(x_user);
} 

<x_user,x_ruser>[^ \t]+  {
  psparselval.stringVal = new std::string(yytext);
  auto  it = GetNextNumericState(YYSTATE);
  BEGIN(it->second.first);
  return it->second.second;
}
<x_user,x_ruser>[ \t]
  
%{//------------------------------------------------------------------------
  //  All of the first 10 fields in our 'ps ...' output are numeric, but I
  //  want different tokens for each one, and a different state for each one.
  //  To avoid 10 duplicate rules, I use the GetNextNumericState() helper
  //  function.
  //------------------------------------------------------------------------%}
<x_pid,x_ppid,x_gid,x_uid,x_ruid,x_svuid,x_rgid,x_svgid,x_rss,x_vsz>[0-9]+  {
  psparselval.uint64Val = strtoull(yytext,nullptr,10);
  auto  it = GetNextNumericState(YYSTATE);
  BEGIN(it->second.first);
  return it->second.second;
}

%{//------------------------------------------------------------------------
  //  Skip whitespace in the states for numeric fields.
  //------------------------------------------------------------------------%}
<x_pid,x_ppid,x_gid,x_uid,x_ruid,x_svuid,x_rgid,x_svgid,x_rss,x_vsz>[ \t]+

<x_pcpu,x_pmem>[0-9\.]+  {
  psparselval.floatVal = strtof(yytext,nullptr);
  auto  it = GetNextNumericState(YYSTATE);
  BEGIN(it->second.first);
  return it->second.second;
}
<x_pcpu,x_pmem>[ \t]+
  
%{//------------------------------------------------------------------------
  //  Match the 'lstart' field, which is time in ctime() format.
  //------------------------------------------------------------------------%}
<x_startTime>{m_wkdy}[ ]{m_mon}[ ]+{m_mday}[ ]{m_tod}[ ][0-9]{4,4} {
  struct tm  tm;
  memset(&tm, 0, sizeof(tm));
  if (strptime(yytext, "%c", &tm)) {
    psparselval.timeVal = mktime(&tm);
    BEGIN(x_tty);
    return STARTTIME;
  }
}

<x_startTime>\-  {
  psparselval.timeVal = 0;
  BEGIN(x_tty);
  return STARTTIME;
}

%{//------------------------------------------------------------------------
  //  Match the 'tty' field, which is just a string.
  //------------------------------------------------------------------------%}
<x_tty>[^ \t\n]+ {
  psparselval.stringVal = new std::string(yytext);
  BEGIN(x_command);
  return STRING;
}

%{//------------------------------------------------------------------------
  //  Skip whitespace in x_startTime and x_tty states.
  //------------------------------------------------------------------------%}
<x_startTime,x_tty>[ \t]+

%{//------------------------------------------------------------------------
  //  Match the 'args' field, which is just command and argument strings.
  //------------------------------------------------------------------------%}
<x_command>[^ \t\n]+ {
  psparselval.stringVal = new std::string(yytext);
  return STRING;
}

%{//------------------------------------------------------------------------
  //  A newline means we're done parsing a process, go back to x_pid state
  //  to start parsing the next process.
  //------------------------------------------------------------------------%}
<x_command>[\n] {
  BEGIN(x_user);
}

%{//------------------------------------------------------------------------
  //  Skip spaces and tabs while processing command and args.
  //------------------------------------------------------------------------%}
<x_command>[ \t]+

<<EOF>>  { BEGIN(INITIAL); return 0; }

%%

//----------------------------------------------------------------------------
//!  This is just a helper to avoid a bunch of duplication in the lexer
//!  rules.
//----------------------------------------------------------------------------
static inline const std::unordered_map<int,std::pair<int,int>>::const_iterator
GetNextNumericState(int curState)
{
  static const std::unordered_map<int,std::pair<int,int>>  nextStates = {
    { x_user,  { x_ruser,     USERVAL  } },
    { x_ruser, { x_pid,       RUSERVAL } },
    { x_pid,   { x_ppid,      PIDVAL   } },
    { x_ppid,  { x_gid,       PPIDVAL  } },
    { x_gid,   { x_uid,       GIDVAL   } },
    { x_uid,   { x_ruid,      UIDVAL   } },
    { x_ruid,  { x_svuid,     RUIDVAL  } },
    { x_svuid, { x_rgid,      SVUIDVAL } },
    { x_rgid,  { x_svgid,     RGIDVAL  } },
    { x_svgid, { x_rss,       SVGIDVAL } },
    { x_rss,   { x_vsz,       RSSVAL   } },
    { x_vsz,   { x_pcpu,      VSZVAL   } },
    { x_pcpu,  { x_pmem,      PCPUVAL  } },
    { x_pmem,  { x_startTime, PMEMVAL  } }
  };
  return nextStates.find(curState);
}

 
