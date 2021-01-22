#include <fstream>
#include <map>
#include <string>

#include "DwmIpv4Address.hh"
#include "DwmStreamIO.hh"

int main(int argc, char *argv[])
{
  std::map<Dwm::Ipv4Address, std::string>  hosts;
  
  //  Read hosts from an ifstream
  std::ifstream  is("/tmp/hosts");
  
  if (is) {
    Dwm::StreamIO::Read(is, hosts);
    is.close();
  }

  //  Do something with hosts, then...
  
  //  Save hosts to an ofstream
  std::ofstream  os("/tmp/hosts");
  if (os) {
    Dwm::StreamIO::Write(os, hosts);
    os.close();
  }

  return 0;
}

 
