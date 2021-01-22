extern "C" {
  #include <unistd.h>
}

#include "DwmGroup.hh"
#include "DwmPassword.hh"
#include "DwmUnitAssert.hh"

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  gid_t  mygid = getegid();
  Dwm::Group  mygroup(mygid);
  UnitAssert(mygid == mygroup.Id());

  Dwm::Password  mypasswd(geteuid());
  auto  gmit = std::find(mygroup.Members().begin(), mygroup.Members().end(),
                         mypasswd.UserName());
  UnitAssert(gmit != mygroup.Members().end());

  struct group  *grp = getgrgid(mygid);
  Dwm::Group     setGroup(getgid());
  setGroup.Set(*grp);
  UnitAssert(setGroup.Id() == grp->gr_gid);
  UnitAssert(setGroup.Name() == grp->gr_name);
  UnitAssert(setGroup.Password() == grp->gr_passwd);
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    exit(1);
  }
  else {
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;
  }
  exit(0);
}
