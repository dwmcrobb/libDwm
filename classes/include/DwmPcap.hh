//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmPcap.hh 9530 $
// @(#) $Id: DwmPcap.hh 9530 2017-06-14 17:24:33Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2016
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
//!  \file DwmPcap.hh
//!  \brief Dwm::Pcap class definition
//---------------------------------------------------------------------------

#ifndef _DWMPCAP_HH_
#define _DWMPCAP_HH_

extern "C" {
  #include <pcap.h>
}

#include <map>
#include <string>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This is a pcap wrapper/utility class.
  //--------------------------------------------------------------------------
  class Pcap
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor.
    //------------------------------------------------------------------------
    Pcap();

    //------------------------------------------------------------------------
    //!  Destructor.  Will close the contained pcap device if it's open.
    //------------------------------------------------------------------------
    ~Pcap();

    int SetBufferSize(int bufferSize);
    
    //------------------------------------------------------------------------
    //!  Open \c device for live packet capture.  Will capture no more than
    //!  \c snaplen from each packet.  Will set the device in promiscuous
    //!  mode if \c promisc is true.  \c timeout_msecs specifies the read
    //!  timeout in milliseconds.  See the pcap(3) manpage for details.
    //!  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    bool OpenLive(const std::string & device, int snaplen,
                  bool promisc, int timeout_msecs);

    //------------------------------------------------------------------------
    //!  Specify whether to capture incoming packets, outgoing packets, or
    //!  both.  See the pcap(3) manpage for details.  Returns true on success,
    //!  false on failure.  Note this function is only supported for live
    //!  packet captures.
    //------------------------------------------------------------------------
    bool SetDirection(pcap_direction_t direction);
    
    //------------------------------------------------------------------------
    //!  Opens the file \c fname for reading.  Returns true on success, 
    //!  false on failure.
    //------------------------------------------------------------------------
    bool OpenOffline(const std::string & fname);

    //------------------------------------------------------------------------
    //!  Sets the filter for the capture device.  \c filter is the usual
    //!  pcap filter string syntax.  If \c optimize is true, the resulting
    //!  compiled filter will be optimized.
    //------------------------------------------------------------------------
    bool SetFilter(const std::string & filter, bool optimize);
    
    //------------------------------------------------------------------------
    //!  A wrapper for pcap_dispatch().  See the pcap(3) manpage.
    //------------------------------------------------------------------------
    int Dispatch(int count, pcap_handler callback, uint8_t *user);

    //------------------------------------------------------------------------
    //!  A wrapper for pcap_loop().  See the pcap(3) manpage.
    //------------------------------------------------------------------------
    int Loop(int count, pcap_handler callback, uint8_t *user);

    //------------------------------------------------------------------------
    //!  A wrapper for pcap_breakloop().
    //------------------------------------------------------------------------
    void BreakLoop();
    
    //------------------------------------------------------------------------
    //!  Closes the capture device.  Returns true if the device was open,
    //!  false if it was already closed.
    //------------------------------------------------------------------------
    bool Close();
    
    //------------------------------------------------------------------------
    //!  Returns the last error string from a failed OpenLive() or
    //!  OpenOffline() call.
    //------------------------------------------------------------------------
    const std::string & LastError() const;
    
    //------------------------------------------------------------------------
    //!  Returns the length of the link layer header for the capture
    //!  device.  This lets us skip over it to get to the network 
    //!  layer.  Returns -1 if we don't know the length of the link
    //!  layer header.
    //------------------------------------------------------------------------
    int DataLinkOffset() const;
    
  private:
    static std::map<int,int>  _dataLinkOffsets;

    pcap_t       *_pcap;
    std::string   _errbuf;
  };
  
}  // namespace Dwm

#endif  // _DWMPCAP_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
