//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2021
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
//!  \file DwmIpv6PrefixMap.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMIPV6PREFIXMAP_HH_
#define _DWMIPV6PREFIXMAP_HH_

#include <map>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#include "DwmASIO.hh"
#include "DwmBZ2IO.hh"
#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmGZIO.hh"
#include "DwmIOUtils.hh"
#include "DwmIpv6Prefix.hh"
#include "DwmStreamIO.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Default hash for our unordered_map in Ipv6PrefixMap.
  //!
  //!  On a Threadripper 3960X, this gets me a bit over 10 million
  //!  longest-match lookups per second with the IPV6_prefixes.202010122 file
  //!  in the tests directory.  IPV6_prefixes.202010122 comes from routeviews
  //!  via CAIDA.
  //!
  //!  The implementation uses xxhash.  I tried some other techniques,
  //!  but they worked very poorly on some architectures (Xeon L5640,
  //!  2018 Macbook Pro).  This is at least consistent.  On a Xeon L5640, I
  //!  get about 5 million longest-match lookups/sec.  On a 2018 Macbook Pro,
  //!  I get 9 to 10 million longest-match lookups/sec.
  //--------------------------------------------------------------------------
  struct OurIpv6PrefixHash
  {
    inline size_t operator () (const Dwm::Ipv6Prefix & pfx) const
    {
      return pfx.Hash();
    }
  };

  //--------------------------------------------------------------------------
  //!  A wrapper around an unordered_map of T keyed by Ipv6Prefix using
  //!  Hash as the hash function.  This isn't the fastest IPv6 prefix
  //!  container, but... it's fast enough for my uses, and is very little
  //!  code to maintain since it leverages the C++ standard library
  //!  facilities.
  //!
  //!  Members are threadsafe (using a mutex).
  //--------------------------------------------------------------------------
  template <typename T, typename Hash = OurIpv6PrefixHash>
  class Ipv6PrefixMap
    : public StreamIOCapable, public FileIOCapable,
      public DescriptorIOCapable, public StreamedLengthCapable,
      public GZIOCapable, public BZ2IOCapable,
      public ASIOReadable, public ASIOWritable
  {
  public:
    //------------------------------------------------------------------------
    //!  Default constructor
    //------------------------------------------------------------------------
    Ipv6PrefixMap()
        : _mtx(), _map(), _lengthCounters()
    {
      _map.max_load_factor(.25);
    }
    
    //------------------------------------------------------------------------
    //!  Adds the given value to the map at key @c pfx.  If the entry
    //!  already exists, it will be replaced.
    //------------------------------------------------------------------------
    void Add(const Ipv6Prefix & pfx, const T & value)
    {
      std::unique_lock  lck(_mtx);
      if (_map.insert_or_assign(pfx, value).second) {
        _lengthCounters[pfx.MaskLength()]++;
      }
      return;
    }

    //------------------------------------------------------------------------
    //!  Find the entry with key @c pfx.  If found, sets @c value to the
    //!  value stored at @c pfx and returns true.  If not found, returns
    //!  false.
    //------------------------------------------------------------------------
    bool Find(const Ipv6Prefix & pfx, T & value) const
    {
      bool  rc = false;
      std::shared_lock  lck(_mtx);
      auto  it = _map.find(pfx);
      if (it != _map.end()) {
        value = it->second;
        rc = true;
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Find the longest match for the given IPv6 address @c addr.  If
    //!  a match is found, sets @c value.first to the matching prefix and
    //!  @c value.second to the value stored at the matching prefix and
    //!  returns true.  If no match is found, returns false.
    //------------------------------------------------------------------------
    bool FindLongest(const Ipv6Address & addr,
                     std::pair<Ipv6Prefix,T> & value) const
    {
      bool              rc = false;
      Ipv6Prefix        pfx(addr, 128);
      std::shared_lock  lck(_mtx);
      for (auto lit = _lengthCounters.rbegin();
           lit != _lengthCounters.rend(); ++lit) {
        pfx.MaskLength(lit->first);
        auto  it = _map.find(pfx);
        if (it != _map.end()) {
          value.first = pfx;
          value.second = it->second;
          rc = true;
          break;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Find all matches for the given IPv6 address @c addr.  If matches
    //!  are found, they are placed in @c values, in most-specific (longest
    //!  prefix length) to least-specific (shortest prefix length) order
    //!  and true is returned.  If no matches are found, returns false.
    //------------------------------------------------------------------------
    bool FindMatches(const Ipv6Address & addr,
                     std::vector<std::pair<Ipv6Prefix,T>> & values) const
    {
      values.clear();
      std::pair<Ipv6Prefix,T>  value;
      std::shared_lock         lck(_mtx);
      for (auto lit = _lengthCounters.rbegin();
           lit != _lengthCounters.rend(); ++lit) {
        Ipv6Prefix  pfx(addr, lit->first);
        auto  it = _map.find(pfx);
        if (it != _map.end()) {
          value.first = pfx;
          value.second = it->second;
          values.push_back(value);
        }
      }
      return (! values.empty());
    }
    
    //------------------------------------------------------------------------
    //!  Removes the entry for the given prefix @c pfx.  Returns true if
    //!  an entry was removed, false if no entry was found for @c pfx.
    //------------------------------------------------------------------------
    bool Remove(const Ipv6Prefix & pfx)
    {
      bool  rc = false;
      std::unique_lock  lck(_mtx);
      auto  it = _map.find(pfx);
      if (it != _map.end()) {
        _map.erase(it);
        auto lit = _lengthCounters.find(pfx.MaskLength());
        lit->second--;
        if (0 == lit->second) {
          _lengthCounters.erase(lit);
        }
        rc = true;
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Clears the map.
    //------------------------------------------------------------------------
    void Clear()
    {
      std::unique_lock  lck(_mtx);
      _map.clear();
      return;
    }

    //------------------------------------------------------------------------
    //!  Returns true if the map is empty.
    //------------------------------------------------------------------------
    bool Empty() const
    {
      std::shared_lock  lck(_mtx);
      return _map.empty();
    }
    
    //------------------------------------------------------------------------
    //!  Reads the Ipv6PrefixMap from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is) override
    {
      std::unique_lock  lck(_mtx);
      return StreamIO::Read(is, _map);
    }
    
    //------------------------------------------------------------------------
    //!  Writes the Ipv6PrefixMap to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const override
    {
      std::shared_lock  lck(_mtx);
      return StreamIO::Write(os, _map);
    }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6PrefixMap from @c f.  Returns 1 on success, 0 on
    //!  failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f) override
    {
      std::unique_lock  lck(_mtx);
      return FileIO::Read(f, _map);
    }
    
    //------------------------------------------------------------------------
    //!  Writes the Ipv6PrefixMap to @c f.  Returns 1 on success, 0 on
    //!  failure.
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const override
    {
      std::shared_lock  lck(_mtx);
      return FileIO::Write(f, _map);
    }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6PrefixMap from file descriptor @c fd.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd) override
    {
      std::unique_lock  lck(_mtx);
      return DescriptorIO::Read(fd, _map);
    }

    //------------------------------------------------------------------------
    //!  Writes the Ipv6PrefixMap to file descriptor @c fd.  Returns the
    //!  number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const override
    {
      std::shared_lock  lck(_mtx);
      return DescriptorIO::Write(fd, _map);
    }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6PrefixMap from @c gzf.  Returns the number of bytes
    //!  read on success, -1 on failure.  Be wary; the integer return is
    //!  risky (could overflow) but it's what zlib's gzread() returns and
    //!  we trickled up the return type.
    //------------------------------------------------------------------------
    int Read(gzFile gzf) override
    {
      std::unique_lock  lck(_mtx);
      return GZIO::Read(gzf, _map);
    }

    //------------------------------------------------------------------------
    //!  Writes the Ipv6PrefixMap to @c gzf.  Returns the number of bytes
    //!  written on success, -1 on failure.  Be wary; the integer return is
    //!  risky (could overflow) but it's what zlib's gzread() returns and
    //!  we trickled up the return type.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const override
    {
      std::shared_lock  lck(_mtx);
      return GZIO::Write(gzf, _map);
    }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6PrefixMap from @c bzf.  Returns the number of bytes
    //!  read on success, -1 on failure.  Be wary; the integer return is
    //!  risky (could overflow) but it's what bzlib's BZ2_bzRead() returns
    //!  and we trickled up the return type.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf) override
    {
      std::unique_lock  lck(_mtx);
      return BZ2IO::BZRead(bzf, _map);
    }
    
    //------------------------------------------------------------------------
    //!  Writes the Ipv6PrefixMap to @c bzf.  Returns the number of bytes
    //!  written on success, -1 on failure.  Be wary; the integer return is
    //!  risky (could overflow) but it's what bzlib's BZ2_bzWrite() returns
    //!  and we trickled up the return type.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const override
    {
      std::shared_lock  lck(_mtx);
      return BZ2IO::BZWrite(bzf, _map);
    }

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if the
    //!  Ipv6PrefixMap was written to a FILE, file descriptor or ostream.
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const override
    {
      std::shared_lock  lck(_mtx);
      return IOUtils::StreamedLength(_map);
    }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6PrefixMap from @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Read(boost::asio::ip::tcp::socket & s) override
    {
      std::unique_lock  lck(_mtx);
      return ASIO::Read(s, _map);
    }

    //------------------------------------------------------------------------
    //!  Writes the Ipv6PrefixMap to @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Write(boost::asio::ip::tcp::socket & s) const override
    {
      std::shared_lock  lck(_mtx);
      return ASIO::Write(s, _map);
    }
    
  private:
    mutable std::shared_mutex              _mtx;
    std::unordered_map<Ipv6Prefix,T,Hash>  _map;
    std::map<uint8_t,uint64_t>             _lengthCounters;
  };
  
}  // namespace Dwm

#endif  // _DWMIPV6PREFIXMAP_HH_
