//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestPatricia.cc 8389 $
// @(#) $Id: TestPatricia.cc 8389 2016-04-17 04:31:36Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2016
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
//!  \file TestPatricia.cc
//!  \brief Unit test for Dwm::Patricia class template
//---------------------------------------------------------------------------

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
}

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

#include "DwmIpv4Address.hh"
#include "DwmIpv4Prefix.hh"
#include "DwmOptArgs.hh"
#include "DwmTimeValue.hh"
#include "DwmPatricia.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestPatricia.cc 8389 $");

using namespace std;
using namespace Dwm;

static bool g_showPerformance = false;

namespace Dwm {
	
	//--------------------------------------------------------------------------
	//!  
	//--------------------------------------------------------------------------
	template <>
	inline int GetBit<std::string>(const string & str, int bit)
	{
		if ((unsigned int)bit < str.length() * 8)
			return(((str[bit >> 3]) >> (7 - (bit & 0x7))) & 0x1);
		return(0);
	}
	
	//--------------------------------------------------------------------------
	//!  
	//--------------------------------------------------------------------------
	template <>
	inline int BitFirstDifferent<string>(const string & key1, const string & key2)
	{
		int n = 0;
		int d = 0;
		int maxn = (key1.length() > key2.length()) ? key2.length() : key1.length();
		while ((key1[n] == key2[n]) && (n < maxn))
			n++;
		while (GetBit(key1[n], d) == GetBit(key2[n], d))
			d++;
		return ((n << 3) + d);
	}

	//--------------------------------------------------------------------------
	//!  
	//--------------------------------------------------------------------------
	template <>
	inline int GetBit<Ipv4Prefix>(const Ipv4Prefix & pfx, int bit)
	{
		assert(bit <= 40);
		
		if (bit < 32)
			return(((pfx.Data()[bit >> 3]) >> (7 - (bit & 0x7))) & 0x1);
		
		if (bit < 40) {
			return(((~(pfx.Data()[bit >> 3])) >> (bit & 0x7)) & 0x1);
		}
		
		return(0);
	}

	//--------------------------------------------------------------------------
	//!  
	//--------------------------------------------------------------------------
	template <>
	inline int BitFirstDifferent<Ipv4Prefix>(const Ipv4Prefix & key1, 
                                           const Ipv4Prefix & key2)
	{
		int n = 0;
		for ( ; n < 5; ++n) {
			if (key1.Data()[n] != key2.Data()[n])
				break;
		}
		if (n == 5)
			return(40);
		
		int d = 0;
		if (n == 4) {
			while (GetBit<Ipv4Prefix>(key1, (n * 8) + d) == 
				   GetBit<Ipv4Prefix>(key2, (n * 8) + d) && d < 8)
				++d;
		}
		else {
			while (GetBit<uint8_t>(key1.Data()[n], d) == 
				   GetBit<uint8_t>(key2.Data()[n], d) && d < 8)
				++d;
		}
		
		return ((n << 3) + d);
	}
	
	//--------------------------------------------------------------------------
	//!  
	//--------------------------------------------------------------------------
	template <>
	bool IsMatch<Ipv4Prefix>(const Ipv4Prefix & key1,
                           const Ipv4Prefix & key2)
	{
		cerr << "IsMatch(" << setw(18) << key1 << "," << setw(18) << key2 << ")"
         << endl;
		return(key1.Contains(key2));
	}
	
	//--------------------------------------------------------------------------
	//!  
	//--------------------------------------------------------------------------
	template <>
	inline int GetBit<Ipv4Address>(const Ipv4Address & ipAddr, int bit)
	{
    return ipAddr.GetBit(bit);
    
		int  rc = 0;
		// assert(bit <= 32);
		if (bit < 32) {
			ipv4addr_t  rawAddr = ipAddr.Raw();
			const uint8_t  *p = (const uint8_t *)(&rawAddr);
			rc = ((p[bit >> 3]) >> (7 - (bit & 0x7))) & 0x1;
		}
		return(rc);
	}
	
