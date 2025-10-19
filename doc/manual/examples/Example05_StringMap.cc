#include <fstream>
#include "DwmStreamIO.hh"

int main(int argc, char *argv[])
{
  using stringmap = std::map<std::string,std::string>;
  std::string  tmpFile("./stringmaptest.tmp");
  stringmap    m1 {
    { "apple",    "fruit"     },
    { "cat",      "feline"    },
    { "broccoli", "vegetable" },
    { "dog",      "canine"    },
    { "kiwi",     "fruit"     },
    { "cow",      "bovine"    }
  };
  bool               success{false};
  std::ofstream      os(tmpFile);
  if (os) {
    Dwm::StreamIO::Write(os, m1);
    os.close();
    std::ifstream  is(tmpFile);
    if (is) {
      stringmap      m2;
      Dwm::StreamIO::Read(is, m2);
      success = (m2 == m1);
      is.close();
    }
    std::remove(tmpFile.c_str());
  }
  return success ? 0 : 1;
}

