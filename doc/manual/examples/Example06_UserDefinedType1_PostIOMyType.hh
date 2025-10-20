#include "DwmStreamIO.hh"

struct MyType {
  uint32_t     id;
  std::string  name;
  std::string  category;

#if ! defined(DWM_CAN_USE_REFLECTION)
  std::istream & Read(std::istream & is)
  { return Dwm::StreamIO::ReadV(is, id, name, category); }

  std::ostream & Write(std::ostream & os) const
  { return Dwm::StreamIO::WriteV(os, id, name, category); }
#endif
};
