#include <sstream>
#include "DwmStreamIO.hh"

int main(int argc, char *argv[])
{
  using container = std::map<std::string,std::vector<std::string>>;
  container  m1 {
    { "fruit",     { "apple", "banana", "kiwi", "orange" }             },
    { "canine",    { "husky", "labrador", "poodle", "wolf", "collie" } },
    { "feline",    { "cougar", "lion", "panther" }                     },
    { "vegetable", { "broccoli", "carrot", "squash" }                  }
  };

  bool               success{false};
  std::stringstream  ss;
  if (Dwm::StreamIO::Write(ss, m1)) {
    container  m2;
    if (Dwm::StreamIO::Read(ss, m2)) {
      success = (m2 == m1);
    }
  }
  return success ? 0 : 1;
}

