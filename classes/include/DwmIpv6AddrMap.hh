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
//!  \file DwmIpv6AddrMap.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv6AddrMap class template declaration
//---------------------------------------------------------------------------

#ifndef _DWMIPV6ADDRMAP_HH_
#define _DWMIPV6ADDRMAP_HH_

#include <mutex>
#include <shared_mutex>
#include <unordered_map>

#include "DwmASIO.hh"
#include "DwmBZ2IO.hh"
#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmGZIO.hh"
#include "DwmIOUtils.hh"
#include "DwmIpv6Address.hh"
#include "DwmStreamIO.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  struct OurIpv6AddressHash
  {
    inline size_t operator () (const Dwm::Ipv6Address & addr) const
    {
      return addr.Hash();
    }
  };

  //--------------------------------------------------------------------------
  //!  A thin wrapper around std::unordered_map<Ipv6Address,T,Hash>.  Mainly
  //!  provides thread safety (shared and unique locks).
  //--------------------------------------------------------------------------
  template <typename T, typename Hash = OurIpv6AddressHash>
  class Ipv6AddrMap
  {
  public:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Ipv6AddrMap()
        : _mtx(), _map()
    {
      _map.max_load_factor(0.25);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Add(const Ipv6Address & addr, const T & value)
    {
      std::unique_lock  lck(_mtx);
      _map[addr] = value;
      return;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Add(std::unique_lock<std::shared_mutex> & lck,
             const Ipv6Address & addr, const T & value)
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      _map[addr] = value;
      return;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Find(const Ipv6Address & addr, T & value) const
    {
      std::shared_lock  lck(_mtx);
      return Find(lck, addr, value);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Find(std::shared_lock<std::shared_mutex> & lck,
              const Ipv6Address & addr, T & value) const
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return FindNoLock(addr, value);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Find(std::unique_lock<std::shared_mutex> & lck,
              const Ipv6Address & addr, T & value) const
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return FindNoLock(addr, value);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Remove(const Ipv6Address & addr)
    {
      std::unique_lock  lck(_mtx);
      return Remove(lck, addr);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Remove(std::unique_lock<std::shared_mutex> & lck,
                const Ipv6Address & addr)
    {
      bool  rc = false;
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      auto  it = _map.find(addr);
      if (it != _map.end()) {
        _map.erase(it);
        rc = true;
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Empty() const
    {
      std::shared_lock  lck(_mtx);
      return Empty(lck);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Empty(std::shared_lock<std::shared_mutex> & lck) const
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return _map.empty();
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Empty(std::unique_lock<std::shared_mutex> & lck) const
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return _map.empty();
    }
               
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Clear()
    {
      std::unique_lock  lck(_mtx);
      return Clear(lck);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Clear(std::unique_lock<std::shared_mutex> & lck)
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      _map.clear();
      return;
    }
    
    //------------------------------------------------------------------------
    //!  Reads the Ipv6AddrMap from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is)
    {
      std::unique_lock  lck(_mtx);
      return StreamIO::Read(is, _map);
    }
    
    //------------------------------------------------------------------------
    //!  Writes the Ipv6AddrMap to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const
    {
      std::shared_lock  lck(_mtx);
      return StreamIO::Write(os, _map);
    }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6AddrMap from @c f.  Returns 1 on success, 0 on
    //!  failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f)
    {
      std::unique_lock  lck(_mtx);
      return FileIO::Read(f, _map);
    }
    
    //------------------------------------------------------------------------
    //!  Writes the Ipv6AddrMap to @c f.  Returns 1 on success, 0 on
    //!  failure.
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const
    {
      std::shared_lock  lck(_mtx);
      return FileIO::Write(f, _map);
    }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6AddrMap from file descriptor @c fd.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd)
    {
      std::unique_lock  lck(_mtx);
      return DescriptorIO::Read(fd, _map);
    }

    //------------------------------------------------------------------------
    //!  Writes the Ipv6AddrMap to file descriptor @c fd.  Returns the
    //!  number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const
    {
      std::shared_lock  lck(_mtx);
      return DescriptorIO::Write(fd, _map);
    }
    
    //------------------------------------------------------------------------
    //!  Reads the Ipv6AddrMap from @c gzf.  Returns the number of bytes
    //!  read on success, -1 on failure.  Be wary; the integer return is
    //!  risky (could overflow) but it's what zlib's gzread() returns and
    //!  we trickled up the return type.
    //------------------------------------------------------------------------
    int Read(gzFile gzf)
    {
      std::unique_lock  lck(_mtx);
      return GZIO::Read(gzf, _map);
    }
    
    //------------------------------------------------------------------------
    //!  Writes the Ipv6AddrMap to @c gzf.  Returns the number of bytes
    //!  written on success, -1 on failure.  Be wary; the integer return is
    //!  risky (could overflow) but it's what zlib's gzwrite() returns and
    //!  we trickled up the return type.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const
    {
      std::shared_lock  lck(_mtx);
      return GZIO::Write(gzf, _map);
    }
    
    //------------------------------------------------------------------------
    //!  Reads the Ipv6AddrMap from @c bzf.  Returns the number of
    //!  bytes read on success, -1 on failure.  Be wary; the integer return is
    //!  risky (could overflow) but it's what bzlib's BZ2_bzRead() returns
    //!  and we trickled up the return type.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf)
    {
      std::unique_lock  lck(_mtx);
      return BZ2IO::BZRead(bzf, _map);
    }
    
    //------------------------------------------------------------------------
    //!  Writes the Ipv6AddrMap to @c bzf.  Returns the number of bytes
    //!  written on success, -1 on failure.  Be wary; the integer return is
    //!  risky (could overflow) but it's what bzlib's BZ2_bzWrite() returns
    //!  and we trickled up the return type.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const
    {
      std::shared_lock  lck(_mtx);
      return BZ2IO::BZWrite(bzf, _map);
    }

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if the
    //!  Ipv6AddrMap was written to a FILE, file descriptor or ostream.
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const
    {
      std::shared_lock  lck(_mtx);
      return IOUtils::StreamedLength(_map);
    }
    
    //------------------------------------------------------------------------
    //!  Reads the Ipv6AddrMap from @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Read(boost::asio::ip::tcp::socket & s,
              boost::system::error_code & ec)
    {
      std::unique_lock  lck(_mtx);
      return ASIO::Read(s, _map, ec);
    }

    //------------------------------------------------------------------------
    //!  Writes the Ipv6AddrMap to @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Write(boost::asio::ip::tcp::socket & s,
               boost::system::error_code & ec) const
    {
      std::shared_lock  lck(_mtx);
      return ASIO::Write(s, _map, ec);
    }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6AddrMap from @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Read(boost::asio::local::stream_protocol::socket & s,
              boost::system::error_code & ec)
    {
      std::unique_lock  lck(_mtx);
      return ASIO::Read(s, _map, ec);
    }

    //------------------------------------------------------------------------
    //!  Writes the Ipv6AddrMap to @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Write(boost::asio::local::stream_protocol::socket & s,
               boost::system::error_code & ec) const
    {
      std::shared_lock  lck(_mtx);
      return ASIO::Write(s, _map, ec);
    }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6AddrMap from @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Read(boost::asio::generic::stream_protocol::socket & s,
              boost::system::error_code & ec)
    {
      std::unique_lock  lck(_mtx);
      return ASIO::Read(s, _map, ec);
    }

    //------------------------------------------------------------------------
    //!  Writes the Ipv6AddrMap to @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Write(boost::asio::generic::stream_protocol::socket & s,
               boost::system::error_code & ec) const
    {
      std::shared_lock  lck(_mtx);
      return ASIO::Write(s, _map, ec);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::shared_lock<std::shared_mutex> SharedLock() const
    {
      return std::shared_lock(_mtx);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::unique_lock<std::shared_mutex> UniqueLock()
    {
      return std::unique_lock(_mtx);
    }
    
  private:
    mutable std::shared_mutex               _mtx;
    std::unordered_map<Ipv6Address,T,Hash>  _map;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool FindNoLock(const Ipv6Address & addr, T & value) const
    {
      auto iter = _map.find(addr);
      if (iter != _map.end()) {
        value = iter->second;
        return true;
      }
      return false;
    }
    
  };
  
}  // namespace Dwm

#endif  // _DWMIPV6ADDRMAP_HH_
