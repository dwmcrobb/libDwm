#include <cassert>
#include <sstream>

#include "DwmStreamIO.hh"

int main(int argc, char *argv[])
{
  using stringmap = std::map<std::string,std::string>;
  stringmap  m1 {
    { "apple",    "fruit"     },
    { "cat",      "feline"    },
    { "broccoli", "vegetable" },
    { "dog",      "canine"    },
    { "kiwi",     "fruit"     },
    { "cow",      "bovine"    }
  };
  
  std::stringstream  ss;
  if (Dwm::StreamIO::Write(ss, m1)) {
    stringmap  m2;
    if (Dwm::StreamIO::Read(ss, m2)) {
      assert(m2 == m1);
    }
  }
}

