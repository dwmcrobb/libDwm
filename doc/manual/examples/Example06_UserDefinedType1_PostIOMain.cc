#include <sstream>
#include "MyType.hh"

//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  bool                 rc{false};
  std::vector<MyType>  myvec1 { {1, "cat"}, {2, "dog" }, { 3, "fish" } };
  std::stringstream    ss;
  if (Dwm::StreamIO::Write(ss, myvec1)) {
    std::vector<MyType>  myvec2;
    if (Dwm::StreamIO::Read(ss, myvec2)) {
      rc = (myvec2 == myvec1);
    }
  }
  return rc ? 0 : 1;
}
