//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2020
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
//!  \file DwmIpv4TcpHeader.cc
//!  \author Dnaiel W. McRobb
//!  \brief Dwm::Ipv4TcpHeader class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <netinet/in.h>
  #include <sys/types.h>
  #include <sys/uio.h>
  #include <unistd.h>
}

#include <cstdlib>
#include <cstring>
#include <iomanip>

#include "DwmSvnTag.hh"
#include "DwmIpv4TcpHeader.hh"
#include "DwmIpv4Utils.hh"
#include "DwmSysLogger.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmIpv4TcpHeader.cc 11109 $");

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4TcpHeader::Ipv4TcpHeader()
      : _ownData(true), _options(0), _ownOptions(true)
  {
    _data = new Data();
    assert(_data);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4TcpHeader::Ipv4TcpHeader(const Ipv4PacketHeader & ipHeader)
      : _data((Data *)ipHeader.End()), _options(0), 
        _ownData(false), _ownOptions(false)
  {
    if (DataOffset() > 20) {
      _options = (uint8_t *)_data + 20;
    }
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4TcpHeader::Ipv4TcpHeader(const Ipv4TcpHeader & tcpHeader)
      : _ownData(true), _options(0), _ownOptions(true)
  {
    _data = new Data();
    assert(_data);
    *_data = *tcpHeader._data;
    if (DataOffset() > 20) {
      AllocateOptions(DataOffset() - 20);
      memcpy(_options, tcpHeader._options, DataOffset() - 20);
    }
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4TcpHeader::Ipv4TcpHeader(struct tcphdr *tcpHdr)
      : _data((Data *)tcpHdr), _ownData(false), _ownOptions(false)
  {
    _options = 0;
    if (DataOffset() > 20) {
      _options = (uint8_t *)tcpHdr + sizeof(*tcpHdr);
    }
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4TcpHeader::Ipv4TcpHeader(const struct tcphdr & tcpHdr)
      : _ownData(true), _ownOptions(true)
  {
    _data = new Data();
    assert(_data);
    memcpy(_data, &tcpHdr, sizeof(tcpHdr));
    if (DataOffset() > 20) {
      AllocateOptions(DataOffset() - 20);
      memcpy(_options, (uint8_t *)&tcpHdr + sizeof(tcpHdr), 
             DataOffset() - 20);
    }
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4TcpHeader & Ipv4TcpHeader::operator = (const Ipv4TcpHeader & tcpHeader)
  {
    if (this != &tcpHeader) {
      FreeOptions();
      *_data = *tcpHeader._data;
      _ownData = tcpHeader._ownData;
      _ownOptions = tcpHeader._ownOptions;
      if (DataOffset() > 20) {
        if (_ownOptions) {
          AllocateOptions(DataOffset() - 20);
          memcpy(_options, tcpHeader._options, DataOffset() - 20);
        }
        else {
          _options = tcpHeader._options;
          _ownOptions = false;
        }
      }
    }
    return(*this);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4TcpHeader::~Ipv4TcpHeader()
  {
    if (_ownData) {
      FreeOptions();
      delete(_data);
    }
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4TcpHeader::SourcePort() const
  {
    return(ntohs(_data->sourcePort));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4TcpHeader::SourcePort(uint16_t port)
  {
    _data->sourcePort = htons(port);
    return(ntohs(_data->sourcePort));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4TcpHeader::DestinationPort() const
  {
    return(ntohs(_data->destinationPort));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4TcpHeader::DestinationPort(uint16_t port)
  {
    _data->destinationPort = htons(port);
    return(ntohs(_data->destinationPort));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Ipv4TcpHeader::SequenceNumber() const
  {
    return(ntohl(_data->sequenceNumber));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Ipv4TcpHeader::SequenceNumber(uint32_t seq)
  {
    _data->sequenceNumber = htonl(seq);
    return(ntohl(_data->sequenceNumber));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Ipv4TcpHeader::AckNumber() const
  {
    return(ntohl(_data->ackNumber));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Ipv4TcpHeader::AckNumber(uint32_t ack)
  {
    _data->ackNumber = htonl(ack);
    return(ntohl(_data->ackNumber));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t Ipv4TcpHeader::DataOffset() const
  {
    return((uint8_t)_data->dataOffset << 2);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t Ipv4TcpHeader::DataOffset(uint8_t offset)
  {
    _data->dataOffset = (offset >> 2);
    return((uint8_t)_data->dataOffset << 2);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t Ipv4TcpHeader::Flags() const
  {
    return(_data->flags);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t Ipv4TcpHeader::Flags(uint8_t flags)
  {
    _data->flags = flags;
    return(_data->flags);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4TcpHeader::Window() const
  {
    return(ntohs(_data->window));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4TcpHeader::Window(uint16_t window)
  {
    _data->window = htons(window);
    return(ntohs(_data->window));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4TcpHeader::Checksum() const
  {
    return(ntohs(_data->checksum));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Ipv4TcpHeader::SetChecksum(const Ipv4PacketHeader & ipHeader,
                                  const string & data)
  {
    _data->checksum = 0;

    //  pseudo-header
    uint32_t  sum = ipHeader.SourceAddress().IpSum();
    sum += ipHeader.DestinationAddress().IpSum();
    sum += htons(6);
    sum += htons(DataOffset() + data.length());

    //  header
    sum += Ipv4Utils::Sum((const uint8_t *)_data, sizeof(*_data));

    //  options
    if (DataOffset() > 20) {
      sum += Ipv4Utils::Sum((const uint8_t *)_options, DataOffset() - 20);
    }

    //  payload
    sum += Ipv4Utils::Sum((const uint8_t *)data.c_str(), data.length());

    //  finalize
    Ipv4Utils::AddCarries(sum);
    _data->checksum = (uint16_t)sum;
    
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4TcpHeader::UrgentPointer() const
  {
    return(ntohs(_data->urgentPointer));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4TcpHeader::UrgentPointer(uint16_t urgent)
  {
    _data->urgentPointer = htons(urgent);
    return(ntohs(_data->urgentPointer));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  istream & Ipv4TcpHeader::Read(istream & is)
  {
    FreeOptions();
    memset(_data, 0, sizeof(*_data));
    if (is) {
      if (is.read((char *)_data, 20)) {
        if (DataOffset() > 20) {
          AllocateOptions(DataOffset() - 20);
          is.read((char *)_options, DataOffset() - 20);
        }
      }
    }
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ostream & Ipv4TcpHeader::Write(ostream & os) const
  {
    if (os) {
      os.write((char *)_data, 20);
      if (DataOffset() > 20) {
        os.write((char *)_options, DataOffset() - 20);
      }
    }
    return(os);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv4TcpHeader::Read(int fd)
  {
    FreeOptions();
    ssize_t  rc = -1;
    if (fd >= 0) {
      ssize_t  bytesRead = read(fd, _data, 20);
      if (bytesRead != 20)
        goto err;
      rc = bytesRead;
      if (DataOffset() > 20) {
        AllocateOptions(DataOffset() - 20);
        bytesRead = read(fd, _options, DataOffset() - 20);
        if (bytesRead != DataOffset() - 20)
          goto err;
        rc += bytesRead;
      }
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4TcpHeader::Read(%d) failed: %m", fd);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv4TcpHeader::Write(int fd) const
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      ssize_t  bytesWritten = write(fd, _data, 20);
      if (bytesWritten != 20)
        goto err;
      rc = bytesWritten;
      if (DataOffset() > 20) {
        bytesWritten = write(fd, _options, DataOffset() - 20);
        if (bytesWritten != DataOffset() - 20)
          goto err;
        rc += bytesWritten;
      }
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4TcpHeader::Write(%d) failed: %m", fd);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4TcpHeader::Read(FILE *f)
  {
    FreeOptions();
    size_t  rc = 0;
    if (f) {
      if (! fread(_data, 20, 1, f))
        goto err;
      rc = 1;
      if (DataOffset() > 20) {
        AllocateOptions(DataOffset() - 20);
        if (! fread(_options, DataOffset() - 20, 1, f))
          goto err;
      }
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4TcpHeader::Read(%p) failed: %m", f);
    return(0);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4TcpHeader::Write(FILE *f) const
  {
    size_t  rc = 0;
    if (f) {
      if (! fwrite(_data, 20, 1, f))
        goto err;
      rc = 1;
      if (DataOffset() > 20) {
        if (! fwrite(_options, DataOffset() - 20, 1, f))
          goto err;
      }
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4TcpHeader::Write(%p) failed: %m", f);
    return(0);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4TcpHeader::Read(gzFile gzf)
  {
    FreeOptions();
    int  rc = -1;
    if (gzf) {
      int  bytesRead = gzread(gzf, _data, 20);
      if (bytesRead != 20)
        goto err;
      rc = bytesRead;
      if (DataOffset() > 20) {
        AllocateOptions(DataOffset() - 20);
        bytesRead = gzread(gzf, _options, DataOffset() - 20);
        if (bytesRead != DataOffset() - 20)
          goto err;
        rc += bytesRead;
      }
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4TcpHeader::Read(%p) failed: %m", gzf);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4TcpHeader::Write(gzFile gzf) const
  {
    int  rc = -1;
    if (gzf) {
      int  bytesWritten = gzwrite(gzf, _data, 20);
      if (bytesWritten != 20)
        goto err;
      rc = bytesWritten;
      if (DataOffset() > 20) {
        bytesWritten = gzwrite(gzf, _options, DataOffset() - 20);
        if (bytesWritten != DataOffset() - 20)
          goto err;
        rc += bytesWritten;
      }
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4TcpHeader::Write(%p) failed: %m", gzf);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4TcpHeader::BZRead(BZFILE *bzf)
  {
    FreeOptions();
    int  rc = -1;
    if (bzf) {
      int  bytesRead = BZ2_bzread(bzf, (void *)_data, 20);
      if (bytesRead != 20)
        goto err;
      rc = bytesRead;
      if (DataOffset() > 20) {
        AllocateOptions(DataOffset() - 20);
        bytesRead = BZ2_bzread(bzf, (void *)_options, DataOffset() - 20);
        if (bytesRead != DataOffset() - 20)
          goto err;
        rc += bytesRead;
      }
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4TcpHeader::BZRead(%p) failed: %m", bzf);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4TcpHeader::BZWrite(BZFILE *bzf) const
  {
    int  rc = -1;
    if (bzf) {
      int  bytesWritten = BZ2_bzwrite(bzf, (void *)_data, 20);
      if (bytesWritten != 20)
        goto err;
      rc = bytesWritten;
      if (DataOffset() > 20) {
        bytesWritten = BZ2_bzwrite(bzf, (void *)_options, DataOffset() - 20);
        if (bytesWritten != DataOffset() - 20)
          goto err;
        rc += bytesWritten;
      }
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4TcpHeader::Write(%p) failed: %m", bzf);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Ipv4TcpHeader::StreamedLength() const
  {
    return(_data->dataOffset << 2);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv4TcpHeader::operator == (const Ipv4TcpHeader & tcpHeader) const
  {
    bool  rc = false;
    if (memcmp(_data, tcpHeader._data, sizeof(*_data)) == 0) {
      if (_options == tcpHeader._options ||
          memcmp(_options, tcpHeader._options, DataOffset() - 20) == 0) {
        rc = true;
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ostream & 
  Ipv4TcpHeader::PrintXML(ostream & os, const string & pre) const
  {
    if (os) {
      os << pre << "<tcpheader>\n"
         << pre << "  <sourcePort>" << SourcePort() 
         << "</sourcePort>\n"
         << pre << "  <destinationPort>" << DestinationPort()
         << "</destinationPort>\n"
         << pre << "  <seq>" << SequenceNumber() << "</seq>\n"
         << pre << "  <ack>" << AckNumber() << "</ack>\n"
         << pre << "  <offset>" << (uint16_t)DataOffset() << "</offset>\n"
         << pre << "  <flags>0x" 
         << hex << setfill('0') << setw(2) 
         << (uint16_t)Flags() 
         << setfill(' ') << dec << "</flags>\n"
         << pre << "  <window>" << Window() << "</window>\n"
         << pre << "  <cksum>" << Checksum() << "</cksum>\n"
         << pre << "  <urgent>" << UrgentPointer() << "</urgent>\n";
      if (_options) {
        os << pre << "  <options>" << hex << setfill('0');
        for (int i = 0; i < DataOffset() - 20; ++i) {
          os << setw(2) << (uint16_t)_options[i];
        }
        os << "</options>\n" << dec << setfill(' ');
      }
      os << pre << "</tcpheader>\n";
    }
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t *Ipv4TcpHeader::End() const
  {
    return((uint8_t *)_data + DataOffset());
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Ipv4TcpHeader::FreeOptions()
  {
    if (_ownOptions) {
      if (_options) {
        free(_options);
      }
    }
    _options = 0;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Ipv4TcpHeader::AllocateOptions(uint8_t len)
  {
    _options = (uint8_t *)calloc(1, len);
    assert(_options);
    _ownOptions = true;
    return;
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
