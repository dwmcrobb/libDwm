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
class MyClass 
{
public:
  //--------------------------------------------------------------------------
  MyClass() = default;
  
  //--------------------------------------------------------------------------
  MyClass(uint32_t u32, uint16_t u16, const std::string & s, uint8_t u8)
      : _u32(u32), _u16(u16), _s(s), _u8(u8)
  {}

  //--------------------------------------------------------------------------
  //!  Write our data into an ostringstream.
  //--------------------------------------------------------------------------
  std::ostream & WriteData(std::ostringstream & oss) const
  { return StreamIO::WriteV(oss, _u32, _u16, _s, _u8); }

  //--------------------------------------------------------------------------
  //!  Read our data from a string by moving it into an istringstream and
  //!  leveraging StreamIO::ReadV()
  //--------------------------------------------------------------------------
  bool ReadData(std::string && s)
  {
    std::istringstream  iss(std::move(s));
    return StreamIO::ReadV(iss, _u32, _u16, _s, _u8) ? true : false;
  }
  
  //--------------------------------------------------------------------------
  //!  Write to an ostringstream and then write a view of that ostringstream
  //!  to the output stream.
  //--------------------------------------------------------------------------
  std::ostream & Write(std::ostream & os) const
  {
    std::ostringstream  oss;
    WriteData(oss);
    std::string_view  ossv(oss.view());
    return StreamIO::Write(os, ossv);
  }

  //--------------------------------------------------------------------------
  //!  Read data into a string, the read our data from the string.
  //--------------------------------------------------------------------------
  std::istream & Read(std::istream & is)
  {
    std::string  s;
    if (StreamIO::Read(is, s)) {
      if (! ReadData(std::move(s))) {
        is.setstate(std::ios_base::failbit);
      }
    }
    return is;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Write(int fd) const
  {
    ssize_t  rc = -1;
    if (0 <= fd) {
      //  To reduce the number of ::write() system calls, write to a
      //  stringstream as a buffer, then write a view of the buffer,
      //  which will call ::write() twice (once for the encoded length,
      //  once for the view data).
      std::ostringstream  oss;
      if (WriteData(oss)) {
        auto  view = oss.view();
        rc = DescriptorIO::Write(fd, view);
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
      if (0 < bytesRead) {
        if (ReadData(std::move(s))) {
          rc = bytesRead;
        }
      }
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  bool operator <=> (const MyClass &) const = default;
  
private:
  uint32_t     _u32;
  uint16_t     _u16;
  std::string  _s;
  uint8_t      _u8;
};

int main(int argc, char *argv[])
{
  const MyClass  c1(0xFF00FF00, 0xBEEF, "hello!", 0xAC);
  std::stringstream  ss;
  assert(StreamIO::Write(ss, c1));
  MyClass  c2(0,0,"",0);
  assert(StreamIO::Read(ss, c2));
  assert(c1 == c2);

#if 1
  int fd = ::open("/tmp/Example07_test", O_CREAT|O_WRONLY, 0644);
  assert(0 <= fd);
  assert(DescriptorIO::Write(fd, c1) > 0);
  close(fd);
  fd = open("/tmp/Example07_test", O_RDONLY);
  assert(0 <= fd);
  MyClass  c3(0,0,"",0);
  assert(DescriptorIO::Read(fd, c3));
  close(fd);
  assert(c1 == c3);
  std::remove("/tmp/Example07_test");
#endif
  
  return 0;
}
