%code requires
{
  #include "DwmEtcServices.hh"
    
  extern FILE *etcservicesin;
  
  extern "C" {
    extern void etcserviceserror(const char *arg, ...);
  }

}

%define api.prefix {etcservices}

%union {
    std::string                           *stringVal;
    uint16_t                               uint16Val;
    std::vector<std::string>              *stringVecVal;
    Dwm::EtcServices::Entry               *serviceEntryVal;
    std::vector<Dwm::EtcServices::Entry>  *serviceEntryVecVal;
}

%code provides
{
  // Tell Flex the expected prototype of yylex.
  #define YY_DECL                             \
    int etcserviceslex()

  // Declare the scanner.
  YY_DECL;
}

%{
  //--------------------------------------------------------------------------
  #include <mutex>
    
  static std::vector<Dwm::EtcServices::Entry>  *g_entries = nullptr;
%}

%token<stringVal>  NAME PROTOCOL ALIAS
%token<uint16Val>  PORTNUM

%type<serviceEntryVal>   Entry
%type<stringVecVal>      Aliases

%%

Entries: Entry
{
  g_entries->push_back(*($1));
  delete $1;
}
| Entries Entry
{
  g_entries->push_back(*($2));
  delete $2;
};

Entry: NAME PORTNUM '/' PROTOCOL Aliases
{
  $$ = new Dwm::EtcServices::Entry(*($1), *($4), $2, *($5));
  delete $1;
  delete $4;
  delete $5;
}
| NAME PORTNUM '/' PROTOCOL
{
  $$ = new Dwm::EtcServices::Entry(*($1), *($4), $2, {});
  delete $1;
  delete $4;
}
;

Aliases: ALIAS
{
  $$ = new std::vector<std::string>();
  $$->push_back(*($1));
  delete $1;
}
| Aliases ALIAS
{
  $$->push_back(*($2));
  delete $2;
};

%%

namespace Dwm {

  static std::mutex  g_entriesMtx;
  
  //--------------------------------------------------------------------------
  EtcServices::EtcServices(const std::string & path)
  {
    std::lock_guard  lck(g_entriesMtx);
    etcservicesin = fopen(path.c_str(), "r");
    if (etcservicesin) {
      g_entries = &_entries;
      etcservicesparse();
      for (const auto & entry : _entries) {
        _byname[entry.Name()] = &entry;
        for (const auto & alias : entry.Aliases()) {
          _byname[alias] = &entry;
        }
        _bypp[std::to_string(entry.Port()) + '/' + entry.Protocol()] = &entry;
      }
      fclose(etcservicesin);
    }
  }
  
  //--------------------------------------------------------------------------
  std::optional<EtcServices::Entry>
  EtcServices::GetEntry(const std::string & name) const
  {
    auto  it = _byname.find(name);
    if (it != _byname.end()) {
      return *(it->second);
    }
    return {};
  }

#if 0
  //-------------------------------------------------------------------------
  std::optional<EtcServices::Entry>
  EtcServices::GetEntry(uint16_t port, const std::string & proto)
  {
    std::string  k = std::to_string(port) + '/' + proto;
    auto  it = _bypp.find(k);
    if (it != _bypp.end()) {
      return *(it->second);
    }
    return {};
  }
#endif
  
}  // namespace Dwm
