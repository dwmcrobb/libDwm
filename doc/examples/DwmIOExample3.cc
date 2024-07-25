#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmStreamIO.hh"

using namespace std;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class PhoneContact
{
public:
  PhoneContact() : _data() { }

  const string & FirstName() const  { return get<0>(_data); }
  const string & FirstName(const string & firstName)
  {
    get<0>(_data) = firstName;
    return get<0>(_data);
  }

  const string & LastName() const  { return get<1>(_data); }
  const string & LastName(const string & lastName)
  {
    get<1>(_data) = lastName;
    return get<1>(_data);
  }

  const set<pair<string,string> > & PhoneNumbers() const
  { return get<2>(_data); }
  bool AddPhoneNumber(const string & phoneName,
                      const string & phoneNumber)
  {
    pair<string,string>  phone(phoneName, phoneNumber);
    return get<2>(_data).insert(phone).second;
  }

  bool RemovePhoneNumber(const string & phoneName,
                         const string & phoneNumber)
  {
    pair<string,string>  phone(phoneName, phoneNumber);
    return (get<2>(_data).erase(phone) == 1);
  }
  
  istream & Read(istream & is) { return Dwm::StreamIO::Read(is, _data); }
  ostream & Write(ostream & os) const { return Dwm::StreamIO::Write(os, _data); }
  ssize_t Read(int fd)             { return Dwm::DescriptorIO::Read(fd, _data); }
  ssize_t Write(int fd) const      { return Dwm::DescriptorIO::Write(fd, _data); }
  size_t Read(FILE *f)             { return Dwm::FileIO::Read(f, _data); }
  size_t Write(FILE *f) const      { return Dwm::FileIO::Write(f, _data); }

private:
  tuple<string,                     // first name
        string,                     // last name
        set<pair<string,string> >   // phone numbers
        > _data;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class PhoneContacts
{
public:
  PhoneContacts() : _contacts() { }

  bool AddContact(const PhoneContact & contact)
  {
    bool  rc = false;
    string  fullName(contact.FirstName() + " " + contact.LastName());
    auto  it = _contacts.find(fullName);
    if (it == _contacts.end()) {
      _contacts[fullName] = contact;
      rc = true;
    }
    return rc;
  }
  
  bool MatchesByEitherName(const string & name,
                           vector<PhoneContact> & matches) const
  {
    if (! matches.empty()) {
      matches.clear();
    }
    for (auto i : _contacts) {
      if ((i.second.FirstName() == name)
          || (i.second.LastName() == name)) {
        matches.push_back(i.second);
      }
    }
    return (! matches.empty());
  }

  bool MatchByFullName(const string & firstName, const string & lastName,
                       PhoneContact & match)
  {
    bool  rc = false;
    for (auto i : _contacts) {
      if ((i.second.FirstName() == firstName)
          && (i.second.LastName() == lastName)) {
        match = i.second;
        rc = true;
        break;
      }
    }
    return rc;
  }

  const map<string,PhoneContact> & Contacts() const  { return _contacts; }
  map<string,PhoneContact> & Contacts() { return _contacts; }
    
  istream & Read(istream & is) { return Dwm::StreamIO::Read(is, _contacts); }
  ostream & Write(ostream & os) const { return Dwm::StreamIO::Write(os, _contacts); }
  ssize_t Read(int fd)             { return Dwm::DescriptorIO::Read(fd, _contacts); }
  ssize_t Write(int fd) const      { return Dwm::DescriptorIO::Write(fd, _contacts); }
  size_t Read(FILE *f)             { return Dwm::FileIO::Read(f, _contacts); }
  size_t Write(FILE *f) const      { return Dwm::FileIO::Write(f, _contacts); }

private:
  map<string,PhoneContact>  _contacts;
};

