//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007, 2020
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
//!  \file DwmTermios.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Termios class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <ctype.h>
}

#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>

#include "DwmTermios.hh"
#include "DwmSysLogger.hh"

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TermioFlags::TermioFlags()
      : _flagPtr(0), _flagInfo(0)
  {}
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const TermioFlagInfo_t *TermioFlags::FindFlag(const string & flagName) const
  {
    const TermioFlagInfo_t  *tinfo;
    for (tinfo = _flagInfo; tinfo->name; ++tinfo) {
      if ((flagName == tinfo->name) || (flagName == tinfo->longName)) {
        return(tinfo);
      }
    }
    return(0);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool TermioFlags::IsSet(const string & flagName) const
  {
    bool  rc = false;
    const TermioFlagInfo_t  *flag = FindFlag(flagName);
    if (flag) {
      if ((*_flagPtr & flag->mask) == flag->flag) {
        rc = true;
      }
    }
    else {
      Syslog(LOG_ERR, "TermioFlags::IsSet('%s') failed: '%s' unknown",
             flagName.c_str(), flagName.c_str());
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool TermioFlags::Set(const string & flagName)
  {
    bool  rc = false;
    const TermioFlagInfo_t  *flag = FindFlag(flagName);
    if (flag) {
      *_flagPtr &= (~flag->mask);
      *_flagPtr |= flag->flag;
      rc = true;
    }
    else {
      Syslog(LOG_ERR, "TermioFlags::Set('%s') failed: '%s' unknown",
             flagName.c_str(), flagName.c_str());
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool TermioFlags::Unset(const string & flagName)
  {
    bool  rc = false;
    const TermioFlagInfo_t  *flag = FindFlag(flagName);
    if (flag) {
      *_flagPtr &= (~flag->mask);
      *_flagPtr &= (~flag->flag);
      rc = true;
    }
    else {
      Syslog(LOG_ERR, "TermioFlags::Unset('%s') failed: '%s' unknown",
             flagName.c_str(), flagName.c_str());
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ostream & operator << (ostream & os, const TermioFlags & flags)
  {
    if (os) {
      const TermioFlagInfo_t  *tinfo;
      int  flagsPrinted = 0;
      for (tinfo = flags._flagInfo; tinfo->name; ++tinfo) {
        if ((*flags._flagPtr & tinfo->mask) == tinfo->flag) {
          if (flagsPrinted)
            os << "|";
          os << tinfo->name;
          ++flagsPrinted;
        }
      }
    }
    return(os);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TermioInputFlags::TermioInputFlags(struct termios & t)
      : _termios(t)
  {
    _flagPtr = &(_termios.c_iflag);
    _flagInfo = TermioInputFlags::k_flags;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const TermioFlagInfo_t *TermioInputFlags::KnownFlags() const
  {
    return(k_flags);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TermioOutputFlags::TermioOutputFlags(struct termios & t)
      : _termios(t)
  {
    _flagPtr = &(_termios.c_oflag);
    _flagInfo = k_flags;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const TermioFlagInfo_t *TermioOutputFlags::KnownFlags() const
  {
    return(k_flags);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TermioControlFlags::TermioControlFlags(struct termios & t)
      : _termios(t)
  {
    _flagPtr = &(_termios.c_cflag);
    _flagInfo = k_flags;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const TermioFlagInfo_t *TermioControlFlags::KnownFlags() const
  {
    return(k_flags);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TermioLocalFlags::TermioLocalFlags(struct termios & t)
      : _termios(t)
  {
    _flagPtr = &(_termios.c_lflag);
    _flagInfo = k_flags;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const TermioFlagInfo_t *TermioLocalFlags::KnownFlags() const
  {
    return(k_flags);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Termios::Termios()
      : _termios(), _inputFlags(_termios), _outputFlags(_termios),
        _controlFlags(_termios), _localFlags(_termios)
  {
    memset(_termios.c_cc, 0xFF, sizeof(_termios.c_cc));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TermioInputFlags & Termios::InputFlags()
  {
    return(_inputFlags);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TermioOutputFlags & Termios::OutputFlags()
  {
    return(_outputFlags);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TermioControlFlags & Termios::ControlFlags()
  {
    return(_controlFlags);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TermioLocalFlags & Termios::LocalFlags()
  {
    return(_localFlags);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  long Termios::InputSpeed() const
  {
    return(cfgetispeed(&_termios));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  long Termios::InputSpeed(long inputSpeed)
  {
    cfsetispeed(&_termios, inputSpeed);
    return(cfgetispeed(&_termios));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  long Termios::OutputSpeed() const
  {
    return(cfgetospeed(&_termios));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  long Termios::OutputSpeed(long outputSpeed)
  {
    cfsetospeed(&_termios, outputSpeed);
    return(cfgetospeed(&_termios));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const Termios::ControlCharName_t *
  Termios::FindControlChar(const std::string & ccName)
  {
    for (int i = 0; k_controlCharNames[i].name; ++i) {
      if (ccName == k_controlCharNames[i].name) {
        return(&(k_controlCharNames[i]));
      }
    }
    return(0);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const Termios::ControlCharName_t *
  Termios::FindControlChar(cc_t cc)
  {
    for (int i = 0; k_controlCharNames[i].name; ++i) {
      if (cc == k_controlCharNames[i].index) {
        return(&(k_controlCharNames[i]));
      }
    }
    return(0);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  cc_t Termios::ControlChar(const std::string & ccName) const
  {
    cc_t  rc = 0xFF;
    const ControlCharName_t  *cc = FindControlChar(ccName);
    if (cc)
      rc = _termios.c_cc[cc->index];
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  cc_t Termios::ControlChar(const std::string & ccName, cc_t val)
  {
    cc_t  rc = 0xFF;
    const ControlCharName_t  *cc = FindControlChar(ccName);
    if (cc) {
      _termios.c_cc[cc->index] = val;
      rc = _termios.c_cc[cc->index];
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Termios::Get(int fd)
  {
    bool  rc = false;
    if (fd >= 0) {
      if (tcgetattr(fd, &_termios) == 0) {
        rc = true;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Termios::Set(int fd, int action)
  {
    bool  rc = false;
    if (fd >= 0) {
      if (tcsetattr(fd, action, &_termios) == 0) {
        rc = true;
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string Termios::PrintableControlChar(cc_t cc)
  {
    ostringstream  os;
    if (cc < 32) {
      os << "^" << (uint8_t)(cc + 64);
    }
    else {
      if (cc < 127) {
        os << cc;
      }
      else {
        os << "\\" << oct << setfill('0') << setw(3) << cc;
      }
    }
    return(os.str());
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Termios::operator == (const Termios & t) const
  {
    return (memcmp(&_termios, &t._termios, sizeof(_termios)) == 0);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & operator << (std::ostream & os, const Termios & t)
  {
    if (os) {
      os << "InputFlags:    " << t._inputFlags << endl
         << "OutputFlags:   " << t._outputFlags << endl
         << "ControlFlags:  " << t._controlFlags << endl
         << "LocalFlags:    " << t._localFlags << endl
         << "InputSpeed:    " << t.InputSpeed() << endl
         << "OutputSpeed:   " << t.OutputSpeed() << endl
         << "Control Chars: ";
      int  numPrinted = 0;
      for (int i = 0; i < NCCS; ++i) {
        if (t._termios.c_cc[i] != 0xFF) {
          const Termios::ControlCharName_t  *ccInfo = 
            Termios::FindControlChar(i);
          if (ccInfo) {
            os << nouppercase << ccInfo->name << " = " 
               << Termios::PrintableControlChar(t._termios.c_cc[i]) 
               << "; ";
            if (((numPrinted + 1) % 4) == 0) 
              os << endl << "               ";
            ++numPrinted;
          }
        }
      }
    }
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const TermioFlagInfo_t TermioInputFlags::k_flags[] = {
    { IGNBRK,   IGNBRK,  "IGNBRK",   "IgnoreBreak"                     },
    { BRKINT,   BRKINT,  "BRKINT",   "MapBreakToSigintr"               },
    { IGNPAR,   IGNPAR,  "IGNPAR",   "IgnoreParityErrors"              },
    { PARMRK,   PARMRK,  "PARMRK",   "MarkParityErrors"                },
    { INPCK,    INPCK,   "INPCK",    "EnableParityChecking"            },
    { ISTRIP,   ISTRIP,  "ISTRIP",   "Strip8thBit"                     },
    { INLCR,    INLCR,   "INLCR",    "MapNewlineToCarriageReturn"      },
    { IGNCR,    IGNCR,   "IGNCR",    "IgnoreCarriageReturn"            },
    { ICRNL,    ICRNL,   "ICRNL",    "MapCarriageReturnToNewline"      },
    { IXON,     IXON,    "IXON",     "EnableOutputFlowControl"         },
    { IXOFF,    IXOFF,   "IXOFF",    "EnableInputFlowControl"          },
#ifdef IXANY
    { IXANY,    IXANY,   "IXANY",    "AnyCharWillRestartAfterStop"     },
#endif
#ifdef IMAXBEL
    { IMAXBEL,  IMAXBEL, "IMAXBEL",  "RingBellOnInputQueueFull"        },
#endif
    { 0,        0,       0,          0                                 }
  };

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const TermioFlagInfo_t  TermioOutputFlags::k_flags[] = {
    { OPOST,   OPOST,  "OPOST",   "EnableFollowingOutputProcessing"    },
#ifdef ONLCR
    { ONLCR,   ONLCR,  "ONLCR",   "MapNewlineToCarriageReturnNewline"  },
#endif
#ifdef OXTABS
    { OXTABS,  OXTABS, "OXTABS",  "ExpandTabsToSpaces"                 },
#endif
#ifdef ONOEOT
    { ONOEOT,  ONOEOT, "ONOEOT",  "DiscardEOT"                         },
#endif
    { 0,       0,      0,         0                                    }
  };

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const TermioFlagInfo_t  TermioControlFlags::k_flags[] = {
#ifdef CIGNORE
    { CIGNORE,      CIGNORE,    "CIGNORE",    "IgnoreControlFlags"     },
#endif
    //  CS5 brakes our scheme, so don't use it.
    // { CS5,          CSIZE,      "CS5",        "5BitCharacters"         },
    { CS6,          CSIZE,      "CS6",        "6BitCharacters"         },
    { CS7,          CSIZE,      "CS7",        "7BitCharacters"         },
    { CS8,          CSIZE,      "CS8",        "8BitCharacters"         },
    { CSTOPB,       CSTOPB,     "CSTOPB",     "Send2StopBits"          },
    { CREAD,        CREAD,      "CREAD",      "EnableReceiver"         },
    { PARENB,       PARENB,     "PARENB",     "ParityEnable"           },
    { PARODD,       PARODD,     "PARODD",     "ParityOdd"              },
    { HUPCL,        HUPCL,      "HUPCL",      "HangUpOnLastClose"      },
    { CLOCAL,       CLOCAL,     "CLOCAL",     "IgnoreModemStatusLines" },
#ifdef CCTS_OFLOW
    { CCTS_OFLOW,   CCTS_OFLOW, "CCTS_OFLOW", "CTSFlowControlOfOutput" },
#endif
#ifdef CRTS_IFLOW
    { CRTS_IFLOW,   CRTS_IFLOW, "CRTS_IFLOW", "RTSFlowControlOfInput"  },
#endif
#ifdef CDTR_IFLOW
    { CDTR_IFLOW,   CDTR_IFLOW, "CDTR_IFLOW", "DTRFlowControlOfInput"  },
#endif
#ifdef CDSR_OFLOW
    { CDSR_OFLOW,   CDSR_OFLOW, "CDSR_OFLOW", "DSRFlowControlOfOutput" },
#endif
#ifdef CCAR_OFLOW
    { CCAR_OFLOW,   CCAR_OFLOW, "CCAR_OFLOW", "DCDFlowControlOfOutput" },
#endif
    { 0,            0,          0,            0                        }
  };

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const TermioFlagInfo_t  TermioLocalFlags::k_flags[] = {
#ifdef ECHOKE
    { ECHOKE,     ECHOKE,     "ECHOKE",     "VisualEraseLineKill"          },
#endif
    { ECHOE,      ECHOE,      "ECHOE",      "VisuallyEraseChars"           },
    { ECHOK,      ECHOK,      "ECHOK",      "EchoNewlineAfterLineKill"     },
    { ECHO,       ECHO,       "ECHO",       "EnableEchoing" },
    { ECHONL,     ECHONL,     "ECHONL",     "EchoNewline"                  },
#ifdef ECHOPRT
    { ECHOPRT,    ECHOPRT,    "ECHOPRT",    "VisualEraseModeForHardcopy"   },
#endif
#ifdef ECHOCTL
    { ECHOCTL,    ECHOCTL,    "ECHOCTL",    "EchoControlCharsAs^(Char)"    },
#endif
    { ISIG,       ISIG,       "ISIG",       "EnableSignalsIntrQuitSusp"    },
    { ICANON,     ICANON,     "ICANON",     "CanonicalizeInputLines"       },
#ifdef ALTWERASE
    { ALTWERASE,  ALTWERASE,  "ALTWERASE",  "UseAlternateWeraseAlgorithm"  },
#endif
    { IEXTEN,     IEXTEN,     "IEXTEN",     "EnableDiscardAndLnext"        },
#ifdef EXTPROC
    { EXTPROC,    EXTPROC,    "EXTPROC",    "ExternalProcessing"           },
#endif
    { TOSTOP,     TOSTOP,     "TOSTOP",     "StopBackgroundJobsFromOutput" },
#ifdef FLUSHO
    { FLUSHO,     FLUSHO,     "FLUSHO",     "OutputBeingFlushed"           },
#endif
#ifdef NOKERNINFO
    { NOKERNINFO, NOKERNINFO, "NOKERNINFO", "NoKernelOutputFromVstatus"    },
#endif
#ifdef PENDIN
    { PENDIN,     PENDIN,     "PENDIN",     "RetypePendingInput"           },
#endif
    { NOFLSH,     NOFLSH,     "NOFLSH",     "NoFlushAfterInterrupt"        },
    { 0,          0,          0,            0                              }
  };

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const Termios::ControlCharName_t  Termios::k_controlCharNames[] = {
    { VEOF,     "VEOF"     },
    { VEOL,     "VEOL"     },
#ifdef VEOL2
    { VEOL2,    "VEOL2"    },
#endif
    { VERASE,   "VERASE"   },
#ifdef VWERASE
    { VWERASE,  "VWERASE"  },
#endif
    { VKILL,    "VKILL"    },
#ifdef VREPRINT
    { VREPRINT, "VREPRINT" },
#endif
    { VINTR,    "VINTR"    },
    { VQUIT,    "VQUIT"    },
    { VSUSP,    "VSUSP"    },
#ifdef VDSUSP
    { VDSUSP,   "VDSUSP"   },
#endif
    { VSTART,   "VSTART"   },
    { VSTOP,    "VSTOP"    },
#ifdef VLNEXT
    { VLNEXT,   "VLNEXT"   },
#endif
#ifdef VDISCARD
    { VDISCARD, "VDISCARD" },
#endif
    { VMIN,     "VMIN"     },
    { VTIME,    "VTIME"    },
#ifdef VSTATUS
    { VSTATUS,  "VSTATUS"  },
#endif
    { 0,        0          }
  };
  
}  // namespace Dwm


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
