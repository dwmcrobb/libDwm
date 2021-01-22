#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "DwmStreamIO.hh"

using namespace std;

int main(int argc, char *argv[])
{
  if (argc > 2) {
    vector<map<string,set<string>>>  data;

    //  Read data from an ifstream
    ifstream  is(argv[1]);
    if (is) {
      Dwm::StreamIO::Read(is, data);
      is.close();
    }

    //  Do something with the data...

    //  Then save to an ofstream
    ofstream  os(argv[2]);
    if (os) {
      Dwm::StreamIO::Write(os, data);
      os.close();
    }
  }

  return 0;
}
