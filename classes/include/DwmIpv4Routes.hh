//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 1999-2005, 2016
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  3. The names of the authors and copyright holders may not be used to
//    endorse or promote products derived from this software without
//    specific prior written permission.
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
//!  \file DwmIpv4Routes.hh
//!  \brief Dwm::Ipv4Routes class template definition
//---------------------------------------------------------------------------

#ifndef _DWMIPV4ROUTES_HH_
#define _DWMIPV4ROUTES_HH_

extern "C" {
#include <assert.h>
}

#include <algorithm>
#include <unordered_map>
#include <vector>

#include "DwmPortability.hh"
#include "DwmBZ2IO.hh"
#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmGZIO.hh"
#include "DwmIOUtils.hh"
#include "DwmStreamIO.hh"
#include "DwmIpv4Prefix.hh"
#include "DwmOperators.hh"

namespace Dwm {
  struct OurIpv4AddressHash
  {
    inline size_t operator () (const Dwm::Ipv4Address & addr) const
    {
      return(addr.Raw());
    }
  };
}  // namespace Dwm

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This template class provides an associative container keyed by
  //!  IPv4 addresses, with longest-match searching.  While this isn't
  //!  as speedy to search as Patricia or radix, it is based on STL
  //!  containers and is hence easy to understand and maintain (note how 
  //!  few lines of actual code are here).
  //!
  //!  I/O functionality is provided, but the real work there is done
  //!  in the Dwm::IO class.
  //!
  //!  Performance
  //!  -----------
  //!    I tested this class with a global IPv4 routing table from 
  //!    October 7, 2005.  It contained 178,567 unique prefixes.  The code
  //!    was compiled with 'g++ -O2 ...' and run on a Xeon 2.8GHz host 
  //!    running FreeBSD 5.3.  Each test looked up one address from each
  //!    of the 178,567 prefixes, and was run 10 times.
  //!
  //!    FindLongest(const Ipv4Address &, std::pair<Ipv4Prefix,_valueT> &)
  //!     - yielded 830,000 lookups/sec with an Ipv4Routes<uint32_t>.
  //!       Since this member copies into the value in the passed-in
  //!       pair reference using operator =, it will be slower with 
  //!       larger _valueT types.
  //!
  //!    FindLongest(const Ipv4Address &, std::pair<Ipv4Prefix,_valueT *> &)
  //!     - yielded 1,040,000 lookups/sec with an Ipv4Routes<string>.
  //!       It makes sense to use this with larger _valueT types.  I've no
  //!       idea why it is faster than the previous FindLongest() function
  //!       that copies by value; it should be just as cheap to copy a
  //!       uint32_t as it is to copy a pointer.
  //!
  //!    In October 2007 I switched to using <unordered_map> for the
  //!    internal containers.  I set the max_load_factor to .15 and get
  //!    1,168,000 lookups/sec for Ipv4Routes<uint32_t> when calling 
  //!    FindLongest(const Ipv4Address &, std::pair<Ipv4Prefix,_valueT> &)
  //!    and 1,489,081 lookups/sec with Ipv4Routes<string> when calling
  //!    FindLongest(const Ipv4Address &, std::pair<Ipv4Prefix,_valueT *> &)
  //!
  //!    More current measurements in 2020 on a Threadripper 3960X: roughly
  //!    5.3 million lookups/second for Ipv4Routes<string>.
  //!
  //!    Note: I don't consider this code fast for lookups; it's a tradeoff.
  //!    You can get _much_ faster lookups with sorted instances of
  //!    std::vector, but at the expense of poor average insertion and
  //!    deletion (and linear in the size of each vector in the worst-case).
  //!    On modern CPUs, cache-friendly containers like std::vector are
  //!    a big advantage for lookups if you keep them sorted, but keeping
  //!    them sorted is expensive.  A quick hack using sorted std::vector
  //!    and std::lower_bound yield about a 10X improvement in lookups but
  //!    a severe penaly for insertions.  This would be less true if the
  //!    typical prefix length distribution was close to normal, but in
  //!    the real world it's typically heavy-tailed with the peak between
  //!    /22 and /24.
  //--------------------------------------------------------------------------
  template <typename _valueT>
  class Ipv4Routes
    : public DescriptorIOCapable, public FileIOCapable, public StreamIOCapable,
      public StreamedLengthCapable, public GZIOCapable, public BZ2IOCapable
  {
  public:
    typedef std::unordered_map<Ipv4Address, _valueT, 
                               OurIpv4AddressHash> _RepSubType;
    typedef typename _RepSubType::const_iterator  const_iterator;

    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    Ipv4Routes<_valueT>()
    : _hashMaps(33)
    {
      for (uint8_t i = 0; i < 33; ++i) {
        _hashMaps[i].max_load_factor(.08);
      }
    }

    //------------------------------------------------------------------------
    //!  Clears all entries.
    //------------------------------------------------------------------------
    void Clear()
    {
      for (uint8_t i = 0; i < 33; ++i)
        _hashMaps[i].clear();
      return;
    }
    
    //------------------------------------------------------------------------
    //!  Returns true if there are no entries.
    //------------------------------------------------------------------------
    bool Empty() const
    {
      bool  rc = true;
      for (uint8_t i = 0; i < 33; ++i) {
        if (! _hashMaps[i].empty()) {
          rc = false;
          break;
        }
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  Adds an entry.  Returns false (and does nothing) if there was
    //!  already an entry present for \c prefix.
    //------------------------------------------------------------------------
    bool Add(const Ipv4Prefix & prefix, const _valueT & value) 
    {
      bool  rc = false;
     
      typename _RepSubType::iterator  iter = 
        _hashMaps[prefix.MaskLength()].find(prefix.Network());
      if (iter == _hashMaps[prefix.MaskLength()].end()) {
        _hashMaps[prefix.MaskLength()][prefix.Network()] = value;
        rc = true;
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  operator [] works like you would expect from an STL map.
    //------------------------------------------------------------------------
    _valueT & operator [] (const Ipv4Prefix & prefix)
    {
      return(_hashMaps[prefix.MaskLength()][prefix.Network()]);
    }
    
    //------------------------------------------------------------------------
    //!  Deletes the entry for \c prefix.  Returns true on success, false
    //!  if an entry wasn't found for \c prefix.
    //------------------------------------------------------------------------
    bool Delete(const Ipv4Prefix & prefix)
    {
      bool  rc = false;
      typename _RepSubType::iterator  iter = 
        _hashMaps[prefix.MaskLength()].find(prefix.Network());
      if (iter != _hashMaps[prefix.MaskLength()].end()) {
        _hashMaps[prefix.MaskLength()].erase(iter);
        rc = true;
      }
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  Find the entry for the given \c prefix.  If an entry is found,
    //!  the value is stored in \c match and true is returned.  Else false
    //!  is returned.
    //------------------------------------------------------------------------
    bool Find(const Ipv4Prefix & prefix, _valueT & match) const
    {
      bool  rc = false;
      if (! _hashMaps[prefix.MaskLength()].empty()) {
        typename _RepSubType::const_iterator  iter =
          _hashMaps[prefix.MaskLength()].find(prefix.Network());
        if (iter != _hashMaps[prefix.MaskLength()].end()) {
          match = iter->second;
          rc = true;
        }
      }
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  Finds the longest match for \c ipAddr.  Places the result in
    //!  \c match and returns true on success.  Returns false if no
    //!  match was found for \c ipAddr.
    //------------------------------------------------------------------------
    bool FindLongest(const Ipv4Address & ipAddr,
                     std::pair<Ipv4Prefix,_valueT> & match) const
    {
      bool  rc = false;

      Ipv4Prefix   lp(ipAddr, 32);
     
      typename _RepSubType::const_iterator  iter;
      for (int8_t i = 32; i >= 0; --i) {
        if (_hashMaps[i].empty())
          continue;
        lp.MaskLength(i);
        iter = _hashMaps[i].find(lp.Network());
        if (iter != _hashMaps[i].end()) {
          match.first = lp;
          match.second = iter->second;
          rc = true;
          break;
        }
      }
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  Finds the longest match for \c ipAddr.  Places the result in
    //!  \c match and returns true on success.  Returns false if no
    //!  match was found for \c ipAddr.
    //!  Note that match.second is a pointer to const for the value stored
    //!  under the prefix.  That means you need to be careful using this
    //!  member; don't call free() or delete() on match.second.
    //------------------------------------------------------------------------
    bool
    FindLongest(const Ipv4Address & ipAddr,
                std::pair<Ipv4Prefix, const _valueT *> & match) const
    {
      bool  rc = false;
      Ipv4Prefix   lp(ipAddr, 32);
      typename _RepSubType::const_iterator  iter;
      for (int8_t i = 32; i >= 0; --i) {
        if (_hashMaps[i].empty())
          continue;
        lp.MaskLength(i);
        iter = _hashMaps[i].find(lp.Network());
        if (iter != _hashMaps[i].end()) {
          match.first = lp;
          match.second = &(iter->second);
          rc = true;
          break;
        }
      }
      return(rc);
    }
        
    //------------------------------------------------------------------------
    //!  Finds all matches for \c ipAddr.  Places the results in \c matches
    //!  (in longest-match-first order) and returns true if any matches were 
    //!  found.  Returns false if no matches were found.
    //------------------------------------------------------------------------
    bool Find(const Ipv4Address & ipAddr,
              std::vector<std::pair<Ipv4Prefix,_valueT> > & matches) const
    {
      if (! matches.empty())
        matches.clear();
     
      typename _RepSubType::const_iterator  iter;

      for (int8_t i = 32; i >= 0; --i) {
        if (_hashMaps[i].empty())
          continue;
        Ipv4Prefix  prefix(ipAddr, i);
        iter = _hashMaps[i].find(prefix.Network());
        if (iter != _hashMaps[i].end()) {
          std::pair<Ipv4Prefix,_valueT> match(prefix, iter->second);
          matches.push_back(match);
        }
      }
      return(! matches.empty());
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void MaxLoadFactor(float loadFactor)
    {
      for (int8_t i = 32; i >= 0; --i) {
        _hashMaps[i].max_load_factor(loadFactor);
      }
      return;
    }

    //------------------------------------------------------------------------
    //!  operator ==
    //!  It's unlikely you'd ever need to use this, and it's expensive.
    //!  It's mainly here for unit testing.
    //------------------------------------------------------------------------
    bool operator == (const Ipv4Routes<_valueT> & r) const
    {
      for (int8_t i = 32; i >= 0; --i) {
        if (_hashMaps[i] != r._hashMaps[i])
          return(false);
      }
      return(true);
    }

    //------------------------------------------------------------------------
    //!  operator !=
    //!  It's unlikely you'd ever need to use this, and it's expensive.
    //!  It's mainly here for unit testing.
    //------------------------------------------------------------------------
    bool operator != (const Ipv4Routes<_valueT> & r) const
    {
      return(! (*this == r));
    }

    //------------------------------------------------------------------------
    //!  Returns the number of routes.
    //------------------------------------------------------------------------
    uint32_t Size() const
    {
      uint32_t  rc = 0;
      for (uint8_t i = 0; i < 33; ++i)
        rc += _hashMaps[i].size();
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void HashSizes(std::vector<std::pair<uint8_t, uint32_t> > & sizes) const
    {
      if (! sizes.empty())
        sizes.clear();
      for (uint8_t i = 0; i < 33; ++i) {
        if (! _hashMaps[i].empty()) {
          sizes.push_back(std::pair<uint8_t,uint32_t>(i,_hashMaps[i].size()));
        }
      }
      return;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    uint32_t StreamedLength() const override
    {
      return(IOUtils::StreamedLength(_hashMaps));
    }
    
    //------------------------------------------------------------------------
    //!  Reads the routes from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is) override
    {
      return(StreamIO::Read(is, _hashMaps));
    }
    
    //------------------------------------------------------------------------
    //!  Writes the routes to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const override
    {
      return(StreamIO::Write(os, _hashMaps));
    }
    
    //------------------------------------------------------------------------
    //!  Reades the routes from a FILE pointer.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f) override
    {
      return(FileIO::Read(f, _hashMaps));
    }
    
    //------------------------------------------------------------------------
    //!  Writes the routes to a FILE pointer.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const override
    {
      return(FileIO::Write(f, _hashMaps));
    }
    
    //------------------------------------------------------------------------
    //!  Reads the routes from a file descriptor.  Returns the number of
    //!  bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd) override
    {
      return(DescriptorIO::Read(fd, _hashMaps));
    }
    
    //------------------------------------------------------------------------
    //!  Writes the routes to a file descriptor.  Returns the number of
    //!  bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const override
    {
      return(DescriptorIO::Write(fd, _hashMaps));
    }

    //------------------------------------------------------------------------
    //!  Reads the routes from a gzFile.  Returns the number of
    //!  bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf) override
    {
      return(GZIO::Read(gzf, _hashMaps));
    }
    
    //------------------------------------------------------------------------
    //!  Writes the routes to a gzFile.  Returns the number of bytes
    //!  written on success, -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const override
    {
      return(GZIO::Write(gzf, _hashMaps));
    }

    //------------------------------------------------------------------------
    //!  Reads the routes from a BZFILE pointer.  Returns the number of
    //!  bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf) override
    {
      return(BZ2IO::BZRead(bzf, _hashMaps));
    }
    
    //------------------------------------------------------------------------
    //!  Writes the routes to a BZFILE pointer.  Returns the number of bytes
    //!  written on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const override
    {
      return(BZ2IO::BZWrite(bzf, _hashMaps));
    }

    //------------------------------------------------------------------------
    //!  Combines adjacent prefixes that can be combined (same value and
    //!  prefixes can be represented by a prefix with a mask length one
    //!  bit wider), then removes specific prefixes that are covered by
    //!  a wider prefix with the same value.
    //!  While we accept a predicate used to compare the two values, it
    //!  only makes sense to use std::equal_to<_valueT>() or an equivalent
    //!  since we make no guarantee of which of the two values we will
    //!  use in the combined entry.
    //------------------------------------------------------------------------
    template <typename BinaryPredicate>
    void Coalesce(BinaryPredicate pred)
    {
      //  Combine adjacents.
      for (uint8_t maskLen = 32; maskLen > 0; --maskLen) {
        auto & hm = this->_hashMaps[maskLen];
        auto hmi = hm.begin();
        while (hmi != hm.end()) {
          bool        erased = false;
          Ipv4Prefix  pfx(hmi->first, maskLen);
          Ipv4Prefix  nextpfx = pfx;
          ++nextpfx;
          std::pair<bool,Ipv4Prefix>  combpfx = pfx.Combine(nextpfx);
          if (combpfx.first) {
            auto  nextit = hm.find(nextpfx.Network());
            if (nextit != hm.end()) {
              if (pred(hmi->second, nextit->second)) {
                hm.erase(nextit);
                _hashMaps[maskLen-1][combpfx.second.Network()] = hmi->second;
                hmi = hm.erase(hmi);
                erased = true;
              }
            }
          }
          if (! erased) {
            ++hmi;
          }
        }
      }
      //  Remove entries that are covered by matching entry with wider
      //  netmask.
      for (uint8_t maskLen = 32; maskLen > 0; --maskLen) {
        auto & hm = this->_hashMaps[maskLen];
        auto   hmi = hm.begin();
        while (hmi != hm.end()) {
          if (HaveWiderMatch(hmi->first, maskLen, hmi->second, pred)) {
            hmi = hm.erase(hmi);
          }
          else {
            ++hmi;
          }
        }
      }
      return;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Coalesce()
    {
      return Coalesce(std::equal_to<_valueT>());
    }
    

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void SortByKey(std::vector<std::pair<Ipv4Prefix,_valueT>> & target,
                   bool ascending = true) const
    {
      if (! target.empty())
        target.clear();
      if (! this->_hashMaps.empty()) {
        target.resize(this->Size());
        typename std::vector<_RepSubType>::const_iterator  iter = 
          this->_hashMaps.begin();
        uint32_t  pfx = 0;
        for (uint8_t hashNum = 0; hashNum < 33; ++hashNum) {
          if (! this->_hashMaps[hashNum].empty()) {
            typename _RepSubType::const_iterator  hiter = 
              this->_hashMaps[hashNum].begin();
            for ( ; hiter != iter->end(); ++hiter) {
              target[pfx].first = Ipv4Prefix(hiter->first, hashNum);
              target[pfx].second = hiter->second;
              ++pfx;
            }
          }
        }
        if (! target.empty()) {
          if (ascending) {
            std::sort(target.begin(), target.end(), KeyLess());
          }
          else {
            std::sort(target.begin(), target.end(), KeyGreater());
          }
        }
      }
     
      return;
    }
      
    //------------------------------------------------------------------------
    //!  Sorts the contained pair<Ipv4Prefix,_valueT> values into a
    //!  vector, in descending order by the value stored for each prefix.
    //!  For example, if you had an Ipv4Routes<uint32_t> object,
    //!  \c target would contain the pair<Ipv4Prefix,_valueT> objects
    //!  sorted in descending order by the uint32_t values.
    //------------------------------------------------------------------------
    void SortByValue(std::vector<std::pair<Ipv4Prefix,_valueT> > & target)
    {
      if (! target.empty())
        target.clear();
      if (! this->_hashMaps.empty()) {
        target.resize(this->Size());
        typename std::vector<_RepSubType>::const_iterator  iter = 
          this->_hashMaps.begin();
        uint32_t  pfx = 0;
        for (uint8_t hashNum = 0; hashNum < 33; ++hashNum) {
          if (! this->_hashMaps[hashNum].empty()) {
            typename _RepSubType::const_iterator  hiter = 
              this->_hashMaps[hashNum].begin();
            for ( ; hiter != iter->end(); ++hiter) {
              target[pfx].first = Ipv4Prefix(hiter->first, hashNum);
              target[pfx].second = hiter->second;
              ++pfx;
            }
          }
        }
        if (! target.empty())
          std::sort(target.begin(), target.end(), ValueGreater());
      }
     
      return;
    }

    //------------------------------------------------------------------------
    //!  Returns the number of addresses covered by the contained prefixes,
    //!  not including 0/0.
    //------------------------------------------------------------------------
    uint32_t AddressesCovered() const
    {
      uint32_t  rc = 0;
      for (int8_t hashNum = 32; hashNum > 1; --hashNum) {
        typename _RepSubType::const_iterator  hiter =
          _hashMaps[hashNum].begin();
        for ( ; hiter != _hashMaps[hashNum].end(); ++hiter) {
          bool  foundWider = false;
          for (int8_t widerHash = hashNum - 1; widerHash > 0; --widerHash) {
            Ipv4Prefix  widerPfx(hiter->first, widerHash);
            if (_hashMaps[widerHash].find(widerPfx.Network())
                != _hashMaps[widerHash].end()) {
              //  found wider match, don't count
              foundWider = true;
              break;
            }
          }
          if (! foundWider) {
            rc += ((uint32_t)1 << (32 - hashNum));
          }
        }
      }
      return rc;
    }
    
    struct KeyGreater
    {
    public:
      bool operator () (const std::pair<Ipv4Prefix,_valueT> & e1,
                        const std::pair<Ipv4Prefix,_valueT> & e2) const
      {
        return(e1.first > e2.first);
      }
    };

    struct KeyLess
    {
    public:
      bool operator () (const std::pair<Ipv4Prefix,_valueT> & e1,
                        const std::pair<Ipv4Prefix,_valueT> & e2) const
      {
        return(e1.first < e2.first);
      }
    };
    
    struct ValueGreater
    {
    public:
      bool operator () (const std::pair<Ipv4Prefix,_valueT> & e1,
                        const std::pair<Ipv4Prefix,_valueT> & e2) const
      {
        return(e1.second > e2.second);
      }
    };
    
    
  protected:
    std::vector<_RepSubType>   _hashMaps;

    template <typename BinaryPredicate>
    bool HaveWiderMatch(const Ipv4Address & addr, uint8_t maskLen,
                        const _valueT & val, BinaryPredicate pred) const
    {
      bool  rc = false;
      if (maskLen > 0) {
        for (int8_t wml = maskLen - 1; wml > 0; --wml) {
          Ipv4Prefix  pfx(addr, wml);
          auto  iter = _hashMaps[wml].find(pfx.Network());
          if (iter != _hashMaps[wml].end()) {
            if (pred(iter->second, val)) {
              rc = true;
              break;
            }
          }
        }
      }
      return rc;
    }
    
  };
  
  
}  // namespace Dwm

#endif  // _DWMIPV4ROUTES_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
