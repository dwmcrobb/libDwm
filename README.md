# libDwm
This class library is just a (somewhat random) collection of classes
I've developed over the years for various applications. Since these
are used in a variety of places, they're collected here to make reuse
easy.  It is somewhat of a kitchen sink and contains some old cruft
I no longer use, but the parts mentioned on this page are heavily
used in my own code and here to stay.  It is likely that at some
point I will refactor other parts and remove a good chunk of classes
that I no longer use.

## History
The library started with a focus on portable binary I/O.  Namely storing
data in network byte order, as well as sending and receiving it over the
network.  At the time I chose network byte order because the workstations
and servers I used at the time had big-endian architectures.  If I were
making the same decision today, I'd likely choose little-endian just due
to the prevalence of little-endian computers in our lives.

## Portable Binary I/O
There are classes in the library which provide static functions and
function templates for binary I/O for fundamental types as well as
strings.  They also provide I/O for most of the containers in the C++
standard library: array, deque, list, map, pair, set, tuple,
unordered_map, unordered_set, variant and vector.  A big part of my
motivation after C++03 was the lack of reflection or introspection
in C++.  As it turns out, in 2024 we still don't have reflection,
hence my technique has been useful for much longer than I expected.
Having read the proposals still active for C++26, I suspect I'm
going to continue using my technique even after we have reflection.
At this point the inertia in my own code is significant.  The last
proposal I read was
[P2996R2](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p2996r2.html).  There's been a lot of good, hard work here and in other papers.  But
using what's been proposed to replace what I have would be a very
large effort in my own code simply because I have a lot of code using
my existing technique.  If we get reflection in C++26, I'll experiment
with a new library that uses it.

The classes that implement the I/O:

- [Dwm::StreamIO](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1StreamIO.html) - read from and write to iostreams
- [Dwm::FileIO](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1FileIO.html) - read from and write to FILE handles
- [Dwm::DescriptorIO](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1DescriptorIO.html) - read from and write to UNIX descriptors
- [Dwm::ASIO](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1ASIO.html) - read from and write to boost asio sockets
- [Dwm::BZ2IO](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1BZ2IO.html) - read from and write to BZFILE handles (bzip2)
- [Dwm::GZIO](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1GZIO.html) - read from and write to gzFile handles (gzip)

Note that I use length encoding for strings and containers.  Length
fields are always uint64_t.  Hence there is an 8-byte I/O overhead for
strings and most containers, to hold the number of elements.

For bzip2 and gzip, the library depends on libbz2 and zlib
(respectively).  It is a shame that the bzip2 library and zlib library
use `int` for return values, in the sense that you can't sanely read
or write anything larger than 2,147,483,647 (2 gigabytes) on platforms
where `int` is 32 bits (which is nearly all 32-bit and 64-bit
systems).  However, I didn't want to maintain my own forks of these
libraries, so it's just something to be aware of when using
[Dwm::BZ2IO](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1BZ2IO.html)
and 
[Dwm::GZIO](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1GZIO.html).

