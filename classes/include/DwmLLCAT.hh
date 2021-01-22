//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmLLCAT.hh 8401 $
// @(#) $Id: DwmLLCAT.hh 8401 2016-04-17 06:44:31Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006
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
//!  \file DwmLLCAT.hh
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMLLCAT_HH_
#define _DWMLLCAT_HH_

extern "C" {
  #include <inttypes.h>
}

#include <vector>

namespace Dwm {

  template <typename DataType> class LLCATNode;
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename DataType>
  class LLCATSegment
  {
  public:
    LLCATSegment<DataType>(uint8_t K)
    : _K(K), _bits(0), _route(0), _forwardPointer(0)
    {}
    
    //  1 bit
    inline bool StopBit() const
    {
      return(_bits & 0x00000001);
    }

    //  1 bit
    bool StopBit(bool on)
    {
      if (on)
        _bits |= 0x00000001;
      else
        _bits &= ~(0x00000001);
      return(on);
    }
    
    //  1 bit
    bool ValidBit() const
    {
      return(_bits & 0x00000002);
    }

    //  1 bit
    bool ValidBit(bool on)
    {
      if (on)
        _bits |= 0x00000002;
      else
        _bits &= ~(0x00000002);
      return(on);
    }

    //  1 bit
    bool FullPrefixRoute() const
    {
      return(_bits & 0x00000004);
    }

    //  1 bit
    bool FullPrefixRoute(bool on)
    {
      if (on)
        _bits |= 0x00000004;
      else
        _bits &= ~(0x00000004);
      return(on);
    }
    
    //  (_K - 1) bits
    uint8_t LifeChildrenCounter() const
    {
      uint8_t  mask = 0;
      for (uint8_t i = 0; i < (_K - 1); ++i) {
        mask |= (1 << i);
      }
      uint8_t  rc = (_bits >> 3) & mask;
      return(rc);
    }

    uint8_t LifeChildrenCounter(uint8_t lcc)
    {
      uint32_t  mask = 0;
      for (uint8_t i = 0; i < (_K - 1); ++i) {
        mask |= (1 << i);
      }

      //  clear the old bits
      mask <<= 3;
      _bits &= ~(mask);
      //  then set them
      _bits |= ((uint32_t)lcc << 3);
     
      uint8_t  rc = (_bits & mask) >> 3;
      return(rc);
    }
    
    const DataType *Route() const
    {
      return(_route);
    }

    DataType *Route()
    {
      return(_route);
    }

    DataType *Route(DataType *route)
    {
      _route = route;
      return(_route);
    }
     
    // (K - 1) bits
    uint8_t ValidSubPrefixPattern() const
    {
      uint8_t  mask = 0;
      for (uint8_t i = 0; i < (_K - 1); ++i) {
        mask |= (1 << i);
      }
      uint8_t  rc = ((_bits >> (3 + (_K - 1))) & mask);
      return(rc);
     
    }
    
    uint8_t ValidSubPrefixPattern(uint8_t vsp)
    {
      uint32_t  mask = 0;
      for (uint8_t i = 0; i < (_K - 1); ++i) {
        mask |= (1 << i);
      }
     
      //  clear the old bits
      mask <<= 3 + (_K - 1);
      _bits &= ~(mask);
      //  then set them
      _bits |= ((uint32_t)vsp << (3 + (_K - 1)));
     
      uint8_t  rc = (_bits & mask) >> (3 + (_K - 1));
      return(rc);
    }
     
    const LLCATNode<DataType> *ForwardPointer() const
    {
      return(_forwardPointer);
    }

    const LLCATNode<DataType> *ForwardPointer(LLCATNode<DataType> *fp)
    {
      _forwardPointer = fp;
      return(_forwardPointer);
    }

  private:
    uint8_t               _K;
    uint32_t              _bits;
    DataType             *_route;
    LLCATNode<DataType>  *_forwardPointer;
  };





  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename DataType>
  class LLCATNode
  {
  public:
    LLCATNode<DataType>(uint8_t K)
    : _segments(((uint32_t)1 << K), LLCATSegment<DataType>(K))
    {
     
    }
    
  private:
    std::vector<LLCATSegment<DataType> >  _segments;
  };



  
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename DataType>
  class Ipv4LLCAT
  {
  };





  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename DataType>
  class Ipv6LLCAT
  {
  };
  
  
}  // namespace Dwm

#endif  // _DWMLLCAT_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
