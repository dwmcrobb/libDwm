#include <cassert>
#include <sstream>

#include "DwmStreamIO.hh"

int main(int argc, char *argv[])
{
  using container = std::map<std::string,std::vector<std::string>>;
  container  m1 {
    { "fruit",     { "apple", "banana", "kiwi", "orange" } },
    { "canine",    { "husky", "labrador", "poodle" } },
    { "feline",    { "cougar", "lion", "panther" } },
    { "vegetable", { "broccoli", "carrot", "squash" } }
  };
  
  std::stringstream  ss;
  if (Dwm::StreamIO::Write(ss, m1)) {
    container  m2;
    if (Dwm::StreamIO::Read(ss, m2)) {
      assert(m2 == m1);
    }
  }
}

