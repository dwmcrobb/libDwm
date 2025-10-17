#include <cassert>
#include <sstream>

#include "DwmStreamIO.hh"

int main(int argc, char *argv[])
{
  std::string  str1("Hello, world!");
  std::stringstream  ss;
  if (Dwm::StreamIO::Write(ss, str1)) {
    std::string  str2;
    if (Dwm::StreamIO::Read(ss, str2)) {
      assert(str2 == str1);
      std::cout << str2 << '\n';
    }
  }
}

