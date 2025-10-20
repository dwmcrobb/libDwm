#include <sstream>
#include "DwmStreamIO.hh"

//----------------------------------------------------------------------------
struct MyType {
  uint32_t     id;
  std::string  name;

  std::istream & Read(std::istream & is)
  { return Dwm::StreamIO::ReadV(is, id, name); }

  std::ostream & Write(std::ostream & os) const
  { return Dwm::StreamIO::WriteV(os, id, name); }

  bool operator <=> (const MyType &) const = default;
};

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
