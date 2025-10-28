#include <sstream>
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

    bool operator <=> (const MyType &) const = default;  // default comparison
};

int main(int argc, char *argv[])
{
    bool                 rc{false};
    std::vector<MyType>  myvec1 {
        {1, "cat", "feline"},
        {2, "dog", "canine" },
        {3, "cow", "bovine" }
    };
    std::stringstream    ss;
    if (Dwm::StreamIO::Write(ss, myvec1)) {
        std::vector<MyType>  myvec2;
        if (Dwm::StreamIO::Read(ss, myvec2)) {
            rc = (myvec2 == myvec1);
        }
    }
    return rc ? 0 : 1;
}
