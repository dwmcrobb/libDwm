/*! \file DwmDocMainPage.hh
 *  \brief Documentation main page (no source code)
 *
 * \mainpage Dwm Class Library
 *
 *  \section intro_sec Introduction
 *
 *  This class library is just a (somewhat random) collection of classes
 *  I've developed over the years for various applications.  Since these
 *  are used in a variety of places, they're collected here to make reuse
 *  easy.
 *
 *  There are a few classes herein that I've used on nearly every
 *  software project in the last 20 years.  One of those would be the
 *  Dwm::SysLogger class with the Syslog() macro, since syslogging is a
 *  common need.  Another would be the Dwm::Assertions class with the
 *  UnitAssert() macro since I almost always write unit tests as part of
 *  my development process.  And when I need to store data, I use the
 *  IO classes along with the various Readable and Writable
 *  interfaces.
 *
 *  Since much of the software I've written in the last 20 years has
 *  been multithreaded, I have also made heavy use of the
 *  Dwm::Thread::Queue class for inter-thread communication (usually
 *  as 'work queues').
 *
 *  \section history_sec History
 *
 *  I started this library in 1998.  I continue to maintain it for my
 *  personal use.  While it has grown over the years, the most common
 *  use features have been stable for a long time.  To some extent they
 *  have to be stable; I have over 500,000 lines of code in my personal
 *  software repository, and a decent amount of it is dependent on this
 *  library.  The good news is that those dependencies don't prevent me
 *  from refactoring, improving and growing libDwm as needed.
 *
 *  \section platforms_sec Platforms
 *
 *  I really only maintain support for 4 platforms: FreeBSD, macOS,
 *  desktop linux and Raspberry Pi OS.
 *  FreeBSD is my operating system of choice for servers and macOS is my
 *  operating system of choice for desktops and laptops.  I have
 *  several Raspberry Pis I utilize for simple small tasks, and Ubuntu
 *  VMs and Ubuntu workstations.
 *
 *  \tableofcontents
 *
 *  \section smattering_sec A small sample of what's in libDwm
 *
 *  \subsection dwm_io_sec I/O - Serialization
 *
 *  Dwm::DescriptorIO, Dwm::FileIO, Dwm::StreamIO, Dwm::ASIO, Dwm::GZIO
 *  and Dwm::BZ2IO provide serialization for intrinsic types, containers of
 *  intrinsic types, user types which implement the required interfaces,
 *  and containers of these user types.
 *
 *  Intrinsic integer types are written in network byte order.  The
 *  library takes care of byte ordering so you need not be concerned
 *  with it.  Float and double types are written in IEEE 754-1985 form
 *  (see RFC 1832).  Some types are stored as (length,value)
 *  pairs, including the standard containers and C++ string objects.
 *
 *  For user types, the required interfaces are *Readable and *Writable,
 *  such as Dwm::StreamReadable and Dwm::StreamWritable.  The required
 *  interfaces for Dwm::GZIO are Dwm::GZReadable and Dwm::GZWritable.
 *  The required interfaces for Dwm::BZ2IO are Dwm::BZ2Readable and
 *  Dwm::BZ2Writable.
 *
 *  Note that inheritance from the aforementioned interface classes is
 *  no longer required, and is in fact discouraged.  Today, the Dwm::*IO
 *  classes use concepts instead.  You'll see these concepts at the top
 *  of the various *IOCapable.hh header files, which just express the
 *  same interface requirements as the pure virtual classes.  This allows
 *  the library user to avoid inheriting from the pure virtual classes.
 *
 *  I wrote most of this functionality in 2004 when we didn't
 *  have a lot of options to support serialization. I finalized most of
 *  it in 2007 with the addition of cleaner code for std::tuple.  I
 *  couldn't do this effectively until the compilers and standard libraries
 *  were up to speed.  With C++11 and beyond, I was able to simplify some
 *  of the code thanks to things like variadic templates.  Given that
 *  we now have C++23 and still no reflection, this work has paid big
 *  dividends for me.
 *
 *  Why did I spend time on this functionality?  I frequently need to do
 *  things like this (error checking absent for the sake of brevity):
 *
 *  \includelineno DwmIOExample1.cc
 *
 *  While this is a trivial example, I've used this functionality in
 *  much more complex scenarios.  Having this type of functionality in
 *  my toolbox has saved me large amounts of development time.  It's
 *  reasonably flexible without a lot of extra work.  No work at all if
 *  you're using types already supported.  Containers of containers of
 *  supported types, for example:
 *
 *  \includelineno DwmIOExample2.cc
 *
 *  Since the library handles C++ &lt;tuple&gt;, you can create new data
 *  classes and easily add serialization by keeping all of your class's
 *  data in a tuple.  Below is a trivial contrived example of a phone
 *  contact data store.  The serialization members required
 *  are lines 44 to 49 and 110 to 115.  These are easy to implement,
 *  since they each require only a single call to a member of an I/O class.
 *  Note that I never put <tt style="color: #0DD;"> &nbsp;using
 *  namespace std </tt> in header files.  It's here just to reduce
 *  clutter on your screen.
 *
 *  \includelineno DwmIOExample3.cc
 *
 *  The astute observer will notice that the PhoneContact class is just
 *  a wrapper around a <tt>std::tuple</tt>, to avoid having to use std::get<>()
 *  directly from application code outside of this class.  If you're
 *  willing to deal directly with std::get<>(), you can just do this:
 *
 *  @code typedef std::tuple<std::string,std::string,std::pair<std::string,std::string> > >  PhoneContact; @endcode
 *  @code typedef std::map<std::string,PhoneContact>  PhoneContacts; @endcode
 *
 *  \subsection dwm_unitassert_sec UnitAssert - A trivial unit testing framework
 *
 *  In DwmUnitAssert.hh you will find the UnitAssert() macro and a handful
 *  of support classes for unit testing.  This trivial framework makes 
 *  simple unit testing easy to accomplish for all exposed interfaces.
 *  This framework is used for all of the unit tests in the <tt>tests</tt>
 *  subdirectory of the libDwm source distribution, and you can look there
 *  for many examples.  But its basic usage is trivial and typically boils
 *  down to just three calls: the UnitAssert() macro to assert conditions
 *  that must be true (just like <tt>assert()</tt> from the standard C 
 *  library), and a call to Dwm::Assertions::Print() at the end of your test
 *  program that is typically wrapped in test of the return of
 *  Dwm::Assertions::Total().Failed().
 *
 *  Below is the entire contents of TestGroup.cc from the <tt>tests</tt>
 *  directory of the libDwm source distribution.  You will see calls to
 *  UnitAssert() on lines 16, 21, 26, 27 and 28.  On line 30, we check
 *  if there were any failed tests via Dwm::Assertions::Total().Failed().
 *  If there were, we print them with Dwm::Assertions::Print().  If there
 *  were no failed tests, we print the total number of tests (which all
 *  passed).
 *
 *  \includelineno UnitAssertExample1.cc 
 *
 *  \subsection dwm_sylogger_sec Dwm::SysLogger - decorated syslogging
 *
 *  The intent of Dwm::SysLogger and the Syslog() macro is to allow the
 *  automatic addition of filename, line number and syslog priority to
 *  syslog messages.  No one in their right mind wants to have to add
 *  <tt>"(%s:%d)", \_\_FILE\_\_, \_\_LINE\_\_</tt> to every syslog call
 *  in their source code.  It's inane work and prone to mistakes.
 *  Syslog() can do this for you if you enable it with
 *  Dwm::SysLogger::ShowFileLocation(bool).  It can also put
 *  three-character tags on each log message if you enable it with
 *  Dwm::SysLogger::ShowPriorities(bool).  You can also change the minimum
 *  priority of messages to be logged via
 *  Dwm::SysLogger::MinimumPriority(int) or
 *  Dwm::SysLogger::MinimumPriority(const std::string &).  This is handy
 *  when you want to toggle debug logging while a program is running,
 *  perhaps via a signal.
 *
*/
