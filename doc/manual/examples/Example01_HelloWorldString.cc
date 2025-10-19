#include <sstream>
#include "DwmStreamIO.hh"

int main(int argc, char *argv[])
{
  using std::string, std::stringstream;
  bool          success{false};
  string        writeString("Hello, world!"), readString;
  stringstream  ss;
  if (Dwm::StreamIO::Write(ss, writeString)) {
    if (Dwm::StreamIO::Read(ss, readString)) {
      success = (readString == writeString);
    }
  }
  return success ? 0 : 1;
}

