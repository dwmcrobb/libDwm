//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestTypeName.cc 8389 $
// @(#) $Id: TestTypeName.cc 8389 2016-04-17 04:31:36Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007
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
//!  \file TestTypeName.cc
//!  \brief Unit tests for Dwm::TypeName
//---------------------------------------------------------------------------

#include <cassert>
#include <iostream>
#include <string>

#include "DwmTypeName.hh"
#include "DwmIpv4Address.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestTypeName.cc 8389 $");

using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void VoidFunc()
{
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void *VoidPtrFunc()
{
  return(0);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void *VoidPtrArgFunc(void *arg)
{
  return(arg);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Ipv4Address  ipv4addr;
  UnitAssert(TypeName(ipv4addr) == "Dwm::Ipv4Address");
  
  bool  b;
  UnitAssert(TypeName(b) == "bool");
    
  char  c;
  UnitAssert(TypeName(c) == "char");
  
  signed char  sc;
  UnitAssert(TypeName(sc) == "signed char");

  unsigned char  uc;
  UnitAssert(TypeName(uc) == "unsigned char");

  short  sh;
  UnitAssert(TypeName(sh) == "short");

  unsigned short  ush;
  UnitAssert(TypeName(ush) == "unsigned short");

  int  i;
  UnitAssert(TypeName(i) == "int");

  unsigned int  ui;
  UnitAssert(TypeName(ui) == "unsigned int");

  long  l;
  UnitAssert(TypeName(l) == "long");

  unsigned long  ul;
  UnitAssert(TypeName(ul) == "unsigned long");

  long long  ll;
  UnitAssert(TypeName(ll) == "long long");

  unsigned long long  ull;
  UnitAssert(TypeName(ull) == "unsigned long long");

  float  f;
  UnitAssert(TypeName(f) == "float");

  double  d;
  UnitAssert(TypeName(d) == "double");

  //  wchar_t demangling is broken on FreeBSD due to an uptream bug
  //  in the ELF toolchain.  I've patched my machines, see
  //  https://bugs.freebsd.org/bugzilla/show_bug.cgi?id=208661 for my
  //  bug report and patch.
  wchar_t  wc;
  UnitAssert(TypeName(wc) == "wchar_t");
  
  void  *vp;
  UnitAssert(TypeName(vp) == "void*");

  int  *ip;
  UnitAssert(TypeName(ip) == "int*");

  long  *lp;
  UnitAssert(TypeName(lp) == "long*");

  unsigned int  *uip;
  UnitAssert(TypeName(uip) == "unsigned int*");

  unsigned long  *ulp;
  UnitAssert(TypeName(ulp) == "unsigned long*");

#if 0
  //  These don't behave consistently across platforms.
  if (! (UnitAssert(TypeName(VoidFunc) == "void()(void)"))) {
    std::cerr << "TypeName(VoidFunc): " << TypeName(VoidFunc) << '\n';
  }
  if (! (UnitAssert(TypeName(VoidPtrFunc) == "void*()(void)"))) {
    std::cerr << "TypeName(VoidPtrFunc): " << TypeName(VoidPtrFunc) << '\n';
  }
  if (! (UnitAssert(TypeName(VoidPtrArgFunc) == "void*()(void*)"))) {
    std::cerr << "TypeName(VoidPtrArgFunc): " << TypeName(VoidPtrArgFunc)
              << '\n';
  }
#endif
  
  if (Assertions::Total().Failed() > 0) {
    Assertions::Print(std::cerr, true);
    exit(1);
  }
  else
    std::cout << Assertions::Total() << " passed" << std::endl;
  
  exit(0);
}
