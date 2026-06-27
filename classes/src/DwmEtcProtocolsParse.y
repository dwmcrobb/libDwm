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
  //!  @file DwmEtcProtocolsParse.y
  //!  @author Daniel W. McRobb
  //!  @brief /etc/protocols parser
  //---------------------------------------------------------------------------
%}

%code requires
{
  #include "DwmEtcProtocols.hh"
    
  extern FILE *etcprotosin;
  
  extern "C" {
    // extern int etcprotoslex_destroy(void);
    extern void etcprotoserror(const char *arg, ...);
  }

}

%define api.prefix {etcprotos}

%union {
    std::string                            *stringVal;
    int                                     intVal;
    std::vector<std::string>               *stringVecVal;
    Dwm::EtcProtocols::Entry               *protoEntryVal;
    std::vector<Dwm::EtcProtocols::Entry>  *protoEntryVecVal;
}

%code provides
{
  // Tell Flex the expected prototype of yylex.
  #define YY_DECL                             \
    int etcprotoslex()

  // Declare the scanner.
  YY_DECL;
}

%{
  //--------------------------------------------------------------------------
  static std::vector<Dwm::EtcProtocols::Entry>  *g_entries = nullptr;
%}

%token<stringVal>  PROTONAME PROTOALIAS
%token<intVal>     PROTONUM

%type<protoEntryVal>     ProtocolEntry
%type<intVal>            ProtocolNumber
%type<stringVal>         ProtocolName ProtocolAlias
%type<stringVecVal>      ProtocolAliases
/* %type<protoEntryVecVal>  ProtocolEntries */

%%

ProtocolEntries: ProtocolEntry
{
  g_entries->push_back(*($1));
  delete $1;
}
| ProtocolEntries ProtocolEntry
{
  g_entries->push_back(*($2));
  delete $2;
};

ProtocolEntry: ProtocolName ProtocolNumber ProtocolAliases
{
  $$ = new Dwm::EtcProtocols::Entry(*($1), $2, *($3));
  delete $1;
  delete $3;
}
| ProtocolName ProtocolNumber
{
  $$ = new Dwm::EtcProtocols::Entry(*($1), $2);
  delete $1;
};

ProtocolName: PROTONAME
{
  $$ = $1;
};

ProtocolNumber: PROTONUM
{
  $$ = $1;
};

ProtocolAliases: ProtocolAlias
{
  $$ = new std::vector<std::string>();
  $$->push_back(*($1));
  delete $1;
}
| ProtocolAliases ProtocolAlias
{
  $$->push_back(*($2));
  delete $2;
};

ProtocolAlias: PROTOALIAS
{
  $$ = $1;
};

%%

namespace Dwm {

  //--------------------------------------------------------------------------
  EtcProtocols::EtcProtocols(const std::string & path)
  {
    etcprotosin = fopen(path.c_str(), "r");
    if (etcprotosin) {
      std::lock_guard  lck(_mtx);
      _entries.clear();
      _byname.clear();
      _bynum.clear();
      g_entries = &_entries;
      etcprotosparse();
      for (const auto & entry : _entries) {
        _byname[entry.Name()] = &entry;
        for (const auto & alias : entry.Aliases()) {
          _byname[alias] = &entry;
        }
        _bynum[entry.Number()] = &entry;
      }
      fclose(etcprotosin);
    }
  }

  //--------------------------------------------------------------------------
  std::optional<EtcProtocols::Entry>
  EtcProtocols::GetEntry(const std::string & name) const
  {
    auto  it = _byname.find(name);
    if (it != _byname.end()) {
      return *(it->second);
    }
    return {};
  }
  
  //--------------------------------------------------------------------------
  std::optional<EtcProtocols::Entry> EtcProtocols::GetEntry(int num) const
  {
    auto  it = _bynum.find(num);
    if (it != _bynum.end()) {
      return *(it->second);
    }
    return {};
  }
  
  //--------------------------------------------------------------------------
  void EtcProtocols::AddEntry(const EtcProtocols::Entry & entry)
  {
      // std::lock_guard  lck(_mtx);
    _entries.push_back(entry);
    return;
  }

}  // namespace Dwm

