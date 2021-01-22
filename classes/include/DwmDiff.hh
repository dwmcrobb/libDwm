//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmDiff.hh 10303 $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2016, 2019, 2020
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
//!  \file DwmDiff.hh
//!  \brief Dwm::Diff function templates
//---------------------------------------------------------------------------

#ifndef _DWMDIFF_HH_
#define _DWMDIFF_HH_

#include <algorithm>
#include <type_traits>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Copies elements from the sorted range @c [first1,last1) which are not
  //!  found in the sorted range [first2,last2) to @c in1not2.  Copies
  //!  elements from the sorted range @c [first2,last2) which are not    
  //!  found in the sorted range [first1,last1) to @c in2not1.
  //!  Note that I don't clear @c in1not2 or @c in1not2.
  //!  Note that I do not clear @c in1not2 or @c in2not1 on input, I simply
  //!  append to the end of these containers.
  //--------------------------------------------------------------------------
  template <class InputIt1, class InputIt2, class OutputCont>
  void Diff(InputIt1 first1, InputIt1 last1,
            InputIt2 first2, InputIt2 last2,
            OutputCont & in1not2, OutputCont & in2not1)
  {
    using in1ItCat =
      typename std::iterator_traits<InputIt1>::iterator_category;
    static_assert(std::is_convertible_v<in1ItCat, std::input_iterator_tag>,
                  "InputIt1 must be an input iterator");
    using in2ItCat =
      typename std::iterator_traits<InputIt2>::iterator_category;
    static_assert(std::is_convertible_v<in2ItCat, std::input_iterator_tag>,
                  "InputIt2 must be an input iterator");

    // xxx: Until C++20 Concepts, we don't have a great way to check
    // that OutputCont iterators meet the requirements of
    // LegacyOutputIterator.  But since we're going to construct a
    // std::inserter, we can at least check that it is in output iterator.
    // This is really a placeholder 'til we have something more sane.
    using  outItCat =
      typename std::iterator_traits<decltype(std::inserter<OutputCont>(in1not2, in1not2.end()))>::iterator_category;
    static_assert(std::is_convertible_v<outItCat, std::output_iterator_tag>,
                  "OutputCont::iterator does not meet requirements of"
                  " LegacyOutputIterator");

    //  Populate in1not2 with elements from [first1,last1) that are not
    //  found in [first2,last2)
    set_difference(first1, last1, first2, last2,
                   std::inserter(in1not2, in1not2.end()));
    //  Populate in2not1 with elements from [first2,last2) that are not
    //  found in [first1,last1)
    set_difference(first2, last2, first1, last1,
                   std::inserter(in2not1, in2not1.end()));
    return;
  }

  //--------------------------------------------------------------------------
  //!  Copies elements from the sorted range @c [first1,last1) which are not
  //!  found in the sorted range @c [first2,last2) to @c in1not2, using
  //!  @c comp to compare elements.  Copies elements from the sorted range
  //!  @c [first2,last2) which are not found in the sorted range
  //!  @c [first1,last1) to @c in2not1, using @c comp to compare elements.
  //!  Note that I do not clear @c in1not2 or @c in2not1 on input, I simply
  //!  append to the end of these containers.
  //--------------------------------------------------------------------------
  template <class InputIt1, class InputIt2, class OutputCont, class Compare>
  void Diff(InputIt1 first1, InputIt1 last1,
            InputIt2 first2, InputIt2 last2,
            OutputCont & in1not2, OutputCont & in2not1, Compare comp)
  {
    using in1ItCat =
      typename std::iterator_traits<InputIt1>::iterator_category;
    using in2ItCat =
      typename std::iterator_traits<InputIt2>::iterator_category;
    static_assert(std::is_convertible_v<in1ItCat, std::input_iterator_tag>,
                  "InputIt1 must be an input iterator");
    static_assert(std::is_convertible_v<in2ItCat, std::input_iterator_tag>,
                  "InputIt2 must be an input iterator");
    // xxx: Until C++20 Concepts, we don't have a great way to check
    // that OutputCont iterators meet the requirements of
    // LegacyOutputIterator.  But since we're going to construct a
    // std::inserter, we can at least check that it is in output iterator.
    // This is really a placeholder 'til we have something more sane.
    using  outItCat =
      typename std::iterator_traits<decltype(std::inserter<OutputCont>(in1not2, in1not2.end()))>::iterator_category;
    static_assert(std::is_convertible_v<outItCat, std::output_iterator_tag>,
                  "OutputCont::iterator does not meet requirements of"
                  " LegacyOutputIterator");

    //  Populate in1not2 with elements from [first1,last1) that are not
    //  found in [first2,last2)
    set_difference(first1, last1, first2, last2,
                   std::inserter(in1not2, in1not2.end()));
    //  Populate in2not1 with elements from [first2,last2) that are not
    //  found in [first1,last1)
    set_difference(first2, last2, first1, last1,
                   std::inserter(in2not1, in2not1.end()));
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  Copies elements from the sorted container @c in1 which are not
  //!  found in the sorted container @c in2 to @c in1not2.  Copies elements
  //!  from the sorted container @c in2 which are not found in the sorted
  //!  container @c in1 to @c in2not1.
  //!  Note that I do not clear @c in1not2 or @c in2not1 on input, I simply
  //!  append to the end of these containers.
  //--------------------------------------------------------------------------
  template <class InputCont, class OutputCont>
  void Diff(const InputCont & in1, const InputCont & in2,
            OutputCont & in1not2, OutputCont & in2not1)
  {
    return Diff(in1.begin(), in1.end(), in2.begin(), in2.end(),
                in1not2, in2not1);
  }
  
  //--------------------------------------------------------------------------
  //!  Copies elements from the sorted container @c in1 which are not
  //!  found in the sorted container @c in2 to @c in1not2, using @c comp
  //!  to compare elements.  Copies elements from the sorted container
  //!  @c in2 which are not found in the sorted container @c in1 to
  //!  @c in2not1, using @c comp to compare elements.
  //!  Note that I do not clear @c in1not2 or @c in2not1 on input, I simply
  //!  append to the end of these containers.
  //--------------------------------------------------------------------------
  template <class InputCont, class OutputCont, class Compare>
  void Diff(const InputCont & in1, const InputCont & in2,
            OutputCont & in1not2, OutputCont & in2not1, Compare comp)
  {
    return Diff(in1.begin(), in1.end(), in2.begin(), in2.end(),
                in1not2, in2not1, comp);
  }
  
}  // namespace Dwm

#endif  // _DWMDIFF_HH_
