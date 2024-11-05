//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmDescriptor.cc 9014 $
// @(#) $Id: TestDwmDescriptor.cc 9014 2017-04-11 09:53:02Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2016
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
//!  \file TestDwmDescriptor.cc
//!  \brief Dwm::Descriptor unit tests
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>
  #include <unistd.h>
}

#include <cstring>

#include "DwmDescriptor.hh"
#include "DwmSvnTag.hh"
#include "DwmSysLogger.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmDescriptor.cc 9014 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  //  SysLogger::Open("TestDwmDescriptor", LOG_PERROR, LOG_USER);
  
  Descriptor  d1 = open("/dev/null", O_RDWR);
  UnitAssert((int)d1 >= 0);

  //  Make sure that a copy-constructed object gets its own descriptor
  Descriptor  d2(d1);
  UnitAssert((int)d2 >= 0);
  UnitAssert((int)d2 != (int)d1);

  //  Make sure Close() sets descriptor to -1
  d1.Close();
  UnitAssert((int)d1 == -1);
  //  and that closing again fails
  UnitAssert(! d1.Close());
  
  //  And that we can still write to the copy-constructed descriptor
  const char *toWrite = "Hello.";
  UnitAssert(write(d2, toWrite, strlen(toWrite)) == strlen(toWrite));

  //  Make sure that an assigned object gets its own descriptor
  Descriptor  d3 = d2;
  UnitAssert((int)d3 >= 0);
  UnitAssert((int)d3 != (int)d2);

  //  And that we can still write to it once the original object is closed
  d2.Close();
  UnitAssert(write(d3, toWrite, strlen(toWrite)) == strlen(toWrite));
  
  int rc = 0;
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
    rc = 0;
  }
  return rc;
}
