//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2018, 2020, 2023, 2024
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
#include "DwmSysLogger.hh"
#include "DwmASIO.hh"
#include "DwmXDRUtils.hh"

namespace Dwm {

  using boost::asio::read, boost::asio::write;
  using boost::asio::buffer;
    
  //-------------------------------------------------------------------------
  template <typename S, typename T>
  requires IsSupportedASIOSocket<S>
  bool _asioread(S & s, T & value, boost::system::error_code & ec)
  {
    bool  rc = false;
    if (s.is_open()) {
      size_t  bytesRead = read(s, buffer(&value, sizeof(value)), ec);
      if ((sizeof(value) == bytesRead) && (! ec)) {
        rc = true;
      }
      else if ((0 == bytesRead)
               && (boost::asio::error::eof == ec.value())) {
        Syslog(LOG_INFO, "EOF on socket");
      }
      else {
        FSyslog(LOG_ERR, "Incomplete read ({} of {} bytes) from socket, '{}'",
                bytesRead, sizeof(value), ec.message());
      }
    }
    else {
      Syslog(LOG_ERR, "Socket is not open");
    }
    return rc;
  }
  
  //-------------------------------------------------------------------------
  template <typename S>
  requires IsSupportedASIOSocket<S>
  bool _asioread(S & s, char *buf, uint64_t len,
                 boost::system::error_code & ec)
  {
    bool  rc = false;
    if (s.is_open()) {
      size_t  bytesRead = read(s, buffer(buf, len), ec);
      if ((bytesRead == len) && (! ec)) {
        rc = true;
      }
      else if ((0 == bytesRead)
               && (boost::asio::error::eof == ec.value())) {
        Syslog(LOG_INFO, "EOF on socket");
      }
      else {
        FSyslog(LOG_ERR, "Failed to read {} bytes from socket", len);
      }
    }
    else {
      Syslog(LOG_ERR, "Socket is not open");
    }
    return rc;
  }
  
  //-------------------------------------------------------------------------
  template <typename T> void _to_host_byte_order(T & value)
  { return; }
  
  //-------------------------------------------------------------------------
  template <> void _to_host_byte_order(uint16_t & value)
  { value = be16toh(value);  return; }
  
  //-------------------------------------------------------------------------
  template <> void _to_host_byte_order(uint32_t & value)
  { value = be32toh(value);  return; }
    
  //-------------------------------------------------------------------------
  template <> void _to_host_byte_order(uint64_t & value)
  { value = be64toh(value);  return; }
  
  //-------------------------------------------------------------------------
  template <> void _to_host_byte_order(int16_t & value)
  { value = be16toh(value);  return; }
  
  //-------------------------------------------------------------------------
  template <> void _to_host_byte_order(int32_t & value)
  { value = be32toh(value);  return; }
    
  //-------------------------------------------------------------------------
  template <> void _to_host_byte_order(int64_t & value)
  { value = be64toh(value); }
    
  //-------------------------------------------------------------------------
  template <typename S, typename T>
  requires IsSupportedASIOSocket<S>
  bool _asio_read_network(S & s, T & value, boost::system::error_code & ec)
  {
    bool  rc = _asioread(s, value, ec);
    if (rc) {
      _to_host_byte_order(value);
    }
    return rc;
  }
  
  //-------------------------------------------------------------------------
  template <typename T> void _to_net_byte_order(T & value)
  { return; }
  
  //-------------------------------------------------------------------------
  template <> void _to_net_byte_order(uint16_t & value)
  { value = htobe16(value);  return; }
    
  //-------------------------------------------------------------------------
  template <> void _to_net_byte_order(uint32_t & value)
  { value = htobe32(value);  return; }
  
  //-------------------------------------------------------------------------
  template <> void _to_net_byte_order(uint64_t & value)
  { value = htobe64(value);  return; }
  
  //-------------------------------------------------------------------------
  template <> void _to_net_byte_order(int16_t & value)
  { value = htobe16(value);  return; }
  
  //-------------------------------------------------------------------------
  template <> void _to_net_byte_order(int32_t & value)
  { value = htobe32(value);  return; }
    
  //-------------------------------------------------------------------------
  template <> void _to_net_byte_order(int64_t & value)
  { value = htobe64(value);  return; }
  
