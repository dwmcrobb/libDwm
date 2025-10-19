#include <fstream>
#include "DwmStreamIO.hh"

int main(int argc, char *argv[])
{
  bool           success{false};
  std::string    writeString("Hello, world!");
  std::ofstream  os("./stringtest.tmp");
  if (os) {
    Dwm::StreamIO::Write(os, writeString);
    os.close();
    std::ifstream  is("./stringtest.tmp");
    if (is) {
      std::string  readString;
      if (Dwm::StreamIO::Read(is, readString)) {
        success = (readString == writeString);
      }
      is.close();
    }
    std::remove("./stringtest.tmp");
  }
  return success ? 0 : 1;
}

