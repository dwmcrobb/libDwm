#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

#include "DwmIpv4Prefix.hh"
#include "DwmTimeValue64.hh"
#include "DwmUnitAssert.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Map from lower 16 bits of network to a value.
  //--------------------------------------------------------------------------
  template <typename T>
  class Ipv4LowerMap
  {
  public:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Add(const Ipv4Prefix & pfx, const T & val)
    {
      ipv4addr_t  a = pfx.NetworkRaw();
      _entries[ntohl(a) & 0xFFFF] = val;
      return true;
    }
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Find(const Ipv4Prefix & pfx, T & val) const
    {
      ipv4addr_t  a = pfx.NetworkRaw();
      auto  it = _entries.find(ntohl(a) & 0xFFFF);
      if (it != _entries.end()) {
        val = it->second;
        return true;
      }
      return false;
    }

  private:
    std::unordered_map<uint16_t,T>  _entries;
  };

  //--------------------------------------------------------------------------
  //!  For each 16 least significant bits of prefix length, hold an
  //!  Ipv4LowerMap.
  //--------------------------------------------------------------------------
  template <typename T>
  class Ipv4LowerTree
  {
  public:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Add(const Ipv4Prefix & pfx, const T & val)
    {
      return  _lowMap[pfx.MaskLength() & 0x0F].Add(pfx, val);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Find(const Ipv4Prefix & pfx, T & val) const
    {
      auto  lit = _lowMap.find(pfx.MaskLength() & 0x0F);
      if (lit != _lowMap.end()) {
        return lit->second.Find(pfx, val);
      }
      return false;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool FindLongest(const Ipv4Address & addr, T & val) const
    {
      for (auto it = _lowMap.rbegin(); it != _lowMap.rend(); ++it) {
        if (it->second.Find(Ipv4Prefix(addr, 16 + it->first), val)) {
          return true;
        }
      }
      return false;
    }
            
    std::map<uint8_t,Ipv4LowerMap<T>>  _lowMap;
  };

  //--------------------------------------------------------------------------
  //!  Map from 16 most significant bits of prefixes to Ipv4LowerTree
  //!  instances.
  //--------------------------------------------------------------------------
  template <typename T>
  class Ipv4UpperMap
  {
  public:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Add(const Ipv4Prefix & pfx, const T & val)
    {
      ipv4addr_t  a = pfx.NetworkRaw();
      if (pfx.MaskLength() < 17) {
        _values[ntohl(a) >> 16] = val;
        return true;
      }
      else {
        return _lowTree[ntohl(a) >> 16].Add(pfx, val);
      }
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Find(const Ipv4Prefix & pfx, T & val) const
    {
      ipv4addr_t  a = pfx.NetworkRaw();
      if (pfx.MaskLength() < 17) {
        auto  it = _values.find(ntohl(a) >> 16);
        if (it != _values.end()) {
          val = it->second;
          return true;
        }
      }
      else {
#if 1
        return _lowTree[ntohl(a) >> 16].Find(pfx, val);
#else
        auto  it = _lowTree.find(ntohl(a) >> 16);
        if (it != _lowTree.end()) {
          return it->second.Find(pfx, val);
        }
#endif
      }
      return false;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool FindLongest(const Ipv4Address & addr, T & val) const
    {
      if (_lowTree.FindLongest(addr, val)) {
        return true;
      }
      else {
        auto  it = _values.find(ntohl(addr.Raw()) >> 16);
        if (it != _values.end()) {
          val = it->second;
          return true;
        }
      }
      return false;
    }
    
      
  private:
    std::unordered_map<uint16_t,T>                 _values;
#if 1
    std::array<Ipv4LowerTree<T>,65536>  _lowTree;
#else
    std::unordered_map<uint16_t,Ipv4LowerTree<T>>  _lowTree;
#endif
  };

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T>
  class Ipv4PrefixTree
  {
  public:
    Ipv4PrefixTree()
        : _upperMap()
    {}
      
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Add(const Ipv4Prefix & pfx, const T & val)
    {
      return _upperMap[pfx.MaskLength() >> 4].Add(pfx, val);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Find(const Ipv4Prefix & pfx, T & val) const
    {
#if 1
      return _upperMap[pfx.MaskLength() >> 4].Find(pfx, val);
#else
      auto  it = _upperMap.find(pfx.MaskLength() >> 4);
      if (it != _upperMap.end()) {
        return it->second.Find(pfx, val);
      }
      return false;
#endif
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool FindLongest(const Ipv4Address & addr, T & val) const
    {
      for (int8_t i = 32; i >= 0; --i) {
        Ipv4Prefix  pfx(addr, i);
        if (_upperMap[pfx.MaskLength() >> 4].Find(pfx, val)) {
          return true;
        }
      }
      return false;
    }
    
  private:
#if 1
    std::array<Ipv4UpperMap<T>,17>  _upperMap;
#else
    std::map<uint8_t,Ipv4UpperMap<T>>  _upperMap;
#endif
  };

}  // namespace Dwm


using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool GetEntries(vector<Ipv4Prefix> & entries)
{
  entries.clear();
  ifstream is("./IPV4_prefixes.20210123");
  if (UnitAssert(is)) {
    char  buf[512];
    memset(buf, 0, 512);
    while (is.getline(buf, 512, '\n')) {
      Ipv4Prefix  pfx(buf);
      entries.push_back(pfx);
    }
    is.close();
  }
  return (! entries.empty());
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  std::vector<Ipv4Prefix>  entries;
  if (UnitAssert(GetEntries(entries))) {
    Ipv4PrefixTree<uint32_t>  pfxTree;
    uint32_t  val = 0;
    for (const auto & entry : entries) {
      UnitAssert(pfxTree.Add(entry, val));
      ++val;
    }

    uint64_t  found = 0;
    Dwm::TimeValue64  startTime(true);
    for (const auto & entry : entries) {
      found += pfxTree.Find(entry, val);
      // UnitAssert(val == found - 1);
    }
    Dwm::TimeValue64  endTime(true);
    endTime -= startTime;
    uint64_t  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
    uint64_t  findsPerSec = (found * 1000000ULL) / usecs;
    cout << found << " prefixes, " << findsPerSec
         << " prefix finds/sec\n";
    UnitAssert(found == entries.size());

    found = 0;
    startTime.SetNow();
    for (const auto & entry : entries) {
      found += pfxTree.FindLongest(entry.Network(), val);
    }
    endTime.SetNow();
    endTime -= startTime;
    usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
    findsPerSec = (found * 1000000ULL) / usecs;;
    cout << found << " prefixes, " << findsPerSec
         << " longest match finds/sec\n";
    UnitAssert(found == entries.size());
    
    if (Assertions::Total().Failed()) {
      Assertions::Print(cerr, true);
      return 1;
    }
    else {
      cout << Assertions::Total() << " passed" << endl;
    }
    sleep(10);
  }
  return 0;
}