  //-------------------------------------------------------------------------
  template <typename S, typename T>
  bool _asiowrite(S & s, const T & value, boost::system::error_code & ec)
  {
    bool  rc = false;
    if (s.is_open()) {
      if ((write(s, buffer(&value, sizeof(value)), ec) == sizeof(value))
          && (! ec)) {
        rc = true;
      }
      else {
        FSyslog(LOG_ERR, "Failed to write {} bytes to socket", sizeof(value));
      }
    }
    else {
      Syslog(LOG_ERR, "Socket is not open");
    }
    return rc;
  }
  
  //-------------------------------------------------------------------------
  template <typename S>
  bool _asiowrite(S & s, const char *buf, uint64_t len,
                  boost::system::error_code & ec)
  {
    bool  rc = false;
    if (s.is_open()) {
      uint64_t  wlen = len;
      _to_net_byte_order(wlen);
      if (_asiowrite(s, wlen, ec)) {
        if ((write(s, buffer(buf, len), ec) == len) && (! ec)) {
          rc = true;
        }
        else {
          FSyslog(LOG_ERR, "Failed to write {} bytes to socket", len);
        }
      }
      else {
        Syslog(LOG_ERR, "Failed to write length to socket");
      }
    }
    return rc;
  }
  
  //-------------------------------------------------------------------------
  template <typename S, typename T>
  bool _asio_write_network(S & s, const T & value,
                           boost::system::error_code & ec)
  {
    T  wvalue = value;
    _to_net_byte_order(wvalue);
    return _asiowrite(s, wvalue, ec);
  }
  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, char & value,
                  boost::system::error_code & ec)
  { return _asioread(s, value, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, char value,
                   boost::system::error_code & ec)
  { return _asiowrite(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::local::stream_protocol::socket & s,
                  char & value, boost::system::error_code & ec)
  { return _asioread(s, value, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::local::stream_protocol::socket & s,
                   char value, boost::system::error_code & ec)
  { return _asiowrite(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::generic::stream_protocol::socket & s,
                  char & value, boost::system::error_code & ec)
  { return _asioread(s, value, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::generic::stream_protocol::socket & s,
                   char value, boost::system::error_code & ec)
  { return _asiowrite(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, uint8_t & value,
                  boost::system::error_code & ec)
  { return _asioread(s, value, ec); }
    
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, uint8_t value,
                   boost::system::error_code & ec)
  { return _asiowrite(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::local::stream_protocol::socket & s,
                  uint8_t & value, boost::system::error_code & ec)
  { return _asioread(s, value, ec); }
    
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::local::stream_protocol::socket & s,
                   uint8_t value, boost::system::error_code & ec)
  { return _asiowrite(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::generic::stream_protocol::socket & s,
                  uint8_t & value, boost::system::error_code & ec)
  { return _asioread(s, value, ec); }
    
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::generic::stream_protocol::socket & s,
                   uint8_t value, boost::system::error_code & ec)
  { return _asiowrite(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, bool & value,
                  boost::system::error_code & ec)
  {
    uint8_t c;
    if (_asioread(s, c, ec)) {
      value = (c ? true : false);
      return true;
    }
    return false;
  }

  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, bool value,
                   boost::system::error_code & ec)
  { uint8_t c = (value ? 1 : 0);  return _asiowrite(s, c, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::local::stream_protocol::socket & s,
                  bool & value, boost::system::error_code & ec)
  {
    uint8_t c;
    if (_asioread(s, c, ec)) {
      value = (c ? true : false);
      return true;
    }
    return false;
  }
    
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::local::stream_protocol::socket & s,
                   bool value, boost::system::error_code & ec)
  { uint8_t c = (value ? 1 : 0); return _asiowrite(s, c, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::generic::stream_protocol::socket & s,
                  bool & value, boost::system::error_code & ec)
  {
    uint8_t c;
    if (_asioread(s, c, ec)) {
      value = (c ? true : false);
      return true;
    }
    return false;
  }
    
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::generic::stream_protocol::socket & s,
                   bool value, boost::system::error_code & ec)
  { uint8_t c = (value ? 1 : 0); return _asiowrite(s, c, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, uint16_t & value,
                  boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, uint16_t value,
                   boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::local::stream_protocol::socket & s,
                  uint16_t & value, boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::local::stream_protocol::socket & s,
                   uint16_t value, boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::generic::stream_protocol::socket & s,
                  uint16_t & value, boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::generic::stream_protocol::socket & s,
                   uint16_t value, boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, uint32_t & value,
                  boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, uint32_t value,
                   boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::local::stream_protocol::socket & s,
                  uint32_t & value, boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::local::stream_protocol::socket & s,
                   uint32_t value, boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::generic::stream_protocol::socket & s,
                  uint32_t & value, boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::generic::stream_protocol::socket & s,
                   uint32_t value, boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, uint64_t & value,
                  boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, uint64_t value,
                   boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::local::stream_protocol::socket & s,
                  uint64_t & value, boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::local::stream_protocol::socket & s,
                   uint64_t value, boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::generic::stream_protocol::socket & s,
                  uint64_t & value, boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::generic::stream_protocol::socket & s,
                   uint64_t value, boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, int8_t & value,
                  boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, int8_t value,
                   boost::system::error_code & ec)
  { return _asiowrite(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::local::stream_protocol::socket & s,
                  int8_t & value, boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::local::stream_protocol::socket & s,
                   int8_t value, boost::system::error_code & ec)
  { return _asiowrite(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::generic::stream_protocol::socket & s,
                  int8_t & value, boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::generic::stream_protocol::socket & s,
                   int8_t value, boost::system::error_code & ec)
  { return _asiowrite(s, value, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, int16_t & value,
                  boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }
    
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, int16_t value,
                   boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::local::stream_protocol::socket & s,
                  int16_t & value, boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }
    
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::local::stream_protocol::socket & s,
                   int16_t value, boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::generic::stream_protocol::socket & s,
                  int16_t & value, boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }
    
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::generic::stream_protocol::socket & s,
                   int16_t value, boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, int32_t & value,
                  boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, int32_t value,
                   boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::local::stream_protocol::socket & s,
                  int32_t & value, boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::local::stream_protocol::socket & s,
                   int32_t value, boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::generic::stream_protocol::socket & s,
                  int32_t & value, boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::generic::stream_protocol::socket & s,
                   int32_t value, boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, int64_t & value,
                  boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }
      
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, int64_t value,
                   boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::local::stream_protocol::socket & s,
                  int64_t & value, boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }
      
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::local::stream_protocol::socket & s,
                   int64_t value, boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::generic::stream_protocol::socket & s,
                  int64_t & value, boost::system::error_code & ec)
  { return _asio_read_network(s, value, ec); }
      
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::generic::stream_protocol::socket & s,
                   int64_t value, boost::system::error_code & ec)
  { return _asio_write_network(s, value, ec); }
  
  //-------------------------------------------------------------------------
  template <typename S>
  requires IsSupportedASIOSocket<S>
  bool ASIO_Read(S & s, std::string & value, boost::system::error_code & ec)
  {
    bool  rc = false;
    value.clear();
    uint64_t  len;
    if (_asio_read_network(s, len, ec)) {
      try {
        value.resize(len);
      }
      catch (...) {
        FSyslog(LOG_ERR, "Exception resizing string to {} characters", len);
        return false;
      }
      if (_asioread(s, value.data(), value.size(), ec)) {
        rc = true;
      }
      else {
        if (boost::asio::error::eof == ec.value()) {
          Syslog(LOG_INFO, "EOF on socket");
        }
        else {
          Syslog(LOG_ERR, "Failed to read string data from socket");
        }
        value.clear();
      }
    }
    else {
      if (boost::asio::error::eof == ec.value()) {
        Syslog(LOG_INFO, "EOF on socket");
      }
      else {
        Syslog(LOG_ERR, "Failed to read string length from socket");
      }
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  template <typename S>
  requires IsSupportedASIOSocket<S>
  bool ASIO_Write(S & s, const std::string & value,
                  boost::system::error_code & ec)
  {
    bool  rc = false;
    uint64_t  len = value.size();
    if (_asio_write_network(s, len, ec)) {
      rc = (write(s, buffer(value.data(), value.size()), ec) == value.size()
            && (! ec));
    }
    return rc;
  }
  
  //-------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, std::string & value,
                  boost::system::error_code & ec)
  { return ASIO_Read(s, value, ec); }

  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s,
                   const std::string & value, boost::system::error_code & ec)
  { return ASIO_Write(s, value, ec); }

  //--------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::local::stream_protocol::socket & s,
                  std::string & value, boost::system::error_code & ec)
  { return ASIO_Read(s, value, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::local::stream_protocol::socket & s,
                   const std::string & value, boost::system::error_code & ec)
  { return ASIO_Write(s, value, ec); }

  //--------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::generic::stream_protocol::socket & s,
                  std::string & value, boost::system::error_code & ec)
  { return ASIO_Read(s, value, ec); }
  
  //-------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::generic::stream_protocol::socket & s,
                   const std::string & value, boost::system::error_code & ec)
  { return ASIO_Write(s, value, ec); }
  
  //-------------------------------------------------------------------------
  uint64_t ASIO::StreamedLength(const std::string & s)
  {
    return (sizeof(uint64_t) + s.size());
  }

  //--------------------------------------------------------------------------
  template <typename S>
  requires IsSupportedASIOSocket<S>
  bool ASIO_Read(S & s, float & val, boost::system::error_code & ec)
  {
    bool  rc = false;
    std::array<char,4>  buf;
    if ((read(s, buffer(buf.data(), buf.size()), ec) == buf.size())
        && (! ec)) {
      rc = XDRUtils::Decode(buf, val);
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  template <typename S>
  requires IsSupportedASIOSocket<S>
  bool ASIO_Write(S & s, float val, boost::system::error_code & ec)
  {
    bool  rc = false;
    std::array<char,4>  buf;
    if (XDRUtils::Encode(val, buf)) {
      rc = ((write(s, buffer(buf.data(), buf.size()), ec) == buf.size())
            && (! ec));
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, float & val,
                  boost::system::error_code & ec)
  { return ASIO_Read(s, val, ec); }
  
  //--------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, float val,
                   boost::system::error_code & ec)
  { return ASIO_Write(s, val, ec); }

  //--------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::local::stream_protocol::socket & s,
                  float & val, boost::system::error_code & ec)
  { return ASIO_Read(s, val, ec); }
  
  //--------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::local::stream_protocol::socket & s,
                   float val, boost::system::error_code & ec)
  { return ASIO_Write(s, val, ec); }

  //--------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::generic::stream_protocol::socket & s,
                  float & val, boost::system::error_code & ec)
  { return ASIO_Read(s, val, ec); }
  
  //--------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::generic::stream_protocol::socket & s,
                   float val, boost::system::error_code & ec)
  { return ASIO_Write(s, val, ec); }
  
  //--------------------------------------------------------------------------
  template <typename S>
  requires IsSupportedASIOSocket<S>
  bool ASIO_Read(S & s, double & val, boost::system::error_code & ec)
  {
    bool  rc = false;
    std::array<char,8>  buf;
    if ((read(s, buffer(buf.data(), buf.size()), ec) == buf.size())
        && (! ec)) {
      rc = XDRUtils::Decode(buf, val);
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::ip::tcp::socket & s, double & val,
                  boost::system::error_code & ec)
  { return ASIO_Read(s, val, ec); }

  //--------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::local::stream_protocol::socket & s,
                  double & val, boost::system::error_code & ec)
  { return ASIO_Read(s, val, ec); }

  //--------------------------------------------------------------------------
  bool ASIO::Read(boost::asio::generic::stream_protocol::socket & s,
                  double & val, boost::system::error_code & ec)
  { return ASIO_Read(s, val, ec); }
  
  //--------------------------------------------------------------------------
  template <typename S>
  requires IsSupportedASIOSocket<S>
  bool ASIO_Write(S & s, double val, boost::system::error_code & ec)
  {
    bool  rc = false;
    std::array<char,8>  buf;
    if (XDRUtils::Encode(val, buf)) {
      rc = ((write(s, buffer(buf.data(), buf.size()), ec) == buf.size())
            && (! ec));
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::ip::tcp::socket & s, double val,
                   boost::system::error_code & ec)
  { return ASIO_Write(s, val, ec); }

  //--------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::local::stream_protocol::socket & s,
                   double val, boost::system::error_code & ec)
  { return ASIO_Write(s, val, ec); }

  //--------------------------------------------------------------------------
  bool ASIO::Write(boost::asio::generic::stream_protocol::socket & s,
                   double val, boost::system::error_code & ec)
  { return ASIO_Write(s, val, ec); }
  
}  // namespace Dwm


