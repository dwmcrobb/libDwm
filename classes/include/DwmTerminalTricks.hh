//===========================================================================
// @(#) $Name:  $
// @(#) $Id: DwmTerminalTricks.hh 11633 2021-01-03 05:20:37Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005
//  All rights reserved.
//
//  Permission to use, copy, modify and distribute any part of this
//  software for educational, research and non-profit purposes,
//  without fee, and without a written agreement is hereby granted,
//  provided that the above copyright notice, this paragraph and the
//  following paragraphs appear in all copies.
//
//  Those desiring to incorporate this into commercial products or use
//  for commercial purposes should contact:
//
//    Daniel W. McRobb
//    dwm@mcplex.net
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
//!  \file DwmTerminalTricks.hh
//!  \brief Dwm::TerminalTricks class declaration
//---------------------------------------------------------------------------

#ifndef _DWMTERMINALTRICKS_HH_
#define _DWMTERMINALTRICKS_HH_

#include <string>
#include <sstream>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulate simple terminal tricks (bold and underscore).
  //--------------------------------------------------------------------------
  class TerminalTricks
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor.  Initializes bold and underscore start and stop
    //!  strings according to TERM from the environment.  Turns the
    //!  terminal tricks on.
    //------------------------------------------------------------------------
    TerminalTricks();
    
    //------------------------------------------------------------------------
    //!  If \c on is true, turns the terminal tricks on.  If \c on is
    //!  false, turns the terminal tricks off.
    //------------------------------------------------------------------------
    void On(bool on);
    
    //------------------------------------------------------------------------
    //!  Returns a string representation of \c t, wrapped with bold
    //!  start and end strings for terminal output in bold.
    //------------------------------------------------------------------------
    template <typename T>
    std::string Bold(const T & t) const
    {
      std::ostringstream  os;
      if (_on) {
        os.imbue(std::locale(""));
        os << _startBold;
      }
      os << t;
      if (_on)
        os << _endBold;
      return(os.str());
    }
      
    //------------------------------------------------------------------------
    //!  Returns a string representation of \c t, wrapped with underscore
    //!  start and end strings for terminal output with underscore.
    //------------------------------------------------------------------------
    template <typename T>
    std::string Underscore(const T & t) const
    {
      std::ostringstream  os;
      if (_on) {
        os.imbue(std::locale(""));
        os << _startUnderscore;
      }
      os << t;
      if (_on)
        os << _endUnderscore;
      return(os.str());
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Bold(std::ostream & os, bool on)
    {
      if (_on) {
        if (on) {
          os.imbue(std::locale(""));
          os << _startBold;
        }
        else {
          os << _endBold;
        }
      }
      return;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Underscore(std::ostream & os, bool on)
    {
      if (_on) {
        if (on) {
          os.imbue(std::locale(""));
          os << _startUnderscore;
        }
        else {
          os << _endUnderscore;
        }
      }
      return;
    }
    
    int Columns() const;
    int Rows() const;
    
  private:
    bool         _on;
    std::string  _startBold;
    std::string  _endBold;
    std::string  _startUnderscore;
    std::string  _endUnderscore;
    int          _columns;
    int          _rows;
  };
	
}  // namespace Dwm

#endif  // _DWMTERMINALTRICKS_HH_
