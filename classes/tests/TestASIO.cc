//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestASIO.cc 10968 $
// @(#) $Id: TestASIO.cc 10968 2020-08-24 22:45:47Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2018, 2020, 2023
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
//!  \file TestASIO.cc
//!  \author Daniel W. McRobb
//!  \brief  Dwm::ASIO unit tests
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/stat.h>
}

#include <iomanip>
#include <iostream>
#include <thread>
#include <type_traits>

#include "DwmUnitAssert.hh"
#include "DwmASIO.hh"
#include "DwmSysLogger.hh"

using namespace std;

namespace ip = boost::asio::ip;
namespace local = boost::asio::local;
namespace generic = boost::asio::generic;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void ServerReader(std::vector<T> & entries, std::atomic<bool> & ready)
{
  entries.clear();
  boost::asio::io_context  ioContext;
  ip::tcp::acceptor        acc(ioContext,
                               ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 7117),
                               true);
  boost::asio::socket_base::reuse_address option(true);
  acc.set_option(option);
  ip::tcp::socket            sck(ioContext);
  ip::tcp::endpoint          endPoint;
  boost::system::error_code  ec;
  ready = true;
  acc.accept(sck, endPoint, ec);
  if (UnitAssert(! ec)) {
    sck.non_blocking(false);
    T  entry;
    while (Dwm::ASIO::Read(sck, entry, ec)) {
      entries.push_back(entry);
    }
    sck.close();
  }
  acc.close();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void LocalServerReader(std::vector<T> & entries,
                              std::atomic<bool> & ready)
{
  entries.clear();
  unlink("./TestASIO.sock");
  boost::asio::io_context           ioContext;
  local::stream_protocol::acceptor  acc(ioContext, "./TestASIO.sock");
  local::stream_protocol::socket    sck(ioContext);
  boost::system::error_code         ec = {};
  ready = true;
  acc.accept(sck, ec);
  if (UnitAssert(! ec)) {
    T  entry;
    while (Dwm::ASIO::Read(sck, entry, ec)) {
      entries.push_back(entry);
    }
    sck.close();
  }
  acc.close();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename ContainerT>
static void ServerContainerReader(ContainerT & c, std::atomic<bool> & ready)
{
  c.clear();
  boost::asio::io_context  ioContext;
  ip::tcp::acceptor        acc(ioContext,
                               ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 7117),
                               true);
  boost::asio::socket_base::reuse_address option(true);
  acc.set_option(option);
  ip::tcp::socket            sck(ioContext);
  ip::tcp::endpoint          endPoint;
  boost::system::error_code  ec;
  ready = true;
  acc.accept(sck, endPoint, ec);
  if (UnitAssert(! ec)) {
    sck.non_blocking(false);
    Dwm::ASIO::Read(sck, c, ec);
    sck.close();
  }
  acc.close();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename ContainerT>
static void LocalServerContainerReader(ContainerT & c,
                                       std::atomic<bool> & ready)
{
  c.clear();
  unlink("./TestASIO.sock");
  boost::asio::io_context           ioContext;
  local::stream_protocol::acceptor  acc(ioContext, "./TestASIO.sock");
  local::stream_protocol::socket    sck(ioContext);
  boost::system::error_code         ec = {};
  ready = true;
  acc.accept(sck, ec);
  if (UnitAssert(! ec)) {
    Dwm::ASIO::Read(sck, c, ec);
    sck.close();
  }
  acc.close();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename valueT, size_t N>
static void ServerArrayReader(std::array<valueT,N> & c,
                              std::atomic<bool> & ready)
{
  boost::asio::io_context  ioContext;
  ip::tcp::acceptor        acc(ioContext,
                               ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 7117),
                               true);
  boost::asio::socket_base::reuse_address option(true);
  acc.set_option(option);
  ip::tcp::socket            sck(ioContext);
  ip::tcp::endpoint          endPoint;
  boost::system::error_code  ec;
  ready = true;
  acc.accept(sck, endPoint, ec);
  if (UnitAssert(! ec)) {
    sck.non_blocking(false);
    Dwm::ASIO::Read(sck, c, ec);
    sck.close();
  }
  acc.close();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename valueT, size_t N>
static void LocalServerArrayReader(std::array<valueT,N> & c,
                                   std::atomic<bool> & ready)
{
  unlink("./TestASIO.sock");
  boost::asio::io_context           ioContext;
  local::stream_protocol::acceptor  acc(ioContext, "./TestASIO.sock");
  local::stream_protocol::socket    sck(ioContext);
  boost::system::error_code         ec = {};
  ready = true;
  acc.accept(sck, ec);
  if (UnitAssert(! ec)) {
    Dwm::ASIO::Read(sck, c, ec);
    sck.close();
  }
  acc.close();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestVectorOf(const vector<T> & invec)
{
  vector<T>  outvec;
  std::atomic<bool>  serverReady = false;
  std::thread  serverthread = std::thread(ServerReader<T>,
                                          std::ref(outvec),
                                          std::ref(serverReady));
  while (! serverReady) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  
  boost::asio::io_context    ioContext;
  ip::tcp::endpoint          endPoint(ip::address::from_string("127.0.0.1"),
                                      7117);
  ip::tcp::socket            sck(ioContext);
  boost::system::error_code  ec;
  sck.connect(endPoint, ec);
  if (UnitAssert((! ec))) {
    sck.non_blocking(false);
    for (const auto & s : invec) {
      UnitAssert(Dwm::ASIO::Write(sck, s, ec));
    }
    sck.close();
  }
  serverthread.join();
  UnitAssert(invec == outvec);
    
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void LocalTestVectorOf(const vector<T> & invec)
{
  vector<T>          outvec;
  std::atomic<bool>  serverReady = false;
  std::thread        serverthread = std::thread(LocalServerReader<T>,
                                                std::ref(outvec),
                                                std::ref(serverReady));
  while (! serverReady) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  
  boost::asio::io_context           ioContext;
  local::stream_protocol::endpoint  endPoint("./TestASIO.sock");
  local::stream_protocol::socket    sck(ioContext);
  boost::system::error_code         ec;
  sck.connect(endPoint, ec);
  if (UnitAssert((! ec))) {
    for (const auto & s : invec) {
      UnitAssert(Dwm::ASIO::Write(sck, s, ec));
    }
    sck.close();
  }
  serverthread.join();
  unlink("./TestASIO.sock");
  UnitAssert(invec == outvec);
    
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void GenericTestVectorOf(const vector<T> & invec)
{
  vector<T>          outvec;
  std::atomic<bool>  serverReady = false;
  std::thread        serverthread = std::thread(LocalServerReader<T>,
                                                std::ref(outvec),
                                                std::ref(serverReady));
  while (! serverReady) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  
  boost::asio::io_context           ioContext;
  local::stream_protocol::endpoint  endPoint("./TestASIO.sock");
  generic::stream_protocol::socket  sck(ioContext, generic::stream_protocol(AF_UNIX,0));
  boost::system::error_code         ec;
  sck.connect(endPoint, ec);
  if (UnitAssert((! ec))) {
    for (const auto & s : invec) {
      UnitAssert(Dwm::ASIO::Write(sck, s, ec));
    }
    sck.close();
  }
  serverthread.join();
  unlink("./TestASIO.sock");
  UnitAssert(invec == outvec);
    
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename ContainerT>
static void TestContainer(const ContainerT & ct)
{
  ContainerT  outct;
  std::atomic<bool>  serverReady = false;
  std::thread  serverthread = std::thread(ServerContainerReader<ContainerT>,
                                          std::ref(outct),
                                          std::ref(serverReady));
  while (! serverReady) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  
  boost::asio::io_context    ioContext;
  ip::tcp::endpoint          endPoint(ip::address::from_string("127.0.0.1"),
                                      7117);
  ip::tcp::socket            sck(ioContext);
  boost::system::error_code  ec;
  sck.connect(endPoint, ec);
  if (UnitAssert((! ec))) {
    sck.non_blocking(false);
    UnitAssert(Dwm::ASIO::Write(sck, ct, ec));
    sck.close();
  }
  serverthread.join();
  UnitAssert(ct == outct);
    
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename ContainerT>
static void LocalTestContainer(const ContainerT & ct)
{
  ContainerT         outct;
  std::atomic<bool>  serverReady = false;
  std::thread        serverthread =
    std::thread(LocalServerContainerReader<ContainerT>,
                std::ref(outct), std::ref(serverReady));
  while (! serverReady) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  
  boost::asio::io_context           ioContext;
  local::stream_protocol::endpoint  endPoint("./TestASIO.sock");
  local::stream_protocol::socket    sck(ioContext);
  boost::system::error_code         ec;
  sck.connect(endPoint, ec);
  if (UnitAssert((! ec))) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    UnitAssert(Dwm::ASIO::Write(sck, ct, ec));
    sck.close();
  }
  serverthread.join();
  unlink("./TestASIO.sock");
  UnitAssert(ct == outct);
    
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename SocketT, typename EndPointT, typename ContainerT>
static void GenericTestContainer(SocketT & sck, const EndPointT & endPoint,
                                 const ContainerT & ct)
{
  ContainerT         outct;
  std::atomic<bool>  serverReady = false;
  std::thread        serverthread =
    std::thread(LocalServerContainerReader<ContainerT>,
                std::ref(outct), std::ref(serverReady));
  while (! serverReady) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  
  boost::system::error_code         ec;
  sck.connect(endPoint, ec);
  if (UnitAssert((! ec))) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    UnitAssert(Dwm::ASIO::Write(sck, ct, ec));
  }
  serverthread.join();
  UnitAssert(ct == outct);
    
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename valueT, size_t N>
static void TestArray(const std::array<valueT,N> & ct)
{
  std::array<valueT,N>  outct;
  std::atomic<bool>     serverReady = false;
  std::thread  serverthread = std::thread(ServerArrayReader<valueT,N>,
                                          std::ref(outct),
                                          std::ref(serverReady));
  while (! serverReady) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  
  boost::asio::io_context    ioContext;
  ip::tcp::endpoint          endPoint(ip::address::from_string("127.0.0.1"),
                                      7117);
  ip::tcp::socket            sck(ioContext);
  boost::system::error_code  ec;
  sck.connect(endPoint, ec);
  if (UnitAssert((! ec))) {
    sck.non_blocking(false);
    UnitAssert(Dwm::ASIO::Write(sck, ct, ec));
    sck.close();
  }
  serverthread.join();
  UnitAssert(ct == outct);
    
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename valueT, size_t N>
static void LocalTestArray(const std::array<valueT,N> & ct)
{
  std::array<valueT,N>  outct;
  std::atomic<bool>  serverReady = false;
  std::thread  serverthread =
    std::thread(LocalServerArrayReader<valueT,N>,
                std::ref(outct), std::ref(serverReady));
  while (! serverReady) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  
  boost::asio::io_context           ioContext;
  local::stream_protocol::endpoint  endPoint("./TestASIO.sock");
  local::stream_protocol::socket    sck(ioContext);
  boost::system::error_code         ec = {};
  sck.connect(endPoint, ec);
  if (UnitAssert((! ec))) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    UnitAssert(Dwm::ASIO::Write(sck, ct, ec));
    sck.close();
  }
  serverthread.join();
  unlink("./TestASIO.sock");
  UnitAssert(ct == outct);
    
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T, size_t N>
static void TestAsArray(const vector<T> & invec)
{
  array<T,N>  inarr;
  for (size_t i = 0; i < N; ++i) {
    inarr[i] = invec[i];
  }
  TestArray(inarr);
  LocalTestArray(inarr);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestAsVector(const vector<T> & invec)
{
  TestContainer(invec);
  LocalTestContainer(invec);

  boost::asio::io_context           ioContext;
  local::stream_protocol::endpoint  endPoint("./TestASIO.sock");
  generic::stream_protocol::socket  gsck(ioContext,
                                         generic::stream_protocol(AF_UNIX, 0));
  GenericTestContainer(gsck, endPoint, invec);
  gsck.close();
  unlink("./TestASIO.sock");

  local::stream_protocol::socket    lsck(ioContext);
  GenericTestContainer(gsck, endPoint, invec);
  lsck.close();
  unlink("./TestASIO.sock");

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestAsDeque(const vector<T> & invec)
{
  deque<T>  indeq;
  for (const auto & val : invec) {  indeq.push_back(val);  }
  TestContainer(indeq);
  LocalTestContainer(indeq);

  boost::asio::io_context           ioContext;
  local::stream_protocol::endpoint  endPoint("./TestASIO.sock");
  generic::stream_protocol::socket  gsck(ioContext,
                                         generic::stream_protocol(AF_UNIX, 0));
  GenericTestContainer(gsck, endPoint, indeq);
  gsck.close();
  unlink("./TestASIO.sock");

  local::stream_protocol::socket    lsck(ioContext);
  GenericTestContainer(gsck, endPoint, indeq);
  lsck.close();
  unlink("./TestASIO.sock");

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestAsList(const vector<T> & invec)
{
  list<T>  inlist;
  for (const auto & val : invec) {  inlist.push_back(val);  }
  TestContainer(inlist);
  LocalTestContainer(inlist);

  boost::asio::io_context           ioContext;
  local::stream_protocol::endpoint  endPoint("./TestASIO.sock");
  generic::stream_protocol::socket  gsck(ioContext,
                                         generic::stream_protocol(AF_UNIX, 0));
  GenericTestContainer(gsck, endPoint, inlist);
  gsck.close();
  unlink("./TestASIO.sock");

  local::stream_protocol::socket    lsck(ioContext);
  GenericTestContainer(gsck, endPoint, inlist);
  lsck.close();
  unlink("./TestASIO.sock");
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestAsMap(const vector<T> & invec)
{
  map<T,T>  inmap;
  for (const auto & val : invec) {  inmap[val] = val;  }
  TestContainer(inmap);
  LocalTestContainer(inmap);

  boost::asio::io_context           ioContext;
  local::stream_protocol::endpoint  endPoint("./TestASIO.sock");
  generic::stream_protocol::socket  gsck(ioContext,
                                         generic::stream_protocol(AF_UNIX, 0));
  GenericTestContainer(gsck, endPoint, inmap);
  gsck.close();
  unlink("./TestASIO.sock");

  local::stream_protocol::socket    lsck(ioContext);
  GenericTestContainer(gsck, endPoint, inmap);
  lsck.close();
  unlink("./TestASIO.sock");
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestAsMultimap(const vector<T> & invec)
{
  multimap<T,T>  inmap;
  for (const auto & val : invec) {  inmap.insert(pair<T,T>(val,val));  }
  TestContainer(inmap);
  LocalTestContainer(inmap);

  boost::asio::io_context           ioContext;
  local::stream_protocol::endpoint  endPoint("./TestASIO.sock");
  generic::stream_protocol::socket  gsck(ioContext,
                                         generic::stream_protocol(AF_UNIX, 0));
  GenericTestContainer(gsck, endPoint, inmap);
  gsck.close();
  unlink("./TestASIO.sock");

  local::stream_protocol::socket    lsck(ioContext);
  GenericTestContainer(gsck, endPoint, inmap);
  lsck.close();
  unlink("./TestASIO.sock");
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestAsSet(const vector<T> & invec)
{
  set<T>  inSet;
  for (const auto & val : invec) {  inSet.insert(val);  }
  TestContainer(inSet);
  LocalTestContainer(inSet);

  boost::asio::io_context           ioContext;
  local::stream_protocol::endpoint  endPoint("./TestASIO.sock");
  generic::stream_protocol::socket  gsck(ioContext,
                                         generic::stream_protocol(AF_UNIX, 0));
  GenericTestContainer(gsck, endPoint, inSet);
  gsck.close();
  unlink("./TestASIO.sock");

  local::stream_protocol::socket    lsck(ioContext);
  GenericTestContainer(gsck, endPoint, inSet);
  lsck.close();
  unlink("./TestASIO.sock");
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestAsMultiset(const vector<T> & invec)
{
  multiset<T>  inSet;
  for (const auto & val : invec) {  inSet.insert(val);  }
  TestContainer(inSet);
  LocalTestContainer(inSet);

  boost::asio::io_context           ioContext;
  local::stream_protocol::endpoint  endPoint("./TestASIO.sock");
  generic::stream_protocol::socket  gsck(ioContext,
                                         generic::stream_protocol(AF_UNIX, 0));
  GenericTestContainer(gsck, endPoint, inSet);
  gsck.close();
  unlink("./TestASIO.sock");

  local::stream_protocol::socket    lsck(ioContext);
  GenericTestContainer(gsck, endPoint, inSet);
  lsck.close();
  unlink("./TestASIO.sock");
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestAsUnorderedMap(const vector<T> & invec)
{
  unordered_map<T,T>  inmap;
  for (const auto & val : invec) {  inmap[val] = val;  }
  TestContainer(inmap);
  LocalTestContainer(inmap);

  boost::asio::io_context           ioContext;
  local::stream_protocol::endpoint  endPoint("./TestASIO.sock");
  generic::stream_protocol::socket  gsck(ioContext,
                                         generic::stream_protocol(AF_UNIX, 0));
  GenericTestContainer(gsck, endPoint, inmap);
  gsck.close();
  unlink("./TestASIO.sock");

  local::stream_protocol::socket    lsck(ioContext);
  GenericTestContainer(gsck, endPoint, inmap);
  lsck.close();
  unlink("./TestASIO.sock");
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestAsUnorderedSet(const vector<T> & invec)
{
  unordered_set<T>  inSet;
  for (const auto & val : invec) {  inSet.insert(val);  }
  TestContainer(inSet);
  LocalTestContainer(inSet);

  boost::asio::io_context           ioContext;
  local::stream_protocol::endpoint  endPoint("./TestASIO.sock");
  generic::stream_protocol::socket  gsck(ioContext,
                                         generic::stream_protocol(AF_UNIX, 0));
  GenericTestContainer(gsck, endPoint, inSet);
  gsck.close();
  unlink("./TestASIO.sock");

  local::stream_protocol::socket    lsck(ioContext);
  GenericTestContainer(gsck, endPoint, inSet);
  lsck.close();
  unlink("./TestASIO.sock");
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStrings()
{
  vector<string>  vs1({ "one", "two", "three", "four", "five", "six",
                        "seven", "eight", "nine", "ten", "eleven", "twelve",
                        "thirteen", "fourteen", "fifteen", "sixteen",
                        "seventeen", "eighteen", "nineteen", "twenty"});
  TestAsArray<string,20>(vs1);
  TestVectorOf(vs1);
  TestAsVector(vs1);
  TestAsDeque(vs1);
  TestAsList(vs1);
  TestAsMap(vs1);
  TestAsMultimap(vs1);
  TestAsSet(vs1); 
  TestAsMultiset(vs1);
  TestAsUnorderedMap(vs1);
  TestAsUnorderedSet(vs1);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestInts()
{
  vector<int>  vs1({
      1, 10, 100, 1000, 10000, 100000,
      2, 20, 200, 2000, 20000, 200000,
      3, 30, 300, 3000, 30000, 300000
    });
  TestAsArray<int,18>(vs1);
  TestVectorOf(vs1);
  TestAsVector(vs1);
  TestAsDeque(vs1);
  TestAsList(vs1);
  TestAsMap(vs1);
  TestAsMultimap(vs1);
  TestAsSet(vs1);
  TestAsMultiset(vs1);
  TestAsUnorderedMap(vs1);
  TestAsUnorderedSet(vs1);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFloats()
{
  vector<float>  vs1({
      1.1,  10.2,  100.3,  1000.4,  10000.5,  100000.6,
      2.7,  20.8,  200.9,  2000.10, 20000.11, 200000.12,
      3.13, 30.14, 300.15, 3000.16, 30000.17, 300000.18
    });
  TestAsArray<float,18>(vs1);
  TestVectorOf(vs1);
  TestAsVector(vs1);
  TestAsDeque(vs1);
  TestAsList(vs1);
  TestAsMap(vs1);
  TestAsMultimap(vs1);
  TestAsSet(vs1);
  TestAsMultiset(vs1);
  TestAsUnorderedMap(vs1);
  TestAsUnorderedSet(vs1);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDoubles()
{
  vector<double>  vs1({
      1.1, 10.2, 100.3, 1000.4, 10000.5, 100000.6,
      2.7, 20.8, 200.9, 2000.11, 20000.12, 200000.13,
      3.14, 30.15, 300.16, 3000.17, 30000.18, 300000.19
    });
  TestAsArray<double,18>(vs1);
  TestVectorOf(vs1);
  TestAsVector(vs1);
  TestAsDeque(vs1);
  TestAsList(vs1);
  TestAsMap(vs1);
  TestAsMultimap(vs1);
  TestAsSet(vs1);
  TestAsMultiset(vs1);
  TestAsUnorderedMap(vs1);
  TestAsUnorderedSet(vs1);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestPairs()
{
  vector<pair<int,string>>  v1 = {
    { 1, "one" },  { 2, "two" }, { 3, "three" }, { 4, "four" },
    { 5, "five" }, { 6, "six" }, { 7, "seven" }, { 8, "eight" },
    { 9, "nine" }, { 10, "ten" }, { 11, "eleven" }, { 12, "twelve" }
  };
  TestAsArray<pair<int,string>,12>(v1);
  TestVectorOf(v1);
  TestAsVector(v1);
  TestAsDeque(v1);
  TestAsList(v1);
  TestAsMap(v1);
  TestAsMultimap(v1);
  TestAsSet(v1);
  TestAsMultiset(v1);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestTuples()
{
  vector<tuple<int,string,double>>  v1 = {
    { 1, "one", 1.1 },    { 2, "two", 2.2 },       { 3, "three", 3.3 },
    { 4, "four", 4.4 },   { 5, "five", 5.5 },      { 6, "six", 6.6 },
    { 7, "seven", 7.7 },  { 8, "eight", 8.8 },     { 9, "nine", 9.9 },
    { 10, "ten", 10.01 }, { 11, "eleven", 11.11 }, { 12, "twelve", 12.12 }
  };
  TestAsArray<tuple<int,string,double>,12>(v1);
  TestVectorOf(v1);
  TestAsVector(v1);
  TestAsDeque(v1);
  TestAsList(v1);
  TestAsMap(v1);
  TestAsMultimap(v1);
  TestAsSet(v1);
  TestAsMultiset(v1);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestVariants()
{
  vector<variant<int,string,double>>  v1 = {
    { (int)1 }, { "two"   }, { 3.3 },
    { (int)4 }, { "five"  }, { 6.6 },
    { (int)7 }, { "eight" }, { 9.9 }
  };
  TestAsArray<variant<int,string,double>,9>(v1);
  TestVectorOf(v1);
  TestAsVector(v1);
  TestAsDeque(v1);
  TestAsList(v1);
  TestAsMap(v1);
  TestAsMultimap(v1);
  TestAsSet(v1);
  TestAsMultiset(v1);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  //  Dwm::SysLogger::Open("TestASIO", LOG_PERROR, LOG_USER);
  
  TestStrings();
  TestInts();
  TestFloats();
  TestDoubles();
  TestPairs();
  TestTuples();
  TestVariants();

  if (Dwm::Assertions::Total().Failed()) {
    Dwm::Assertions::Print(cerr, true);
  }
  else {
    cout << Dwm::Assertions::Total() << " passed" << endl;
  }
  
}

