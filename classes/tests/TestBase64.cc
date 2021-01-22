//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestBase64.cc 9945 $
// @(#) $Id: TestBase64.cc 9945 2018-01-06 08:30:43Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2018
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
//!  \file TestBase64.cc
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/uio.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <ctype.h>
}

#include <cstdlib>
#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmBase64.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestIOStreams()
{
  int  fd = open("/dev/urandom", O_RDONLY);
  if (UnitAssert(fd >= 0)) {
    for (int len = 0; len < 250; ++len) {
      uint8_t  *s = (uint8_t *)calloc(len+1, sizeof(uint8_t));
      if (UnitAssert(s)) {
        if (UnitAssert(read(fd, s, len) == len)) {
          string  ss((char *)s, len);
          istringstream  is(ss);
          ostringstream  os;
          UnitAssert(Base64::Encode(is, os));
          if (ss.size() > 0) {
            UnitAssert(! os.str().empty());
          }
          istringstream  is2(os.str());
          ostringstream  os2;
          UnitAssert(Base64::Decode(is2, os2));
          if (ss.size() > 0) {
            UnitAssert(! os2.str().empty());
          }
          UnitAssert(os2.str() == ss);
        }
        free(s);
      }
    }
    close(fd);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestUrlIOStreams()
{
  int  fd = open("/dev/urandom", O_RDONLY);
  if (UnitAssert(fd >= 0)) {
    for (int len = 0; len < 250; ++len) {
      uint8_t  *s = (uint8_t *)calloc(len+1, sizeof(uint8_t));
      if (UnitAssert(s)) {
        int  chars = 0;
        while (chars < len) {
          if (read(fd, &s[chars], 1) != 1) {
            break;
          }
          if (isascii(s[chars])) {
            ++chars;
          }
        }
        if (UnitAssert(chars == len)) {
          string  ss((char *)s, len);
          istringstream  is(ss);
          ostringstream  os;
          UnitAssert(Base64Url::Encode(is, os));
          if (len > 0) {
            UnitAssert(! os.str().empty());
          }
          istringstream  is2(os.str());
          ostringstream  os2;
          UnitAssert(Base64Url::Decode(is2, os2));
          if (len > 0) {
            UnitAssert(! os2.str().empty());
          }
          UnitAssert(os2.str() == ss);
        }
        free(s);
      }
    }
    close(fd);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestIOStreams();
  TestUrlIOStreams();
  
  string  url("http://www.mcplex.net");
  string  urlEncoded = Base64Url::Encode(url);
  UnitAssert(! urlEncoded.empty());
  UnitAssert(Base64Url::Decode(urlEncoded) == url);
  url = "http://www.mcplex.net/";
  urlEncoded = Base64Url::Encode(url);
  UnitAssert(! urlEncoded.empty());
  UnitAssert(Base64Url::Decode(urlEncoded) == url);
  url = "http://www.mcplex.net/a";
  urlEncoded = Base64Url::Encode(url);
  UnitAssert(! urlEncoded.empty());
  UnitAssert(Base64Url::Decode(urlEncoded) == url);

  int  fd = open("/dev/urandom", O_RDONLY);
  if (UnitAssert(fd >= 0)) {
    string  s, sEncoded;
    for (int len = 0; len < 250; ++len) {
      for (int i = 0; i < len; ++i) {
        uint8_t  c;
        if (UnitAssert(read(fd, &c, 1) == 1)) {
          s += c;
        }
      }
      if (UnitAssert(s.size() == len)) {
        sEncoded = Base64::Encode(s);
        UnitAssert(Base64::Decode(sEncoded) == s);
      }
      s = "";
    }
    close(fd);
  }
      
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return 1;
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  return 0;
}

