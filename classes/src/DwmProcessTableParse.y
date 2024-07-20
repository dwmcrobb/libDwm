%{
  //=========================================================================
  // @(#) $DwmPath$
  //=========================================================================
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
  //=========================================================================

  //-------------------------------------------------------------------------
  //!  \file DwmProcessTableParse.y
  //!  \author Daniel W. McRobb
  //!  \brief parser for 'ps' output
  //-------------------------------------------------------------------------
%}

%code requires
{
  #include <cstdint>
  #include <string>

  #include "DwmBootTime.hh"
  #include "DwmProcessTable.hh"
    
  extern "C" {
    extern int psparselex_destroy(void);
  }
}

%{
  #include <cstdio>
  #include <iostream>
  #include <mutex>
  #include <string>
  #include <vector>
  #include "DwmProcessTable.hh"
  #include "DwmProcessTree.hh"

  extern "C" {
    #include <netdb.h>

    extern void psparseerror(const char *arg, ...);
    extern FILE *psparsein;
  }
        
  Dwm::ProcessTable  *g_pt = 0;
%}

%define api.prefix {psparse}

%union {
  time_t             timeVal;
  uint64_t           uint64Val;
  float              floatVal;
  std::string       *stringVal;
  Dwm::ProcessInfo  *procVal;
}

%code provides
{
  // Tell Flex the expected prototype of yylex.
  #define YY_DECL                             \
    int psparselex ()

  // Declare the scanner.
  YY_DECL;
}

%token<timeVal>    STARTTIME
%token<stringVal>  USERVAL RUSERVAL STRING
%token<uint64Val>  PIDVAL PPIDVAL GIDVAL UIDVAL RUIDVAL SVUIDVAL RGIDVAL
%token<uint64Val>  SVGIDVAL RSSVAL VSZVAL
%token<floatVal>   PCPUVAL PMEMVAL

%type<stringVal> ProcessCommand
%type<procVal> Process

%%

//---------------------------------------------------------------------------
//!  
//---------------------------------------------------------------------------
Processes: Process
{
  (*g_pt)[$1->pid] = *($1);
  delete $1;
}
| Processes Process
{
  (*g_pt)[$2->pid] = *($2);
  delete $2;
};

//---------------------------------------------------------------------------
//!  
//---------------------------------------------------------------------------
Process: USERVAL RUSERVAL PIDVAL PPIDVAL GIDVAL UIDVAL RUIDVAL SVUIDVAL RGIDVAL SVGIDVAL RSSVAL VSZVAL PCPUVAL PMEMVAL STARTTIME STRING ProcessCommand
{
  $$ = new Dwm::ProcessInfo();
  $$->user   = *($1);
  $$->ruser  = *($2);
  $$->pid    = $3;
  $$->ppid   = $4;
  $$->gid    = $5;
  $$->uid    = $6;
  $$->ruid   = $7;
  $$->svuid  = $8;
  $$->rgid   = $9;
  $$->svgid  = $10;
  $$->rss    = $11 * 1024;
  $$->vsz    = $12;
  $$->pcpu   = $13;
  $$->pmem   = $14;
  $$->lstart = $15;
  $$->tty    = *($16);
  $$->args   = *($17);

  delete $1;
  delete $2;
  delete $16;
  delete $17;
};

//---------------------------------------------------------------------------
//!  
//---------------------------------------------------------------------------
ProcessCommand: STRING
{
  $$ = new std::string(*$1);
  delete $1;
}
| ProcessCommand STRING
{
  *($$) += ' ';
  *($$) += *$2;
  delete $2;
};

%%

namespace Dwm {

  std::mutex     g_mtx;

  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  static int64_t AncestorLstart(const ProcessTable & pt, pid_t ppid)
  {
    auto  ppit = pt.find(ppid);
    if (ppit != pt.end()) {
      if (ppit->second.lstart) {
        return ppit->second.lstart;
      }
      else {
        return AncestorLstart(pt, ppit->second.ppid);
      }
    }
    return BootTime();
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool GetProcessTable(ProcessTable & pt)
  {
    std::string  pscmd("ps -o 'user,ruser,pid,ppid,gid,uid,ruid,svuid,rgid,svgid,rss,vsz,pcpu,pmem,lstart,tty,command' -A");
    pt.clear();
    std::lock_guard<std::mutex>  lck(g_mtx);
    g_pt = &pt;
    psparsein = popen(pscmd.c_str(), "r");
    if (psparsein) {
      psparseparse();
      pclose(psparsein);
      psparselex_destroy();
    }
#if (defined(__FreeBSD__))
    //  lstart isn't populated for some processes in 'ps' output.
    //  For those processes, use an ancestor's lstart.
    for (auto & p : pt) {
      if (0 == p.second.lstart) {
        p.second.lstart = AncestorLstart(pt, p.second.ppid);
      }
    }
#endif
    return (! pt.empty());
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool GetProcessInfo(pid_t pid, ProcessInfo & processInfo)
  {
    bool  rc = false;
    ProcessTable  pt;
    std::string  pscmd("ps -o 'user,ruser,pid,ppid,gid,uid,ruid,svuid,rgid,svgid,rss,vsz,pcpu,pmem,lstart,tty,command' -p ");
    pscmd += std::to_string(pid);
    std::lock_guard<std::mutex>  lck(g_mtx);
    g_pt = &pt;
    psparsein = popen(pscmd.c_str(), "r");
    if (psparsein) {
      psparseparse();
      pclose(psparsein);
      psparselex_destroy();
    }
    if (1 == pt.size()) {
      processInfo = pt.begin()->second;
      rc = true;
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool ToJson(const ProcessTable & processTable, nlohmann::json & j)
  {
    j = nlohmann::json::array();
    ssize_t i = 0;
    for (const auto & p : processTable) {
        j[i] = p.second.ToJson();
      ++i;
    }
    return (! j.empty());
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  nlohmann::json ToJson(const ProcessTable & processTable)
  {
    nlohmann::json j = nlohmann::json::array();
    ssize_t i = 0;
    for (const auto & p : processTable) {
      j[i] = p.second.ToJson();
      ++i;
    }
    return j;
  }

}  // namespace Dwm

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
std::ostream & operator << (std::ostream & os, const Dwm::ProcessTable & pt)
{
  for (const auto & pi : pt) {
    os << pi.second << '\n';
  }
  return os;
}