#### Using the I/O classes with your own types
To leverage the library I/O classes for containers of your own types,
you simply need to add public members to your class to read and write
from the sources/sinks of interest.  For a long time I used
inheritance from pure virtual classes, but today I'm migrating away
from it since what's needed can be expressed cleanly with concepts in
C++20.  I could have done this with function templates and SFINAE long
ago, but resisted since it's difficult for non-experts to read and can
be difficult to maintain.  Most C++ programmers understand pure
virtual classes and inheritance.  The downside is that it leads to
multiple inheritance, which creates headaches for some modern C++
features like defaulted comparison operators.  Now that we have
readable concepts for simple tasks, I can remove the inheritance
requirement and one day remove these virtual classes (which I now
consider deprecated but haven't yet marked them as such):

- DescriptorIOCapable, DescriptorReadable and DescriptorWritable
- FileIOCapable, FileReadable and FileWritable
- StreamIOCapable, StreamReadable and StreamWritable
- BZ2IOCapable, BZ2Readable and BZ2Writable
- GZIOCapable, GZReadable and GZWritable
- ASIOCapable, ASIOReadable and ASIOWritable

For [Dwm::StreamIO](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1StreamIO.html),
you'd add these public members to your class:

    //------------------------------------------------------------------
    //!  Reads from an istream.  Returns the istream.
    //------------------------------------------------------------------
    std::istream & Read(std::istream & is);
    //------------------------------------------------------------------
    //!  Writes to an ostream.  Returns the ostream.
    //------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const;

For [Dwm::FileIO](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1FileIO.html), you'd add these public members to your class:

    //------------------------------------------------------------------
    //!  Reads from a FILE pointer.  Returns 1 on success, 0 on
	//!  failure.
    //------------------------------------------------------------------
    size_t Read(FILE * f);
    //------------------------------------------------------------------
    //!  Writes to a FILE pointer.  Returns 1 on success, 0 on
	//!  failure.
    //------------------------------------------------------------------
    size_t Write(FILE * f) const

For [DescriptorIO](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1DescriptorIO.html) you'd add these public members to your class:

    //------------------------------------------------------------------
    //!  Reads from a file descriptor.  Returns the number of bytes
	//!  read on success, -1 on failure.
    //------------------------------------------------------------------
    ssize_t Read(int fd);
    //------------------------------------------------------------------
    //!  Writes to a file descriptor.  Returns the number of bytes
	//!  written on success, -1 on failure.
    //------------------------------------------------------------------
    ssize_t Write(int fd) const;

For [Dwm::BZ2IO](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1BZ2IO.html) you'd add these public members to your class:

    //------------------------------------------------------------------
    //!  Reads from a BZFILE pointer.  Returns the number of bytes
    //!  read on success, -1 on failure.
    //------------------------------------------------------------------
    int BZRead(BZFILE *bzf);
    //------------------------------------------------------------------
    //!  Writes to a BZFILE pointer.  Returns the number of bytes
	//!  written on success, -1 on failure.
    //------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const;

For [Dwm::GZIO](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1GZIO.html)
you'd add these public members to your class:

    //------------------------------------------------------------------
    //!  Reads from a gzFile.  Returns the number of bytes read
    //!  on success, -1 on failure.
    //------------------------------------------------------------------
    int Read(gzFile gzf);
    //------------------------------------------------------------------
    //!  Writes to a gzFile.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------
    int Write(gzFile gzf) const;

For [Dwm::ASIO](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1ASIO.html)
you'd add these public members to your class:

    //------------------------------------------------------------------
    //!  Reads from a boost::asio::ip::tcp::socket.  Returns true on 
	//!  success.  Returns false and sets @c ec on failure.
    //------------------------------------------------------------------
    bool Read(boost::asio::ip::tcp::socket & s,
              boost::system::error_code & ec);
    //------------------------------------------------------------------
    //!  Writes to a boost::asio::ip::tcp::socket.  Returns true on
	//!  success.  Returns false and sets @c ec on failure.
    //------------------------------------------------------------------
    bool Write(boost::asio::ip::tcp::socket & s,
               boost::system::error_code & ec) const;
    //------------------------------------------------------------------
    //!  Reads from a boost::asio::local::stream_protocol::socket.
	//!  Returns true on success.  Returns false and sets @c ec on 
	//!  failure.
    //------------------------------------------------------------------
    bool Read(boost::asio::local::stream_protocol::socket & s,
              boost::system::error_code & ec);
    //------------------------------------------------------------------
    //!  Writes to a boost::asio::local::stream_protocol::socket.
	//!  Returns true on success.  Returns false and sets @c ec on 
	//!  failure.
    //------------------------------------------------------------------
    bool Write(boost::asio::local::stream_protocol::socket & s,
               boost::system::error_code & ec) const;
    //------------------------------------------------------------------------
    //!  Reads from a boost::asio::generic::stream_protocol::socket.  Returns
    //!  true on success.  Returns false and sets @c ec on failure.
    //------------------------------------------------------------------------
    bool Read(boost::asio::generic::stream_protocol::socket & s,
              boost::system::error_code & ec);
    //------------------------------------------------------------------------
    //!  Writes to a boost::asio::generic::stream_protocol::socket.  Returns
    //!  true on success.  Returns false and sets @c ec on failure.
    //------------------------------------------------------------------------
    bool Write(boost::asio::generic::stream_protocol::socket & s,
               boost::system::error_code & ec) const;
	
## SysLogger
The [SysLogger](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1SysLogger.html)
class wraps `syslog()` functionality in a singleton class.  A 
[Syslog()](https://www.mcplex.net/Software/Documentation/libDwm/DwmSysLogger_8hh.html)
macro is provided which will pass `__FILE__` and `__LINE__` along
automatically and they will be shown in syslog output if
[SysLogger::ShowFileLocation()](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1SysLogger.html)
is set to true.  This is really just a convenience that alleviated me of
having to write `"... {%s:%d}", ... __FILE__,__LINE__` every time I wrote a
syslog statement.  The
[SysLogger](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1SysLogger.html)
class also encodes the priority in a 3-letter string, which
is convenient.  For example, `[E]` for something logged at `LOG_ERR`
level.

## UnitAssert: simple unit testing
When I started this library in 1998, we didn't have a lot of great
options for C++ unit testing.  Kent Beck's
[Extreme Programming Explained](https://www.amazon.com/Extreme-Programming-Explained-Embrace-Change/dp/0201616416/)
wasn't published until October 1999.
[CppUnit](https://www.freedesktop.org/wiki/Software/cppunit/) didn't
show up until 2000, and didn't do what I wanted for my unit testing.

What did I want?  I basically just wanted something like the standard
`assert()` macro, but to not trigger `abort()` and instead keep track of
the issue for later reporting.  I also wanted my macro to return `true`
or `false`, allowing me to skip further processing without throwing an
exception or aborting.  The whole idea here is something trivially
simple to use that lets me just write test code in the same manner as
non-test code, just with a liberal use of
[UnitAssert()](https://www.mcplex.net/Software/Documentation/libDwm/DwmUnitAssert_8hh.html)
calls to verify correct operation.  One macro to know and love, one
small set of functions to print results and set test program exit code.

For examples, all of the unit tests in the `classes/tests` directory use
[UnitAssert()](https://www.mcplex.net/Software/Documentation/libDwm/DwmUnitAssert_8hh.html)
and the members of the
[Assertions](https://www.mcplex.net/Software/Documentation/libDwm/classDwm_1_1Assertions.html)
class.

## Platforms
FreeBSD, linux (Debian-based systems including Ubuntu and Raspberry Pi OS)
and macOS.

Note that on macOS, I am using MacPorts for dependencies.  Homebrew
will probably work, I've just always been a MacPorts user.

## Dependencies
### Tools
#### C++ Compiler
A C++20 compiler is required.  I'm using these at the time of writing:
- FreeBSD: clang++ 18.1.4
- Ubuntu 24.04 LTS: g++ 13.2.0
- macOS: Apple clang version 15.0.0 (clang-1500.3.9.4)
- Raspberry Pi OS 12(bookworm): g++ 12.2.0
#### GNU make
- FreeBSD: `sudo pkg install gmake`
- Linux: `sudo apt install make`
#### GNU flex
- FreeBSD: `sudo pkg install flex`
- Linux: `sudo apt install flex`
#### GNU bison
- FreeBSD: `sudo pkg install bison`
- Linux: `sudo apt install bison`
- macOS: `sudo port install bison`
#### [mkfbsdmnfst](https://github.com/dwmcrobb/mkfbsdmnfst)
Needed to build a package on FreeBSD.
#### [mkdebcontrol](https://github.com/dwmcrobb/mkdebcontrol)
Needed to build a package on Debian, Ubuntu and Raspberry Pi OS.
### Libraries
#### nlohmann_json
- FreeBSD: `sudo pkg install nlohmann-json`
- Linux: `sudo apt install nlohmann-json3-dev`
- macOS: `sudo port install nlohmann-json`
#### libxxhash
- FreeBSD: `sudo pkg install xxhash`
- Linux: `sudo apt install libxxhash-dev`
- macOS: `sudo port install xxhashlib`
#### libncurses (needed for termcap on Linux)
- Linux: `sudo apt install libncurses-dev`
#### libpcap
- Linux: `sudo apt install libpcap-dev`
- macOS: `sudo port install libpcap`
#### boost_iostreams
- FreeBSD: `sudo pkg install boost-libs`
- Linux: `sudo apt install libboost1.83-all-dev`
- macOS: `sudo port install boost181`
#### libtirpc (needed on Linux for XDR routines)
- Linux: `sudo apt install libtirpc-dev`

## Build
The build requires GNU make (hence on FreeBSD, the make command below
should be `gmake`).  It also requires GNU flex and GNU bison.

    ./configure
    make

## Build a native package
I normally build a native package to allow me to install the library
using the native packaging tools on FreeBSD, Debian-based Linux systems
and macOS.  This is done with:

    make package

Note that if you want documentation to be included in the build, you
must use:

    make BUILD_DOCS=yes package

## Installation
Once a package is built, it may be installed using the native installation
tools on your platform.
#### FreeBSD
    pkg install libDwm-0.9.40.pkg
#### Linux (Debian-based systems)
    dpkg -i libDwm_0.9.40_amd64.deb
#### macOS
    open libDwm-0.9.40.pkg

#### Other options
You can stage all of the installation files without creating a package
by running:

    make tarprep
	
This will place all of the files for installation in the `staging/usr/local`
directory.  The top level `tarprep` make target is a dependency of the
`package` target.

## Documentation
Library documentation is available at [www.mcplex.net/Software/Documentation/libDwm](https://www.mcplex.net/Software/Documentation/libDwm/) and may be built in the `doc` directory.
