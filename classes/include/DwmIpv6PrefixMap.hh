//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2021, 2023, 2024
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
  //!  On a Threadripper 3960X, this gets me over 13 million longest-match
  //!  lookups per second with the IPV6_prefixes.202010122 file in the tests
  //!  directory.  The data in IPV6_prefixes.202010122 comes from routeviews
  //!  via CAIDA.
  //!
  //!  The implementation uses xxhash.  I tried some other techniques,
  //!  but they worked very poorly on some architectures (Xeon L5640,
  //!  2018 Macbook Pro).  This is at least consistent.  On a Threadripper
  //!  3960X, I get about 1.5 million longest-match lookups/sec.
  //!
  //!  Note that the interfaces are all single-threaded internally.  Hence
  //!  the performance scales with CPU clock, memory bandwidth, on-die cache,
  //!  etc. and NOT the number of CPU cores.  However, multiple threads can
  //!  safely call any of the public members since we have shared/unique
  //!  locks providing protection.  The overhead of the locks is
  //!  insignificant for members like FindLongest(), but measurable for
  //!  members like Find().  This is a tradeoff I'm willing to make in order
  //!  to maintain enforcement of thread safety in my use.  I rarely write
  //!  single-threaded C++ code these days, and built-in thread safety makes
  //!  my life easier.
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
  {
  public:
    //------------------------------------------------------------------------
    //!  The types of our encapsulated containers.
    //------------------------------------------------------------------------
    using MapType = std::unordered_map<Ipv6Prefix,T,Hash>;
    using LengthMapType = std::map<uint8_t,uint64_t>;

    //------------------------------------------------------------------------
    //!  Internally we keep our maps in a pair.  This simplifies the I/O
    //!  functions.
    //------------------------------------------------------------------------
    using MapPair = std::pair<MapType,LengthMapType>;
    
    //------------------------------------------------------------------------
    //!  Default constructor
    //------------------------------------------------------------------------
    Ipv6PrefixMap()
        : _mtx(), _maps()
    {
      _maps.first.max_load_factor(.25);
    }
    
    //------------------------------------------------------------------------
    //!  Adds the given value to the map at key @c pfx.  If the entry
    //!  already exists, it will be replaced.
    //------------------------------------------------------------------------
    void Add(const Ipv6Prefix & pfx, const T & value)
    {
      std::unique_lock  lck(_mtx);
      return Add(lck, pfx, value);
    }

    //------------------------------------------------------------------------
    //!  Adds the given value to the map at key @c pfx.  If the entry
    //!  already exists, it will be replaced.
    //!  @c lck must be a lock created with UniqueLock() and must be locked.
    //------------------------------------------------------------------------
    void Add(std::unique_lock<std::shared_mutex> & lck,
             const Ipv6Prefix & pfx, const T & value)
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      if (_maps.first.insert_or_assign(pfx, value).second) {
        _maps.second[pfx.MaskLength()]++;
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
      std::shared_lock  lck(_mtx);
      return Find(lck, pfx, value);
    }

    //------------------------------------------------------------------------
    //!  Find the entry with key @c pfx.  If found, sets @c value to the
    //!  value stored at @c pfx and returns true.  If not found, returns
    //!  false.
    //!  @c lck must be a lock created with SharedLock() and must be locked.
    //------------------------------------------------------------------------
    bool Find(std::shared_lock<std::shared_mutex> & lck,
              const Ipv6Prefix & pfx, T & value) const
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return FindNoLock(pfx, value);
    }

    //------------------------------------------------------------------------
    //!  Find the entry with key @c pfx.  If found, sets @c value to the
    //!  value stored at @c pfx and returns true.  If not found, returns
    //!  false.
    //!  @c lck must be a lock created with UniqueLock() and must be locked.
    //------------------------------------------------------------------------
    bool Find(std::unique_lock<std::shared_mutex> & lck,
              const Ipv6Prefix & pfx, T & value) const
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return FindNoLock(pfx, value);
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
      std::shared_lock  lck(_mtx);
      return FindLongest(lck, addr, value);
    }

    //------------------------------------------------------------------------
    //!  Find the longest match for the given IPv6 address @c addr.  If
    //!  a match is found, sets @c value.first to the matching prefix and
    //!  @c value.second to the value stored at the matching prefix and
    //!  returns true.  If no match is found, returns false.
    //!  @c lck must be a lock created with SharedLock() and must be locked.
    //------------------------------------------------------------------------
    bool FindLongest(std::shared_lock<std::shared_mutex> & lck,
                     const Ipv6Address & addr,
                     std::pair<Ipv6Prefix,T> & value) const
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return FindLongestNoLock(addr, value);
    }
    
    //------------------------------------------------------------------------
    //!  Find the longest match for the given IPv6 address @c addr.  If
    //!  a match is found, sets @c value.first to the matching prefix and
    //!  @c value.second to the value stored at the matching prefix and
    //!  returns true.  If no match is found, returns false.
    //!  @c lck must be a lock created with UniqueLock() and must be locked.
    //------------------------------------------------------------------------
    bool FindLongest(std::unique_lock<std::shared_mutex> & lck,
                     const Ipv6Address & addr,
                     std::pair<Ipv6Prefix,T> & value) const
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return FindLongestNoLock(addr, value);
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
      std::shared_lock  lck(_mtx);
      return FindMatches(lck, addr, values);
    }

    //------------------------------------------------------------------------
    //!  Find all matches for the given IPv6 address @c addr.  If matches
    //!  are found, they are placed in @c values, in most-specific (longest
    //!  prefix length) to least-specific (shortest prefix length) order
    //!  and true is returned.  If no matches are found, returns false.
    //!  @c lck must be a lock created with SharedLock() and must be locked.
    //------------------------------------------------------------------------
    bool FindMatches(std::shared_lock<std::shared_mutex> & lck,
                     const Ipv6Address & addr,
                     std::vector<std::pair<Ipv6Prefix,T>> & values) const
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return FindMatchesNoLock(addr, values);
    }

    //------------------------------------------------------------------------
    //!  Find all matches for the given IPv6 address @c addr.  If matches
    //!  are found, they are placed in @c values, in most-specific (longest
    //!  prefix length) to least-specific (shortest prefix length) order
    //!  and true is returned.  If no matches are found, returns false.
    //!  @c lck must be a lock created with UniqueLock() and must be locked.
    //------------------------------------------------------------------------
    bool FindMatches(std::unique_lock<std::shared_mutex> & lck,
                     const Ipv6Address & addr,
                     std::vector<std::pair<Ipv6Prefix,T>> & values) const
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return FindMatchesNoLock(addr, values);
    }
    
    //------------------------------------------------------------------------
    //!  Removes the entry for the given prefix @c pfx.  Returns true if
    //!  an entry was removed, false if no entry was found for @c pfx.
    //------------------------------------------------------------------------
    bool Remove(const Ipv6Prefix & pfx)
    {
      std::unique_lock  lck(_mtx);
      return Remove(lck, pfx);
    }

    //------------------------------------------------------------------------
    //!  Removes the entry for the given prefix @c pfx.  Returns true if
    //!  an entry was removed, false if no entry was found for @c pfx.
    //!  @c lck must be a lock created with UniqueLock() and must be locked.
    //------------------------------------------------------------------------
    bool Remove(std::unique_lock<std::shared_mutex> & lck,
                const Ipv6Prefix & pfx)
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      bool  rc = false;
      auto  it = _maps.first.find(pfx);
      if (it != _maps.first.end()) {
        _maps.first.erase(it);
        auto lit = _maps.second.find(pfx.MaskLength());
        lit->second--;
        if (0 == lit->second) {
          _maps.second.erase(lit);
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
      return Clear(lck);
    }

    //------------------------------------------------------------------------
    //!  Clears the map.
    //!  @c lck must be a lock created with UniqueLock() and must be locked.
    //------------------------------------------------------------------------
    void Clear(std::unique_lock<std::shared_mutex> & lck)
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      _maps.first.clear();
      _maps.second.clear();
      return;
    }
    
    //------------------------------------------------------------------------
    //!  Returns true if the map is empty.
    //------------------------------------------------------------------------
    bool Empty() const
    {
      std::shared_lock  lck(_mtx);
      return Empty(lck);
    }

    //------------------------------------------------------------------------
    //!  Returns true if the map is empty.
    //!  @c lck must be a lock created with SharedLock() and must be locked.
    //------------------------------------------------------------------------
    bool Empty(std::shared_lock<std::shared_mutex> & lck) const
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return _maps.first.empty();
    }

    //------------------------------------------------------------------------
    //!  Returns true if the map is empty.
    //!  @c lck must be a lock created with UniqueLock() and must be locked.
    //------------------------------------------------------------------------
    bool Empty(std::unique_lock<std::shared_mutex> & lck) const
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return _maps.first.empty();
    }
    
    //------------------------------------------------------------------------
    //!  Reads the Ipv6PrefixMap from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is)
    {
      std::unique_lock  lck(_mtx);
      return StreamIO::Read(is, _maps);
    }
    
    //------------------------------------------------------------------------
    //!  Writes the Ipv6PrefixMap to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const
    {
      std::shared_lock  lck(_mtx);
      return StreamIO::Write(os, _maps);
    }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6PrefixMap from @c f.  Returns 1 on success, 0 on
    //!  failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f)
    {
      std::unique_lock  lck(_mtx);
      return FileIO::Read(f, _maps);
    }
    
    //------------------------------------------------------------------------
    //!  Writes the Ipv6PrefixMap to @c f.  Returns 1 on success, 0 on
    //!  failure.
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const
    {
      std::shared_lock  lck(_mtx);
      return FileIO::Write(f, _maps);
    }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6PrefixMap from file descriptor @c fd.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd)
    {
      std::unique_lock  lck(_mtx);
      return DescriptorIO::Read(fd, _maps);
    }

    //------------------------------------------------------------------------
    //!  Writes the Ipv6PrefixMap to file descriptor @c fd.  Returns the
    //!  number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const
    {
      std::shared_lock  lck(_mtx);
      return DescriptorIO::Write(fd, _maps);
    }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6PrefixMap from @c gzf.  Returns the number of bytes
    //!  read on success, -1 on failure.  Be wary; the integer return is
    //!  risky (could overflow) but it's what zlib's gzread() returns and
    //!  we trickled up the return type.
    //------------------------------------------------------------------------
    int Read(gzFile gzf)
    {
      std::unique_lock  lck(_mtx);
      return GZIO::Read(gzf, _maps);
    }

    //------------------------------------------------------------------------
    //!  Writes the Ipv6PrefixMap to @c gzf.  Returns the number of bytes
    //!  written on success, -1 on failure.  Be wary; the integer return is
    //!  risky (could overflow) but it's what zlib's gzread() returns and
    //!  we trickled up the return type.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const
    {
      std::shared_lock  lck(_mtx);
      return GZIO::Write(gzf, _maps);
    }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6PrefixMap from @c bzf.  Returns the number of bytes
    //!  read on success, -1 on failure.  Be wary; the integer return is
    //!  risky (could overflow) but it's what bzlib's BZ2_bzRead() returns
    //!  and we trickled up the return type.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf)
    {
      std::unique_lock  lck(_mtx);
      return BZ2IO::BZRead(bzf, _maps);
    }
    
    //------------------------------------------------------------------------
    //!  Writes the Ipv6PrefixMap to @c bzf.  Returns the number of bytes
    //!  written on success, -1 on failure.  Be wary; the integer return is
    //!  risky (could overflow) but it's what bzlib's BZ2_bzWrite() returns
    //!  and we trickled up the return type.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const
    {
      std::shared_lock  lck(_mtx);
      return BZ2IO::BZWrite(bzf, _maps);
    }

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if the
    //!  Ipv6PrefixMap was written to a FILE, file descriptor or ostream.
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const
    {
      std::shared_lock  lck(_mtx);
      return IOUtils::StreamedLength(_maps);
    }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6PrefixMap from @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Read(boost::asio::ip::tcp::socket & s,
              boost::system::error_code & ec)
    {
      std::unique_lock  lck(_mtx);
      return ASIO::Read(s, _maps, ec);
    }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6PrefixMap from @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Read(boost::asio::local::stream_protocol::socket & s,
              boost::system::error_code & ec)
    {
      std::unique_lock  lck(_mtx);
      return ASIO::Read(s, _maps, ec);
    }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6PrefixMap from @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Read(boost::asio::generic::stream_protocol::socket & s,
              boost::system::error_code & ec)
    {
      std::unique_lock  lck(_mtx);
      return ASIO::Read(s, _maps, ec);
    }
    
    //------------------------------------------------------------------------
    //!  Writes the Ipv6PrefixMap to @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Write(boost::asio::ip::tcp::socket & s,
               boost::system::error_code & ec) const
    {
      std::shared_lock  lck(_mtx);
      return ASIO::Write(s, _maps, ec);
    }

    //------------------------------------------------------------------------
    //!  Writes the Ipv6PrefixMap to @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Write(boost::asio::local::stream_protocol::socket & s,
               boost::system::error_code & ec) const
    {
      std::shared_lock  lck(_mtx);
      return ASIO::Write(s, _maps, ec);
    }

    //------------------------------------------------------------------------
    //!  Writes the Ipv6PrefixMap to @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Write(boost::asio::generic::stream_protocol::socket & s,
               boost::system::error_code & ec) const
    {
      std::shared_lock  lck(_mtx);
      return ASIO::Write(s, _maps, ec);
    }
    
    //------------------------------------------------------------------------
    //!  Returns a shared lock of the Ipv6PrefixMap, in the locked state.
    //!  This should be used with care to avoid deadlock.  It is intended
    //!  for scenarios where the caller needs to perform many read-only
    //!  operations in quick succession and performance is paramount, since
    //!  it allows one to call the members which accept a shared lock and
    //!  hence do not lock and unlock on each call.  Note that the only
    //!  read-only member with significant locking overhead is Find().
    //!  FindLongest()'s locking overhead is dwarfed by the cycles it
    //!  needs for other activities.
    //------------------------------------------------------------------------
    std::shared_lock<std::shared_mutex> SharedLock() const
    {
      return std::shared_lock(_mtx);
    }

    //------------------------------------------------------------------------
    //!  Returns a unique lock of the Ipv6PrefixMap, in the locked state.
    //!  This (and the members that accept a unique lock as an argument)
    //!  should be used with care to avoid deadlock.  It is intended for
    //!  scenarios where the caller needs to perform many operations in
    //!  quick succession and performance is paramount, since it allows one
    //!  to call the members which accept a unique lock and hence do not
    //!  lock and unlock on each call.  Note that the only read-only member
    //!  with significant locking overhead is Find(); FindLongest()'s
    //!  locking overhead is dwarfed by the cycles it needs for other
    //!  activities.  Add() is a little bit faster for repetitive operations
    //!  using the pre-locked version (about 5%).
    //------------------------------------------------------------------------
    std::unique_lock<std::shared_mutex> UniqueLock()
    {
      return std::unique_lock(_mtx);
    }
    
  private:
    mutable std::shared_mutex  _mtx;
    MapPair                    _maps;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool FindNoLock(const Ipv6Prefix & pfx, T & value) const
    {
      bool  rc = false;
      auto  it = _maps.first.find(pfx);
      if (it != _maps.first.end()) {
        value = it->second;
        rc = true;
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool FindLongestNoLock(const Ipv6Address & addr,
                           std::pair<Ipv6Prefix,T> & value) const
    {
      bool              rc = false;
      value.first.Set(addr, 128);
      for (auto lit = _maps.second.rbegin();
           lit != _maps.second.rend(); ++lit) {
        value.first.MaskLength(lit->first);
        auto  it = _maps.first.find(value.first);
        if (it != _maps.first.end()) {
          value.second = it->second;
          rc = true;
          break;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool FindMatchesNoLock(const Ipv6Address & addr,
                           std::vector<std::pair<Ipv6Prefix,T>> & values) const
    {
      values.clear();
      std::pair<Ipv6Prefix,T>  value;
      for (auto lit = _maps.second.rbegin();
           lit != _maps.second.rend(); ++lit) {
        Ipv6Prefix  pfx(addr, lit->first);
        auto  it = _maps.first.find(pfx);
        if (it != _maps.first.end()) {
          value.first = pfx;
          value.second = it->second;
          values.push_back(value);
        }
      }
      return (! values.empty());
    }

  };
  
}  // namespace Dwm

#endif  // _DWMIPV6PREFIXMAP_HH_
