//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005, 2020
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
//!  \file DwmTerminalTricks.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::TerminalTricks implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <curses.h>
  #include <sys/ioctl.h>
  #include <term.h>
  #include <termcap.h>
}

#include <cstring>
#include <iostream>

#include "DwmTerminalTricks.hh"

using namespace std;

static const string svnid("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmTerminalTricks.cc 11634 $");

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static int outc(int c)
{
  cout << (char)c;
  return(c);
}

namespace Dwm {
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TerminalTricks::TerminalTricks()
      : _on(true), _rows(0), _columns(0)
  {
    char   termBuf[4096];
    char  *termCapability = (char *)malloc(4096);
    char  *termCapPtr = termCapability;
    
    int termEntry = tgetent(termBuf, getenv("TERM"));
    
    char  us[3] = "us";
    _startUnderscore = tgetstr(us, &termCapability);
    memset(termCapPtr, 0, 4096);
    termCapability = termCapPtr;
    char  ue[3] = "ue";
    _endUnderscore = tgetstr(ue, &termCapability);
    memset(termCapPtr, 0, 4096);
    termCapability = termCapPtr;
    char  md[3] = "md";
    _startBold = tgetstr(md, &termCapability);
    memset(termCapPtr, 0, 4096);
    termCapability = termCapPtr;
    char  me[3] = "me";
    _endBold = tgetstr(me, &termCapability);
    struct winsize  ws;
    if (ioctl(0, TIOCGWINSZ, &ws) != -1) {
      _columns =  ws.ws_col;
      _rows = ws.ws_row;
    }
    
    free(termCapPtr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void TerminalTricks::On(bool on)
  {
    _on = on;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int TerminalTricks::Columns() const
  {
    return _columns;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int TerminalTricks::Rows() const
  {
    return _rows;
  }
  
}  // namespace Dwm


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
