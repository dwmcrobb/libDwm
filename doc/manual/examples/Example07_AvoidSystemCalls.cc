extern "C" {
  #include <fcntl.h>   // for open()
}

#include <sstream>

#include "DwmDescriptorIO.hh"
#include "DwmIOUtils.hh"
#include "DwmStreamIO.hh"
#include "DwmEncodedUnsigned.hh"
#include "DwmEncodedSigned.hh"

using Dwm::StreamIO, Dwm::DescriptorIO, Dwm::IOUtils, Dwm::EncodedU64;

//----------------------------------------------------------------------------
class C 
{
public:
  //--------------------------------------------------------------------------
  C() = default;
  
  //--------------------------------------------------------------------------
  C(uint32_t u32, uint16_t u16, const std::string & s, uint8_t u8)
      : _u32(u32), _u16(u16), _s(s), _u8(u8)
  {}

  //--------------------------------------------------------------------------
  std::ostream & Write(std::ostream & os) const
  {
    EncodedU64  len = IOUtils::StreamedLengthV(_u32, _u16, _s, _u8);
    if (StreamIO::Write(os, len)) {
      StreamIO::WriteV(os, _u32, _u16, _s, _u8);
    }
    return os;
  }

  //--------------------------------------------------------------------------
  std::istream & Read(std::istream & is)
  {
    std::string  s;
    if (StreamIO::Read(is, s)) {
      assert(! s.empty());
      std::istringstream  iss(std::move(s));
      assert(s.empty());
      StreamIO::ReadV(iss, _u32, _u16, _s, _u8);
    }
    return is;
  }

  //--------------------------------------------------------------------------
  ssize_t Write(int fd) const
  {
    ssize_t  rc = -1;
    if (0 <= fd) {
      std::stringstream  ss;
      if (Write(ss)) {
        auto  view = ss.view();
        rc = ::write(fd, view.data(), view.size());
      }
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  ssize_t Read(int fd)
  {
    ssize_t  rc = -1;
    if (0 <= fd) {
      std::string  s;
      ssize_t  bytesRead = DescriptorIO::Read(fd, s);
      if (bytesRead > 0) {
        std::istringstream  iss(std::move(s));
        if (StreamIO::ReadV(iss, _u32, _u16, _s, _u8)) {
          rc = bytesRead;
        }
      }
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  bool operator <=> (const C &) const = default;
  
private:
  uint32_t     _u32;
  uint16_t     _u16;
  std::string  _s;
  uint8_t      _u8;
};

int main(int argc, char *argv[])
{
  const C  c1(0xFF00FF00, 0xBEEF, "hello!", 0xAC);
  std::stringstream  ss;
  assert(StreamIO::Write(ss, c1));
  C  c2(0,0,"",0);
  assert(StreamIO::Read(ss, c2));
  assert(c1 == c2);

#if 1
  int fd = ::open("/tmp/Example07_test", O_CREAT|O_WRONLY, 0644);
  assert(0 <= fd);
  assert(DescriptorIO::Write(fd, c1) > 0);
  close(fd);
  fd = open("/tmp/Example07_test", O_RDONLY);
  assert(0 <= fd);
  C  c3(0,0,"",0);
  assert(DescriptorIO::Read(fd, c3));
  close(fd);
  assert(c1 == c3);
  std::remove("/tmp/Example07_test");
#endif
  
  return 0;
}
