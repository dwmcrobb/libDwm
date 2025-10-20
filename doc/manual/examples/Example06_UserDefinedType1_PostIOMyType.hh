#include "DwmStreamIO.hh"

struct MyType {
  uint32_t     id;
  std::string  name;
  std::string  category;
  
  std::istream & Read(std::istream & is)
  { return Dwm::StreamIO::ReadV(is, id, name); }

  std::ostream & Write(std::ostream & os) const
  { return Dwm::StreamIO::WriteV(os, id, name); }
};