	//--------------------------------------------------------------------------
	//!  
	//--------------------------------------------------------------------------
	template <>
	inline int BitFirstDifferent<Ipv4Address>(const Ipv4Address & key1, 
											  const Ipv4Address & key2)
	{
		return(BitFirstDifferent<ipv4addr_t>(key1.Raw(), key2.Raw()));
	}
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestIostream(const Patricia<Ipv4Prefix,uint32_t> & pt,
                  const vector<Ipv4Prefix> & pfxVec)
{
  ofstream  os("Ipv4Prefix.dat");
  if (os) {
    if (! pt.Write(os))
      abort();
    os.close();
  }
  else {
    abort();
  }
  
  Patricia<Ipv4Prefix,uint32_t>  pt2;
  ifstream is("Ipv4Prefix.dat");
  if (is) {
    if (! pt2.Read(is))
      abort();
    is.close();
    cerr << "pt2.Size(): " << pt2.Size() << endl;
	vector<Ipv4Prefix>::const_iterator  viter;
    for (viter = pfxVec.begin(); viter != pfxVec.end(); ++viter) {
      PatriciaNode<Ipv4Prefix,uint32_t>  *node = pt2.LookupNode(*viter);
      UnitAssert(node);
      if (! node)
        cerr << "Patricia<Ipv4Prefix,uint32_t> failed to find "
             << *viter << endl;
    }
  }
  else {
    abort();
  }
  
  unlink("Ipv4Prefix.dat");
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestIpv4PrefixKey(const std::string & fileName, 
                       const std::string & ipaddr)
{
  Patricia<Ipv4Prefix,uint32_t>  pt;
  vector<Ipv4Prefix>             pfxVec;

  ifstream  is(fileName.c_str());
  if (is) {
    char  buf[512];
    memset(buf,0,512);
    uint32_t   i = 0;
    while (is.getline(buf,512,'\n')) {
      string      pfxstr(buf);
      Ipv4Prefix  pfx(pfxstr);
      pt.Insert(pfx, i);
      ++i;
      pfxVec.push_back(pfx);
      memset(buf,0,512);
    }
    is.close();

	vector<Ipv4Prefix>::const_iterator  viter;
    PatriciaNode<Ipv4Prefix,uint32_t>  *node = 0;
    int        maxReps = 1;
    Dwm::TimeValue  startTime(true);
    for (int  c = 0; c < maxReps; ++c) {
      for (viter = pfxVec.begin(); viter != pfxVec.end(); ++viter) {
        node = pt.LookupNode(*viter);
        UnitAssert(node);
      }
    }
    Dwm::TimeValue endTime(true);
    if (g_showPerformance) {
      endTime -= startTime;
      uint64_t  lookupsPerSec = 
        (uint64_t)((pfxVec.size() * maxReps) / (double)endTime);
      cout << "Patricia<Ipv4Prefix,uint32_t> " << pt.Size() << " prefixes, "
           << lookupsPerSec << " lookups/sec" << endl;
    }
    
    TestIostream(pt, pfxVec);
    
    //  test deletion
    for (viter = pfxVec.begin(); viter != pfxVec.end(); ++viter) {
      UnitAssert(pt.Delete(*viter));
    }
    UnitAssert(pt.Size() == 0);
    
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestIpv4PrefixKeyLongest(const std::string & pfxFile,
                              const std::string & addrFile)
{
  Patricia<Ipv4Prefix,uint32_t>  pt;

  //  read in the prefixes
  ifstream  is(pfxFile.c_str());
  if (is) {
    char  buf[512];
    memset(buf,0,512);
    uint32_t   i = 0;
    while (is.getline(buf,512,'\n')) {
      string      pfxstr(buf);
      Ipv4Prefix  pfx(pfxstr);
      pt.Insert(pfx, i);
      ++i;
      memset(buf,0,512);
    }
    is.close();
  }
  else {
    cerr << "Failed to open '" << pfxFile << "'" << endl;
    return;
  }
  
  //  read in the IP addresses
  vector<Ipv4Prefix>        addrVec;
  ifstream  addrStream(addrFile.c_str());
  if (addrStream) {
    char  buf[512];
    memset(buf,0,512);
    
    while (addrStream.getline(buf,512,'\n')) {
      string      pfxstr(buf);
      pfxstr += "/32";
      Ipv4Prefix  pfx(pfxstr);
      addrVec.push_back(pfx);
      memset(buf,0,512);
    }
    addrStream.close();
  }
  else {
    cerr << "Failed to open '" << addrFile << "'" << endl;
    return;
  }
  
  vector<Ipv4Prefix>::const_iterator  viter;
  PatriciaNode<Ipv4Prefix,uint32_t>  *node = 0;
  int             maxReps = 1;
  
  Dwm::TimeValue  startTime(true);
  for (int  c = 0; c < maxReps; ++c) {
    for (viter = addrVec.begin(); viter != addrVec.end(); ++viter) {
      node = pt.LongestMatchNode2(*viter);
      UnitAssert(node);
      if (! node)
        cerr << "Patricia<Ipv4Prefix,uint32_t> failed to find "
             << *viter << endl;
      else
        cerr << "key " << *viter 
             << " longest-match " << node->GetKey()
             << endl;
      
      // val = pt.Lookup(*viter);
    }
  }
  Dwm::TimeValue  endTime(true);
  endTime -= startTime;
  uint64_t  lookupsPerSec =
    (uint64_t)((addrVec.size() * maxReps) / (double)endTime);
  cout << "Patricia<Ipv4Prefix,uint32_t> " << pt.Size() << " prefixes, "
       << lookupsPerSec << " longest-match lookups/sec" << endl;
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestIpv4AddressPerformance(const std::string & fileName)
{
  Patricia<Ipv4Address,uint32_t>  pt;
  vector<Ipv4Address>             addrVec;

  ifstream  is(fileName.c_str());
  if (is) {
    char  *buf = (char *)malloc(512);
    assert(buf);
    memset(buf,0,512);
    uint32_t   i = 0;
    while (is.getline(buf,512,'\n')) {
      string  addrStr(buf);
      string::size_type  endIdx = addrStr.find_first_of("/");
      if (endIdx != addrStr.npos) {
        Ipv4Address  ipAddr(addrStr.substr(0,endIdx));
        pt.Insert(ipAddr, i);
        ++i;
        addrVec.push_back(ipAddr);
      }
      memset(buf,0,512);
    }
    is.close();
    free(buf);

    vector<Ipv4Address>::const_iterator   siter;
    PatriciaNode<Ipv4Address,uint32_t>   *node = 0;
    int                                   maxReps = 5;

    Dwm::TimeValue  startTime(true);
    for (int  c = 0; c < maxReps; ++c) {
      for (siter = addrVec.begin(); siter != addrVec.end(); ++siter) {
        node = pt.LookupNode(*siter);
      }
    }
    assert(node);
    
    Dwm::TimeValue  endTime(true);

    endTime -= startTime;
    cerr << "endTime: " << endTime.Secs() << "," << endTime.Usecs() << '\n'
         << "addrVec.size(): " << addrVec.size() << '\n';
    
    uint64_t  lookupsPerSec =
      (uint64_t)((addrVec.size() * maxReps) / (double)endTime);
    cout << "Patricia<Ipv4Address,uint32_t> " << pt.Size() << " addresses, "
         << lookupsPerSec << " lookups/sec" << endl;

    for (siter = addrVec.begin(); siter != addrVec.end(); ++siter) {
      pt.Delete(*siter);
    }
    UnitAssert(pt.Size() == 0);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestIpv4Address(const std::string & fileName)
{
  Patricia<Ipv4Address,uint32_t>  pt;
  set<Ipv4Address>                addrSet;

  ifstream  is(fileName.c_str());
  if (is) {
    char  *buf = (char *)malloc(512);
    assert(buf);
    memset(buf,0,512);
    uint32_t   i = 0;
    while (is.getline(buf,512,'\n')) {
      string  addrStr(buf);
      string::size_type  endIdx = addrStr.find_first_of("/");
      if (endIdx != addrStr.npos) {
        Ipv4Address  ipAddr(addrStr.substr(0,endIdx));
        pt.Insert(ipAddr, i);
        ++i;
        addrSet.insert(ipAddr);
      }
      memset(buf,0,512);
    }
    is.close();
    free(buf);

    set<Ipv4Address>::const_iterator      siter;
    PatriciaNode<Ipv4Address,uint32_t>   *node = 0;
    int                                   maxReps = 1;
    for (int  c = 0; c < maxReps; ++c) {
      for (siter = addrSet.begin(); siter != addrSet.end(); ++siter) {
        node = pt.LookupNode(*siter);
        UnitAssert(node);
      }
    }
    for (siter = addrSet.begin(); siter != addrSet.end(); ++siter) {
      UnitAssert(pt.Delete(*siter));
    }
    UnitAssert(pt.Size() == 0);
  }
  
  if (g_showPerformance)
    TestIpv4AddressPerformance(fileName);

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestRawIpv4AddressPerformance(const std::string & fileName)
{
  Patricia<ipv4addr_t,uint32_t>  pt;
  vector<ipv4addr_t>             addrVec;

  ifstream  is(fileName.c_str());
  if (is) {
    char  *buf = (char *)malloc(512);
    assert(buf);
    memset(buf,0,512);
    uint32_t   i = 0;
    while (is.getline(buf,512,'\n')) {
      char  *ipAddrcp = strsep(&buf,"/");
      ipv4addr_t key = inet_addr(ipAddrcp);
      pt.Insert(key, i);
      ++i;
      addrVec.push_back(key);
      buf = ipAddrcp;
      memset(buf,0,512);
    }
    is.close();
    free(buf);
    
    vector<ipv4addr_t>::const_iterator   siter;
    PatriciaNode<ipv4addr_t,uint32_t>   *node = 0;
    int                                  maxReps = 3;

    Dwm::TimeValue  startTime(true);
    for (int  c = 0; c < maxReps; ++c) {
      for (siter = addrVec.begin(); siter != addrVec.end(); ++siter) {
        node = pt.LookupNode(*siter);
      }
    }
    Dwm::TimeValue  endTime(true);
    UnitAssert(node);

    endTime -= startTime;
    uint64_t  lookupsPerSec = 
      (uint64_t)((addrVec.size() * maxReps) / (double)endTime);
    cout << "Patricia<ipv4addr_t,uint32_t> " << pt.Size() << " addresses, "
         << lookupsPerSec << " lookups/sec" << endl;

    uint32_t  numEntries = pt.Size();
    startTime.SetNow();
    for (siter = addrVec.begin(); siter != addrVec.end(); ++siter) {
      pt.Delete(*siter);
    }
    endTime.SetNow();
    endTime -= startTime;
    UnitAssert(pt.Size() == 0);
    uint64_t  deletesPerSec = (uint64_t)(numEntries) / (double)endTime;
    cout << "Patricia<ipv4addr_t,uint32_t> " << numEntries << " addresses, "
         << deletesPerSec << " deletes/sec" << endl;
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestRawIpv4Address(const std::string & fileName)
{
  Patricia<ipv4addr_t,uint32_t>  pt;
  set<ipv4addr_t>                addrSet;

  ifstream  is(fileName.c_str());
  if (is) {
    char  *buf = (char *)malloc(512);
    assert(buf);
    memset(buf,0,512);
    uint32_t   i = 0;
    while (is.getline(buf,512,'\n')) {
      char  *ipAddrcp = strsep(&buf,"/");
      ipv4addr_t key = inet_addr(ipAddrcp);
      pt.Insert(key, i);
      ++i;
      addrSet.insert(key);
      buf = ipAddrcp;
      memset(buf,0,512);
    }
    is.close();
    free(buf);
    
	set<ipv4addr_t>::const_iterator        siter;
    PatriciaNode<ipv4addr_t,uint32_t>   *node = 0;
    int        maxReps = 1;
    for (int  c = 0; c < maxReps; ++c) {
      for (siter = addrSet.begin(); siter != addrSet.end(); ++siter) {
        node = pt.LookupNode(*siter);
        UnitAssert(node);
      }
    }
    for (siter = addrSet.begin(); siter != addrSet.end(); ++siter) {
      UnitAssert(pt.Delete(*siter));
    }
    UnitAssert(pt.Size() == 0);
  }

  if (g_showPerformance)
    TestRawIpv4AddressPerformance(fileName);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStringKeysPerformance(const std::string & fileName)
{
  Patricia<string,uint32_t>  pt;
  vector<string>             stringVec;

  ifstream  is(fileName.c_str());
  if (is) {
    char  buf[512];
    memset(buf,0,512);
    uint32_t   i = 0;
    while (is.getline(buf,512,'\n')) {
      string  key(buf);
      pt.Insert(buf, i);
      ++i;
      stringVec.push_back(key);
      memset(buf,0,512);
    }
    is.close();

    vector<string>::const_iterator   viter;
    PatriciaNode<string,uint32_t>   *node = 0;
    uint32_t                         maxReps = 10;

    Dwm::TimeValue  startTime(true);
    for (uint32_t  c = 0; c < maxReps; ++c) {
      for (viter = stringVec.begin(); viter != stringVec.end(); ++viter) {
        node = pt.LookupNode(*viter);
        if (! node) {
          cerr << "Failed to find node for " << *viter << '\n';
        }
      }
    }
    Dwm::TimeValue  endTime(true);
    endTime.SetNow();
    cerr << "stringVec.size(): " << stringVec.size() << '\n';
    cerr << "startTime: " << startTime.Secs() << "," << startTime.Usecs()
         << '\n';
    cerr << "endTime: " << endTime.Secs() << "," << endTime.Usecs() << '\n';
    endTime -= startTime;
    cerr << "endTime: " << (double)endTime << '\n';
    
    uint64_t  lookupsPerSec = 
      (uint64_t)(stringVec.size() * maxReps) / (double)endTime;
    cout << "Patricia<string,uint32_t> " << pt.Size() << " prefixes, "
         << lookupsPerSec << " lookups/sec" << endl;

    //  test deletion
    for (viter = stringVec.begin(); viter != stringVec.end(); ++viter) {
      UnitAssert(pt.Delete(*viter));
    }
    UnitAssert(pt.Size() == 0);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestStringKeys(const std::string & fileName)
{
  Patricia<string,uint32_t>  pt;
  vector<string>             stringVec;

  ifstream  is(fileName.c_str());
  if (is) {
    char  buf[512];
    memset(buf,0,512);
    uint32_t   i = 0;
    while (is.getline(buf,512,'\n')) {
      string  key(buf);
      pt.Insert(buf, i);
      ++i;
      stringVec.push_back(key);
      memset(buf,0,512);
    }
    is.close();
    
    vector<string>::const_iterator   viter;
    PatriciaNode<string,uint32_t>   *node = 0;
    uint32_t   maxReps = 1;
    for (uint32_t c = 0; c < maxReps; ++c) {
      for (viter = stringVec.begin(); viter != stringVec.end(); ++viter) {
        node = pt.LookupNode(*viter);
        UnitAssert(node);
      }
    }
    //  test deletion
    for (viter = stringVec.begin(); viter != stringVec.end(); ++viter) {
      UnitAssert(pt.Delete(*viter));
    }
    UnitAssert(pt.Size() == 0);
  }

  if (g_showPerformance)
    TestStringKeysPerformance(fileName);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIpv4AddressBasics()
{
  uint8_t  p = 192;

  UnitAssert(GetBit<uint8_t>(p, 0) == 1);
  UnitAssert(GetBit<uint8_t>(p, 1) == 1);
  UnitAssert(GetBit<uint8_t>(p, 2) == 0);
  UnitAssert(GetBit<uint8_t>(p, 3) == 0);
  UnitAssert(GetBit<uint8_t>(p, 4) == 0);
  UnitAssert(GetBit<uint8_t>(p, 5) == 0);
  UnitAssert(GetBit<uint8_t>(p, 6) == 0);
  UnitAssert(GetBit<uint8_t>(p, 7) == 0);
  
  Ipv4Address  ipAddr("0.0.0.0");
  for (uint8_t i = 0; i < 32; ++i)
    UnitAssert(GetBit<Ipv4Address>(ipAddr, i) == 0);
  ipAddr = inet_addr("255.255.255.255");
  for (uint8_t i = 0; i < 32; ++i)
    UnitAssert(GetBit<Ipv4Address>(ipAddr, i) == 1);

  Ipv4Address  ipAddr2("192.172.0.0");
  Ipv4Address  ipAddr3("192.172.255.0");

  UnitAssert(BitFirstDifferent<Ipv4Address>(ipAddr2, ipAddr3) == 16);

  ipAddr3 = ipAddr2;
  UnitAssert(BitFirstDifferent<Ipv4Address>(ipAddr2, ipAddr3) == 32);

  ipAddr2 = inet_addr("192.0.0.0");
  ipAddr3 = inet_addr("0.0.0.0");
  UnitAssert(BitFirstDifferent<Ipv4Address>(ipAddr2, ipAddr3) == 0);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIpv4RawAddressBasics()
{
  uint8_t  p = 192;

  UnitAssert(GetBit<uint8_t>(p, 0) == 1);
  UnitAssert(GetBit<uint8_t>(p, 1) == 1);
  UnitAssert(GetBit<uint8_t>(p, 2) == 0);
  UnitAssert(GetBit<uint8_t>(p, 3) == 0);
  UnitAssert(GetBit<uint8_t>(p, 4) == 0);
  UnitAssert(GetBit<uint8_t>(p, 5) == 0);
  UnitAssert(GetBit<uint8_t>(p, 6) == 0);
  UnitAssert(GetBit<uint8_t>(p, 7) == 0);
  
  ipv4addr_t   ipAddr = 0;
  for (uint8_t i = 0; i < 32; ++i)
    UnitAssert(GetBit<ipv4addr_t>(ipAddr, i) == 0);
  ipAddr = inet_addr("255.255.255.255");
  for (uint8_t i = 0; i < 32; ++i)
    UnitAssert(GetBit<ipv4addr_t>(ipAddr, i) == 1);

  ipv4addr_t  ipAddr2 = inet_addr("192.172.0.0");
  ipv4addr_t  ipAddr3 = inet_addr("192.172.255.0");

  UnitAssert(BitFirstDifferent<ipv4addr_t>(ipAddr2, ipAddr3) == 16);

  ipAddr3 = ipAddr2;
  UnitAssert(BitFirstDifferent<ipv4addr_t>(ipAddr2, ipAddr3) == 32);

  ipAddr2 = inet_addr("192.0.0.0");
  ipAddr3 = inet_addr("0.0.0.0");
  UnitAssert(BitFirstDifferent<ipv4addr_t>(ipAddr2, ipAddr3) == 0);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  OptArgs  optargs;
  optargs.AddOptArg("p", "performance", false, "false",
                    "show performance results");
  optargs.Parse(argc, argv);
  g_showPerformance = optargs.Get<bool>('p');
  
  string  pfxFile("IPV4_prefixes.20051007");
  string  addrFile("IPV4_addrs.20051007");
  
  string  lookupAddr("12.0.21.254");

  TestIpv4AddressBasics();
  TestIpv4RawAddressBasics();
  
  TestIpv4Address(pfxFile);
  TestRawIpv4Address(pfxFile);
  //  TestIpv4PrefixKey(pfxFile, lookupAddr);
  //  TestIpv4PrefixKeyLongest(pfxFile, addrFile);
  
  TestStringKeys(pfxFile);

  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  exit(0);
}
