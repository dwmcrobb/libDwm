//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmASIO.cc 10964 $
// @(#) $Id: DwmASIO.cc 10964 2020-08-24 07:08:29Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2018, 2020
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
//!  \file DwmASIO.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::ASIO class implementation
//---------------------------------------------------------------------------

#include "DwmPortability.hh"
#include "DwmSvnTag.hh"    
#include "DwmSysLogger.hh"
#include "DwmASIO.hh"
#include "DwmXDRUtils.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmASIO.cc 10964 $");

namespace Dwm {

  using boost::asio::read, boost::asio::write;
  using boost::asio::buffer;
    
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <typename T>
  bool _asioread(boost::asio::ip::tcp::socket & s, T & value)
  {
    bool  rc = false;
    if (s.is_open()) {
      boost::system::error_code  ec;
      if ((read(s, buffer(&value, sizeof(value)), ec) == sizeof(value))
          && (! ec)) {
        rc = true;
      }
      else {
        Syslog(LOG_ERR, "Failed to read %llu bytes from socket",
               sizeof(value));
      }
    }
    else {
      Syslog(LOG_ERR, "Socket is not open");
    }
    return rc;
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool _asioread(boost::asio::ip::tcp::socket & s, char *buf, uint64_t len)
  {
    bool  rc = false;
    if (s.is_open()) {
      boost::system::error_code  ec;
      if ((read(s, buffer(buf, len), ec) == len) && (! ec)) {
        rc = true;
      }
      else {
        Syslog(LOG_ERR, "Failed to read %llu bytes from socket", len);
      }
    }
    else {
      Syslog(LOG_ERR, "Socket is not open");
    }
    return rc;
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <typename T> void _to_host_byte_order(T & value)
  {
    return;
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <> void _to_host_byte_order(uint16_t & value)
  {
    value = be16toh(value);
    return;
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <> void _to_host_byte_order(uint32_t & value)
  {
    value = be32toh(value);
    return;
  }
    
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <> void _to_host_byte_order(uint64_t & value)
  {
    value = be64toh(value);
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <> void _to_host_byte_order(int16_t & value)
  {
    value = be16toh(value);
    return;
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <> void _to_host_byte_order(int32_t & value)
  {
    value = be32toh(value);
    return;
  }
    
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <> void _to_host_byte_order(int64_t & value)
  {
    value = be64toh(value);
  }
    
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <typename T>
  bool _asio_read_network(boost::asio::ip::tcp::socket & s, T & value)
  {
    bool  rc = _asioread(s, value);
    if (rc) {
      _to_host_byte_order(value);
    }
    return rc;
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <typename T> void _to_net_byte_order(T & value)
  {
    return;
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <> void _to_net_byte_order(uint16_t & value)
  {
    value = htobe16(value);
    return;
  }
    
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <> void _to_net_byte_order(uint32_t & value)
  {
    value = htobe32(value);
    return;
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <> void _to_net_byte_order(uint64_t & value)
  {
    value = htobe64(value);
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <> void _to_net_byte_order(int16_t & value)
  {
    value = htobe16(value);
    return;
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <> void _to_net_byte_order(int32_t & value)
  {
    value = htobe32(value);
    return;
  }
    
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <> void _to_net_byte_order(int64_t & value)
  {
    value = htobe64(value);
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <typename T>
  bool _asiowrite(boost::asio::ip::tcp::socket & s, const T & value)
  {
    bool  rc = false;
    if (s.is_open()) {
      boost::system::error_code  ec;
      if ((write(s, buffer(&value, sizeof(value)), ec) == sizeof(value))
          && (! ec)) {
        rc = true;
      }
      else {
        Syslog(LOG_ERR, "Failed to write %llu bytes to socket",
               sizeof(value));
      }
    }
    else {
      Syslog(LOG_ERR, "Socket is not open");
    }
    return rc;
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool _asiowrite(boost::asio::ip::tcp::socket & s, const char *buf,
                  uint64_t len)
  {
    bool  rc = false;
    if (s.is_open()) {
      uint64_t  wlen = len;
      _to_net_byte_order(wlen);
      if (_asiowrite(s, wlen)) {
        boost::system::error_code  ec;
        if ((write(s, buffer(buf, len), ec) == len) && (! ec)) {
          rc = true;
        }
        else {
          Syslog(LOG_ERR, "Failed to write %llu bytes to socket",
                 len);
        }
      }
      else {
        Syslog(LOG_ERR, "Failed to write length to socket");
      }
    }
    return rc;
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  template <typename T>
  bool _asio_write_network(boost::asio::ip::tcp::socket & s,
                           const T & value)
  {
    T  wvalue = value;
    _to_net_byte_order(wvalue);
    return _asiowrite(s, wvalue);
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, uint8_t & value)
  {
    return _asioread(s, value);
  }
    
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, uint8_t value)
  {
    return _asiowrite(s, value);
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, uint16_t & value)
  {
    return _asio_read_network(s, value);
  }

  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, uint16_t value)
  {
    return _asio_write_network(s, value);
  }
    
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, uint32_t & value)
  {
    return _asio_read_network(s, value);
  }

  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, uint32_t value)
  {
    return _asio_write_network(s, value);
  }
    
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, uint64_t & value)
  {
    return _asio_read_network(s, value);
  }

  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, uint64_t value)
  {
    return _asio_write_network(s, value);
  }
    
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, int8_t & value)
  {
    return _asio_read_network(s, value);
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, int8_t value)
  {
    return _asiowrite(s, value);
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, int16_t & value)
  {
    return _asio_read_network(s, value);
  }
    
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, int16_t value)
  {
    return _asio_write_network(s, value);
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, int32_t & value)
  {
    return _asio_read_network(s, value);
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, int32_t value)
  {
    return _asio_write_network(s, value);
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, int64_t & value)
  {
    return _asio_read_network(s, value);
  }
      
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, int64_t value)
  {
    return _asio_write_network(s, value);
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, std::string & value)
  {
    bool  rc = false;
    value.clear();
    uint32_t  len;
    if (_asio_read_network(s, len)) {
      try {
        value.resize(len);
      }
      catch (...) {
        Syslog(LOG_ERR, "Exception resizing string to %%llu characters",
               len);
        return false;
      }
      if (_asioread(s, value.data(), value.size())) {
        rc = true;
      }
      else {
        Syslog(LOG_ERR, "Failed to read string data from socket");
        value.clear();
      }
    }
    else {
      Syslog(LOG_ERR, "Failed to read string length from socket");
    }
    return rc;
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s,
                   const std::string & value)
  {
    bool  rc = false;
    uint32_t  len = value.size();
    if (_asio_write_network(s, len)) {
      boost::system::error_code  ec;
      rc = (write(s, buffer(value.data(), value.size()), ec) == value.size());
    }
    return rc;
  }
    
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  uint64_t ASIO::StreamedLength(const std::string & s)
  {
    return (sizeof(uint32_t) + s.size());
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, float & val)
  {
    bool  rc = false;
    std::array<char,4>  buf;
    boost::system::error_code  ec;
    if (read(s, buffer(buf.data(), buf.size()), ec) == buf.size()) {
      rc = XDRUtils::Decode(buf, val);
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, float val)
  {
    bool  rc = false;
    std::array<char,4>  buf;
    boost::system::error_code  ec;
    if (XDRUtils::Encode(val, buf)) {
      rc = (write(s, buffer(buf.data(), buf.size()), ec) == buf.size());
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, double & val)
  {
    bool  rc = false;
    std::array<char,8>  buf;
    boost::system::error_code  ec;
    if (read(s, buffer(buf.data(), buf.size()), ec) == buf.size()) {
      rc = XDRUtils::Decode(buf, val);
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, double val)
  {
    bool  rc = false;
    std::array<char,8>  buf;
    boost::system::error_code  ec;
    if (XDRUtils::Encode(val, buf)) {
      rc = (write(s, buffer(buf.data(), buf.size()), ec) == buf.size());
    }
    return rc;
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, ASIOReadable & val)
  {
    return val.Read(s);
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, const ASIOWritable & val)
  {
    return val.Write(s);
  }
  
  //-------------------------------------------------------------------------
  //!  
  //-------------------------------------------------------------------------
  uint64_t ASIO::StreamedLength(const ASIOWritable & val)
  {
    return val.StreamedLength();
  }

}  // namespace Dwm
