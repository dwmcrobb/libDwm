//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmTermios.hh 8401 $
// @(#) $Id: DwmTermios.hh 8401 2016-04-17 06:44:31Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007, 2016
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
//!  \file DwmTermios.hh
//!  \brief Dwm::Termios and supporting classes definitions
//---------------------------------------------------------------------------

#ifndef _DWMTERMIOS_HH_
#define _DWMTERMIOS_HH_

extern "C" {
#include <inttypes.h>
#include <termios.h>
}

#include <iostream>
#include <string>
#include <utility>

namespace Dwm {

  class Termios;
  
  //--------------------------------------------------------------------------
  //!  Holds termios flag values and names.
  //--------------------------------------------------------------------------
  typedef struct {
    tcflag_t       flag;
    tcflag_t       mask;
    const char    *name;
    const char    *longName;
  } TermioFlagInfo_t;

  //--------------------------------------------------------------------------
  //!  Base class for input, output, control and local termio flags.
  //--------------------------------------------------------------------------
  class TermioFlags
  {
  public:
    virtual ~TermioFlags() { }

    //------------------------------------------------------------------------
    //!  Returns true if the given \c flagName is set, else returns false.
    //------------------------------------------------------------------------
    bool IsSet(const std::string & flagName) const;

    //------------------------------------------------------------------------
    //!  Sets the flag for the given \c flagName.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    bool Set(const std::string & flagName);

    //------------------------------------------------------------------------
    //!  Unsets the flag for the given \c flagName.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    bool Unset(const std::string & flagName);

    //------------------------------------------------------------------------
    //!  Prints to an ostream.
    //------------------------------------------------------------------------
    friend std::ostream & 
    operator << (std::ostream & os, const TermioFlags & flags);

    //------------------------------------------------------------------------
    //!  Returns the known flags.
    //------------------------------------------------------------------------
    virtual const TermioFlagInfo_t *KnownFlags() const = 0;
    
  protected:
    tcflag_t             *_flagPtr;
    const TermioFlagInfo_t  *_flagInfo;

    TermioFlags();
    const TermioFlagInfo_t *FindFlag(const std::string & flagName) const;
  };
  
  //--------------------------------------------------------------------------
  //!  Encapsulates termios input flags
  //--------------------------------------------------------------------------
  class TermioInputFlags
    : public TermioFlags
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    TermioInputFlags(struct termios & t);

    //------------------------------------------------------------------------
    //!  Returns the known input flags.
    //------------------------------------------------------------------------
    const TermioFlagInfo_t *KnownFlags() const;
    
  private:
    struct termios  & _termios;

    static const TermioFlagInfo_t  k_flags[];

    TermioInputFlags();
  };

  //--------------------------------------------------------------------------
  //!  Encapsulates termios output flags
  //--------------------------------------------------------------------------
  class TermioOutputFlags
    : public TermioFlags
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    TermioOutputFlags(struct termios & t);

    //------------------------------------------------------------------------
    //!  Returns the known output flags.
    //------------------------------------------------------------------------
    const TermioFlagInfo_t *KnownFlags() const;

  private:
    struct termios  & _termios;
    
    static const TermioFlagInfo_t  k_flags[];

    TermioOutputFlags();
  };
  
  //--------------------------------------------------------------------------
  //!  Encapsulates termios control flags.
  //--------------------------------------------------------------------------
  class TermioControlFlags
    : public TermioFlags
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    TermioControlFlags(struct termios & t);

    //------------------------------------------------------------------------
    //!  Returns the known control flags.
    //------------------------------------------------------------------------
    const TermioFlagInfo_t *KnownFlags() const;
    
  private:
    struct termios  & _termios;
    
    static const TermioFlagInfo_t  k_flags[];

    TermioControlFlags();
  };

  //--------------------------------------------------------------------------
  //!  Encapsulates termios local flags.
  //--------------------------------------------------------------------------
  class TermioLocalFlags
    : public TermioFlags
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    TermioLocalFlags(struct termios & t);

    //------------------------------------------------------------------------
    //!  Returns the known local flags.
    //------------------------------------------------------------------------
    const TermioFlagInfo_t *KnownFlags() const;
    
  private:
    struct termios  & _termios;
    
    static const TermioFlagInfo_t  k_flags[];

    TermioLocalFlags();
  };
  
  //--------------------------------------------------------------------------
  //!  Encapsulates a struct termios.
  //--------------------------------------------------------------------------
  class Termios
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    Termios();
    
    //------------------------------------------------------------------------
    //!  Returns a reference to the input flags
    //------------------------------------------------------------------------
    TermioInputFlags & InputFlags();

    //------------------------------------------------------------------------
    //!  Returns a reference to the output flags
    //------------------------------------------------------------------------
    TermioOutputFlags & OutputFlags();

    //------------------------------------------------------------------------
    //!  Returns a reference to the control flags
    //------------------------------------------------------------------------
    TermioControlFlags & ControlFlags();

    //------------------------------------------------------------------------
    //!  Returns a reference to the local flags
    //------------------------------------------------------------------------
    TermioLocalFlags & LocalFlags();

    //------------------------------------------------------------------------
    //!  Returns the input speed
    //------------------------------------------------------------------------
    long InputSpeed() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the input speed
    //------------------------------------------------------------------------
    long InputSpeed(long inputSpeed);

    //------------------------------------------------------------------------
    //!  Returns the input speed
    //------------------------------------------------------------------------
    long OutputSpeed() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the input speed
    //------------------------------------------------------------------------
    long OutputSpeed(long outputSpeed);

    //------------------------------------------------------------------------
    //!  Returns the control char value for the given control char named
    //!  \c ccName
    //------------------------------------------------------------------------
    cc_t ControlChar(const std::string & ccName) const;
    
    //------------------------------------------------------------------------
    //!  Sets and returns the control char value for the given control
    //!  char named \c ccName
    //------------------------------------------------------------------------
    cc_t ControlChar(const std::string & ccName, cc_t val);
    
    //------------------------------------------------------------------------
    //!  Gets the termios for the given file descriptor
    //------------------------------------------------------------------------
    bool Get(int fd);

    //------------------------------------------------------------------------
    //!  Applies the termios to the given file descriptor.  \c action
    //!  is TCSANOW, TCSADRAIN or TCSAFLUSH.  TCSASOFT may be or'ed
    //!  with any of these.  See the tcsetattr(3) manpage.
    //------------------------------------------------------------------------
    bool Set(int fd, int action);
    
    //------------------------------------------------------------------------
    //!  Prints to an ostream
    //------------------------------------------------------------------------
    friend std::ostream & operator << (std::ostream & os, const Termios & t);

    //------------------------------------------------------------------------
    //!  operator ==
    //------------------------------------------------------------------------
    bool operator == (const Termios & t) const;
    
  private:
    struct termios      _termios;
    TermioInputFlags    _inputFlags;
    TermioOutputFlags   _outputFlags;
    TermioControlFlags  _controlFlags;
    TermioLocalFlags    _localFlags;

    typedef struct {
      uint8_t      index;
      const char  *name;
    } ControlCharName_t;
    
    static const  ControlCharName_t k_controlCharNames[];

    static std::string PrintableControlChar(cc_t cc);

    static const ControlCharName_t *
    FindControlChar(const std::string & ccName);

    static const ControlCharName_t *
    FindControlChar(cc_t cc);
  };
  
}  // namespace Dwm

#endif  // _DWMTERMIOS_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
