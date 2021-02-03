#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>

#include "DwmIpv4Prefix.hh"
#include "DwmTimeValue64.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

template <typename T, uint8_t Level>
class PrefixLevel;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
class PrefixLevel<T, 0>
{
public:
  void Add(const Ipv4Prefix & pfx, T & val)
  {
    uint8_t  pfxBitsRemaining = pfx.MaskLength() - 24;
    _myEntries[pfxBitsRemaining][pfx.NetworkRaw()] = val;
  }

  bool Find(const Ipv4Prefix & pfx, T & val) const
  {
    uint8_t  pfxBitsRemaining = pfx.MaskLength() - 24;
    auto  myit = _myEntries.find(pfxBitsRemaining);
    if (myit != _myEntries.end()) {
      auto  eit = myit->second.find(pfx.NetworkRaw());
      if (eit != myit->second.end()) {
        val = eit->second;
        return true;
      }
    }
    return false;
  }


private:
  std::map<uint8_t,std::unordered_map<ipv4addr_t,T>>  _myEntries;
};

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T, uint8_t Level>
class PrefixLevel
{
public:
  void Add(const Ipv4Prefix & pfx, T & val)
  {
    uint8_t  pfxBitsRemaining = pfx.MaskLength() - ((Level + 1) * 8);
    if (pfxBitsRemaining < 9) {
      _myEntries[pfxBitsRemaining][pfx.NetworkRaw()] = val;
    }
    else {
      _nextLevel[(ntohl(pfx.NetworkRaw()) >> (Level * 8)) & 0xFF].Add(pfx, val);
    }
  }

  bool Find(const Ipv4Prefix & pfx, T & val) const
  {
#if 1
    uint8_t  pfxBitsRemaining = pfx.MaskLength() - ((Level + 1) * 8);
    if (pfxBitsRemaining < 9) {
      auto  myit = _myEntries.find(pfxBitsRemaining);
      if (myit != _myEntries.end()) {
        auto  eit = myit->second.find(pfx.NetworkRaw());
        if (eit != myit->second.end()) {
          val = eit->second;
          return true;
        }
      }
    }
    else {
      uint8_t  octet = (ntohl(pfx.NetworkRaw()) >> (Level * 8)) & 0xFF;
      auto  it = _nextLevel.find(octet);
      if (it != _nextLevel.end()) {
        return it->second.Find(pfx, val);
      }
    }
#else    
    uint8_t  octet = (ntohl(pfx.NetworkRaw()) >> (Level * 8)) & 0xFF;
    auto  it = _nextLevel.find(octet);
    if (it != _nextLevel.end()) {
      return it->second.Find(pfx, val);
    }
    else {
      uint8_t  pfxBitsRemaining = pfx.MaskLength() - ((Level + 1) * 8);
      if (pfxBitsRemaining < 9) {
        auto  myit = _myEntries.find(pfxBitsRemaining);
        if (myit != _myEntries.end()) {
          auto  eit = myit->second.find(pfx.NetworkRaw());
          if (eit != myit->second.end()) {
            val = eit->second;
            return true;
          }
        }
      }
    }
#endif
    return false;
  }


private:
  std::map<uint8_t,std::unordered_map<ipv4addr_t,T>>  _myEntries;
  std::map<uint8_t,PrefixLevel<T,Level-1>>            _nextLevel;
};

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
static void TestFindPerformance(const std::vector<Ipv4Prefix> & entries,
                                const PrefixLevel<uint32_t,0> & pfxTree)
{
  uint32_t  val;
  uint32_t  numFound = 0;
  TimeValue64  startTime(true);
  for (const auto & entry : entries) {
    numFound += pfxTree.Find(entry, val);
  }
  TimeValue64  endTime(true);
  endTime -= startTime;
  uint64_t  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
  uint64_t  findsPerSec = (numFound * 1000000ULL) / usecs;
  cout << numFound << " prefixes, " << findsPerSec
       << " prefix finds/sec\n";
  UnitAssert(numFound == entries.size());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  std::vector<Ipv4Prefix>  entries;
  uint32_t  val = 0;
  if (UnitAssert(GetEntries(entries))) {
    PrefixLevel<uint32_t,0>  pfxTree;
    for (const auto & entry : entries) {
      pfxTree.Add(entry, val);
      ++val;
    }

    uint32_t  expectedVal = 0;
    for (const auto & entry : entries) {
      UnitAssert(pfxTree.Find(entry, val));
      UnitAssert(val == expectedVal);
      ++expectedVal;
    }

    TestFindPerformance(entries, pfxTree);
    
    if (Assertions::Total().Failed()) {
      Assertions::Print(cerr, true);
      return 1;
    }
    else {
      cout << Assertions::Total() << " passed" << endl;
    }
  }
  return 0;
}
