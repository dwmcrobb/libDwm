//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmXDRUtils.cc 11084 $
// @(#) $Id: DwmXDRUtils.cc 11084 2020-09-06 04:57:40Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2020
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
//!  \file DwmXDRUtils.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::XDRUtils class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <rpc/rpc.h>
  #include <rpc/xdr.h>
}

#include <cassert>
#include <cstdlib>

#include "DwmSvnTag.hh"
#include "DwmXDRUtils.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmXDRUtils.cc 11084 $");

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool XDRUtils::Encode(float val, std::array<char,4> & buf)
  {
    XDR   xdrs;
    xdrmem_create(&xdrs, buf.data(), buf.size(), XDR_ENCODE);
    bool  rc = xdr_float(&xdrs, &val) ? true : false;
    xdr_destroy(&xdrs);
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool XDRUtils::Decode(const std::array<char,4> & buf, float & val)
  {
    XDR   xdrs;
    xdrmem_create(&xdrs, (char *)(buf.data()), buf.size(),
                  XDR_DECODE);
    bool  rc = xdr_float(&xdrs, &val) ? true : false;
    xdr_destroy(&xdrs);
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool XDRUtils::Encode(double val, std::array<char,8> & buf)
  {
    XDR   xdrs;
    xdrmem_create(&xdrs, buf.data(), buf.size(), XDR_ENCODE);
    bool  rc = xdr_double(&xdrs, &val) ? true : false;
    xdr_destroy(&xdrs);
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool XDRUtils::Decode(const std::array<char,8> & buf, double & val)
  {
    XDR   xdrs;
    xdrmem_create(&xdrs, (char *)buf.data(), buf.size(), XDR_DECODE);
    bool  rc = xdr_double(&xdrs, &val);
    xdr_destroy(&xdrs);
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t XDRUtils::Encode(float val, char **buf)
  {
    float value = val;
    XDR   xdrs;
    if (*buf) {
      free(*buf);
    }
    *buf = (char *)malloc(4);
    assert(*buf);
    xdrmem_create(&xdrs, *buf, 4, XDR_ENCODE);
    xdr_float(&xdrs, &value);
    xdr_destroy((&xdrs));
    return(4);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void XDRUtils::Decode(char *buf, float & val)
  {
    assert(buf);
    float  value;
    XDR    xdrs;
    xdrmem_create(&xdrs, buf, 4, XDR_DECODE);
    xdr_float(&xdrs, &value);
    val = value;
    xdr_destroy((&xdrs));
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t XDRUtils::Encode(double val, char **buf)
  {
    double  value = val;
    XDR   xdrs;
    if (*buf) {
      free(*buf);
    }
    *buf = (char *)malloc(8);
    assert(*buf);
    xdrmem_create(&xdrs, *buf, 8, XDR_ENCODE);
    xdr_double(&xdrs, &value);
    xdr_destroy((&xdrs));
    return(8);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void XDRUtils::Decode(char *buf, double & val)
  {
    assert(buf);
    double  value;
    XDR     xdrs;
    xdrmem_create(&xdrs, buf, 8, XDR_DECODE);
    xdr_double(&xdrs, &value);
    val = value;
    xdr_destroy((&xdrs));
    return;
  }
  
}  // namespace Dwm
