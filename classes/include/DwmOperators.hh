//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmOperators.hh 8401 $
// @(#) $Id: DwmOperators.hh 8401 2016-04-17 06:44:31Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2016
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
//!  \file DwmOperators.hh
//!  \brief Miscellaneous operators
//---------------------------------------------------------------------------

#ifndef _DWMOPERATORS_HH_
#define _DWMOPERATORS_HH_

#include <unordered_map>
#include <unordered_set>

#include "DwmPortability.hh"

//----------------------------------------------------------------------------
//!  operator == for unordered_map.
//----------------------------------------------------------------------------
#if (! defined(__clang__))
#if (! HAVE_GPLUSPLUS_VERSION(4,6))
template <typename _keyT, typename _valueT, typename _Hash,
          typename _Pred, typename _Alloc>
bool operator ==
(const std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> & a,
 const std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> & b)
{
  if (a.size() != b.size())
    return(false);
  typename std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc>::const_iterator
    ai, bi;
  for (ai = a.begin(); ai != a.end(); ++ai) {
    bi = b.find(ai->first);
    if (bi == b.end())
      return(false);
    if (bi->second != ai->second)
      return(false);
  }
  return(true);
}
#endif
#endif

//----------------------------------------------------------------------------
//!  operator != for unordered_map.
//----------------------------------------------------------------------------
#if (! defined(__clang__))
#if (! HAVE_GPLUSPLUS_VERSION(4,6))
template <typename _keyT, typename _valueT, typename _Hash,
          typename _Pred, typename _Alloc>
bool operator !=
(const std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> & a,
 const std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> & b)
{
  return(! (a == b));
}
#endif
#endif

//----------------------------------------------------------------------------
//!  operator == for unordered_multimap.
//----------------------------------------------------------------------------
#if (! defined(__clang__))
#if (! HAVE_GPLUSPLUS_VERSION(4,6))
template <typename _keyT, typename _valueT, typename _Hash,
          typename _Pred, typename _Alloc>
bool operator ==
(const std::unordered_multimap<_keyT, _valueT, _Hash, _Pred, _Alloc> & a,
 const std::unordered_multimap<_keyT, _valueT, _Hash, _Pred, _Alloc> & b)
{
  //  Are the multimaps the same size?
  if (a.size() != b.size())
    return(false);
  typedef typename std::unordered_multimap<_keyT, _valueT, _Hash, _Pred, _Alloc>::const_iterator  ConstIterator;
  ConstIterator  ai;
  for (ai = a.begin(); ai != a.end(); ++ai) {
    std::pair<ConstIterator,ConstIterator>  aiRange = 
      a.equal_range(ai->first);
    std::pair<ConstIterator,ConstIterator>  biRange =
      b.equal_range(ai->first);
    //  Do we have the same number of entries for the given key in
    //  each multimap?
    if (std::distance(aiRange.first, aiRange.second)
        != std::distance(biRange.first, biRange.second)) {
      return(false);
    }
    
    //  Do we have the same number of each value for the given key?
    size_t  aCount = 0, bCount = 0;
    ConstIterator  ari = aiRange.first;
    for ( ; ari != aiRange.second; ++ari) {
      ConstIterator  ri;
      for (ri = aiRange.first; ri != aiRange.second; ++ri) {
        if (ri->second == ari->second) {
          ++aCount;
        }
      }
      for (ri = biRange.first ; ri != biRange.second; ++ri) {
        if (ri->second == ari->second) {
          ++bCount;
        }
      }
    }
    if (aCount != bCount) {
      return(false);
    }
  }
  return(true);
}
#endif
#endif

#if (! defined(__clang__))
#if (! HAVE_GPLUSPLUS_VERSION(4,6))
//----------------------------------------------------------------------------
//!  operator != for unordered_multimap.
//----------------------------------------------------------------------------
template <typename _keyT, typename _valueT, typename _Hash,
          typename _Pred, typename _Alloc>
bool operator !=
(const std::unordered_multimap<_keyT, _valueT, _Hash, _Pred, _Alloc> & a,
 const std::unordered_multimap<_keyT, _valueT, _Hash, _Pred, _Alloc> & b)
{
  return(! (a == b));
}
#endif
#endif

//----------------------------------------------------------------------------
//!  operator == for unordered_set.
//----------------------------------------------------------------------------
#if (! defined(__clang__))
#if (! HAVE_GPLUSPLUS_VERSION(4,6))
template <typename _valueT, typename _Hash,
          typename _Pred, typename _Alloc>
bool operator ==
(const std::unordered_set<_valueT, _Hash, _Pred, _Alloc> & a,
 const std::unordered_set<_valueT, _Hash, _Pred, _Alloc> & b)
{
  if (a.size() != b.size())
    return(false);
  typename std::unordered_set<_valueT, _Hash, _Pred, _Alloc>::const_iterator
    ai, bi;
  for (ai = a.begin(); ai != a.end(); ++ai) {
    bi = b.find(*ai);
    if (bi == b.end())
      return(false);
    if (*bi != *ai)
      return(false);
  }
  return(true);
}
#endif
#endif

//----------------------------------------------------------------------------
//!  operator != for unordered_set.
//----------------------------------------------------------------------------
#if (! defined(__clang__))
#if (! HAVE_GPLUSPLUS_VERSION(4,6))
template <typename _valueT, typename _Hash,
          typename _Pred, typename _Alloc>
bool operator !=
(const std::unordered_set<_valueT, _Hash, _Pred, _Alloc> & a,
 const std::unordered_set<_valueT, _Hash, _Pred, _Alloc> & b)
{
  return(! (a == b));
}
#endif
#endif

//----------------------------------------------------------------------------
//!  operator == for unordered_multiset.
//----------------------------------------------------------------------------
#if (! defined(__clang__))
#if (! HAVE_GPLUSPLUS_VERSION(4,6))
template <typename _valueT, typename _Hash,
          typename _Pred, typename _Alloc>
bool operator ==
(const std::unordered_multiset<_valueT, _Hash, _Pred, _Alloc> & a,
 const std::unordered_multiset<_valueT, _Hash, _Pred, _Alloc> & b)
{
  //  Are the multisets the same size?
  if (a.size() != b.size())
    return(false);
  typedef typename std::unordered_multiset<_valueT, _Hash, _Pred, _Alloc>::const_iterator  ConstIterator;
  ConstIterator  ai;
  for (ai = a.begin(); ai != a.end(); ++ai) {
    std::pair<ConstIterator,ConstIterator>  aiRange = a.equal_range(*ai);
    std::pair<ConstIterator,ConstIterator>  biRange = b.equal_range(*ai);
    //  Do we have the same number of entries for the given key in
    //  each multiset?
    if (std::distance(aiRange.first, aiRange.second)
        != std::distance(biRange.first, biRange.second)) {
      return(false);
    }
  }
  return(true);
}
#endif
#endif

//----------------------------------------------------------------------------
//!  operator != for unordered_multiset.
//----------------------------------------------------------------------------
#if (! defined(__clang__))
#if (! HAVE_GPLUSPLUS_VERSION(4,6))
template <typename _valueT, typename _Hash,
          typename _Pred, typename _Alloc>
bool operator !=
(const std::unordered_multiset<_valueT, _Hash, _Pred, _Alloc> & a,
 const std::unordered_multiset<_valueT, _Hash, _Pred, _Alloc> & b)
{
  return(! (a == b));
}
#endif
#endif

#endif  // _DWMOPERATORS_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
