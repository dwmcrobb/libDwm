//===========================================================================
// @(#) $Name$
// @(#) $Id: dwmnet.cc 8954 2017-04-01 20:29:46Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2012
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
//!  \file dwmnet.cc
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <fstream>
#include <string>

#include "DwmIpv4Prefix.hh"
#include "DwmIpv4Routes.hh"
#include "DwmOptArgs.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  bool  rc = 1;
  
  Dwm::OptArgs  optargs;
  optargs.AddOptArg("f:", "file", false, "", "file");
  optargs.AddOptArg("r", "reverse", false, "false", "a.b.c.d e.f.g.h");
  optargs.AddNormalArg("a.b.c.d/n", false);
  int  nextArg = optargs.Parse(argc, argv);

  string  prefixFile = optargs.Get<string>("file");
  if (! prefixFile.empty()) {
    ifstream  is(prefixFile.c_str());
    if (is) {
      Ipv4Routes<uint8_t>  routes;
      string    prefixString;
      while (getline(is, prefixString)) {
        if (! prefixString.empty()) {
          Ipv4Prefix  prefix(prefixString);
          if (nextArg < argc) {
            Ipv4Prefix  addr(argv[nextArg]);
            if (prefix.Contains(addr)) {
              cout << argv[nextArg] << " is in " << prefix << "\n";
              rc = 0;
              break;
            }
          }
          else {
            routes.Add(prefix, 0);
          }
        }
      }
      if (nextArg >= argc) {
        auto  addrsCovered = routes.AddressesCovered();
        cout.imbue(std::locale(""));
        cout << "Addresses: " << addrsCovered << " (" 
             << setprecision(4)
             << addrsCovered / 42949672.96 << "% of 2^32, "
          //  Show percentage of non-reserved unicast address space.
          //  Need to remove space for 10/8, 127/8, 169.254/16,
          //  172.16/12, 198.18/15, 192.168/16, 224/4 from denominator.
          //  That's 2^24 + 2^24 + 2^16 + 2^22 + 2^17 + 2^16 + 2^28
          //    = 306,446,336
             << addrsCovered / (42949672.96 - 3064463.36)
             << "% of publicly routable unicast)\n";
        rc = 0;
      }
      else {
        if (rc) {
          cerr << argv[nextArg] << " is not in any prefix in " << prefixFile
               << "\n";
        }
      }
      return rc;
    }
    else {
      cerr << "Unable to open '" << prefixFile << "': " << strerror(errno)
           << "\n";
      return 1;
    }
  }
  else if (optargs.Get<bool>("reverse")) {
    if (nextArg < (argc - 2)) {
      vector<Ipv4Prefix> &&prefixes =
        Ipv4RangePrefixes(Ipv4Address(argv[nextArg]),
                          Ipv4Address(argv[nextArg+2]));
      if (! prefixes.empty()) {
        auto  pi = prefixes.begin();
        cout << pi->ToShortString();
        ++pi;
        for ( ; pi != prefixes.end(); ++pi) {
          cout << ", " << pi->ToShortString();
        }
        cout << '\n';
      }
    }
  }
  else {
    if (nextArg < argc) {
      Ipv4Prefix  prefix(argv[1]);
      cout << prefix.FirstAddress() << " - " << prefix.LastAddress() << "\n";
      return 0;
    }
    else {
      optargs.Usage(argv[0]);
      return 1;
    }
  }
  
}
