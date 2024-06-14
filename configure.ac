AC_INIT([libDwm],[m4_esyscmd_s(./getvers.sh -v)],[dwmcrobb@me.com],[libDwm],[http://www.mcplex.net])
AC_CONFIG_HEADERS([classes/include/DwmPortability.hh])

AC_PROG_CC
AC_PROG_CXX
AC_CANONICAL_TARGET

LT_INIT([])

dnl  ###  Read some generally useful macros.
builtin(include, ./dwm.m4)  dnl
builtin(include, ax_check_compile_flag.m4) dnl

dnl  Set variables for building shared library
DWM_SET_CXX_SHARED_FLAGS

dnl  Set variables for pthreads
DWM_SET_PTHREADFLAGS

OSLIBS=""
LIBTOOL="libtool"

case $host_os in
  freebsd1[[0123]]*)
    CXXFLAGS="-O2"
    LDFLAGS=""
    ;;
  linux*)
    CXXFLAGS="-O2"
    LDFLAGS="$LDFLAGS"
    DEBARCH=`dpkg --print-architecture`
    AC_SUBST(DEBARCH)
    ;;
  darwin*)
    CC="clang++"
    CXX="clang++"
    CXXFLAGS="${CXXFLAGS} -stdlib=libc++"
    LDFLAGS="${LDFLAGS} -g -O2 -stdlib=libc++"
    LIBTOOL="glibtool"
    ;;
esac

echo "host_os ${host_os}"

dnl  Check for C++20
DWM_CHECK_CPLUSPLUS_20

DWM_CHECK_NEED_LIBATOMIC
DWM_CHECK_NEED_LIBRT

AC_SUBST(LIBTOOL)
AC_SUBST(OSLIBS)

dnl  Look for libtermcap
AC_LANG_PUSH(C++)
AC_CHECK_LIB(termcap, tgetent, [TERMCAPLIB=-ltermcap], [exit 1])
AC_LANG_POP()
AC_SUBST(TERMCAPLIB)

dnl  Look for libz
AC_LANG_PUSH(C++)
AC_CHECK_LIB(z, gzwrite, [ZLIB=-lz], [exit 1])
AC_LANG_POP()
AC_SUBST(ZLIB)

CHECK_NLOHMANN_JSON_PKG
CHECK_LIBXXHASH_PKG
CHECK_LIBTIRPC_PKG
CHECK_LIBPCAP_PKG

EXTINCS=`pkg-config --cflags-only-I ${EXTPKGS}`
EXTLIBS=`pkg-config --libs ${EXTPKGS}`

if [[ ${DWM_HAVE_LIBPCAP_PKG} -ne 1 ]]; then
  CHECK_LIBPCAP
fi
CHECK_BZIP2
DWM_CHECK_BOOSTASIO
DWM_CHECK_NEED_LIBIBVERBS

AC_SUBST(EXTINCS)
AC_SUBST(EXTLIBS)

AC_SUBST(LDFLAGS)

DWM_PREREQ_PATH(htmlman,share/htmlman,[HTML manpage path])

DWM_SET_PKGVARS(libDwm,[libDwm-0.0.0])
DWM_GET_TAG([libDwm])

dnl ------------------------------------------------------------------------
dnl //  Check for CoreFoundation/CFByteOrder.h, an OS X thing.  We need
dnl //  this so we can define be64toh() and htobe64() to use
dnl //  CFSwapInt64BigToHost() and CFSwapInt64HostToBig() respectively.
dnl ------------------------------------------------------------------------
AC_CHECK_HEADER(CoreFoundation/CFByteOrder.h, [AC_DEFINE(HAVE_CFBYTE_ORDER_H)])

dnl ------------------------------------------------------------------------
dnl //  Check for byteswap.h, a Linux thing.  We need this so we can define
dnl //  be64toh() and htobe64() to use bswap_64() on Linux running on 
dnl //  little-endian hardware.
dnl ------------------------------------------------------------------------
AC_CHECK_HEADER(byteswap.h, [AC_DEFINE(HAVE_BYTESWAP_H)])
AC_CHECK_HEADER(endian.h, [AC_DEFINE(HAVE_ENDIAN_H)])

dnl ------------------------------------------------------------------------
dnl //  Check for <net/pfvar.h> and <net/if_pflog.h>
dnl ------------------------------------------------------------------------
AC_CHECK_HEADER([net/pfvar.h], [AC_DEFINE(HAVE_NET_PFVAR_H)], [],
                [#include <net/if.h>])
AC_CHECK_HEADER([net/if_pflog.h], [AC_DEFINE(HAVE_NET_IF_PFLOG_H)], [],
                [#include <net/if.h>
		 #ifdef HAVE_NET_PFVAR_H
		 #include <net/pfvar.h>
		 #endif])

AC_CHECK_PROG([MANDOC], [mandoc], [mandoc], [])

case $host_os in
  freebsd1*)  
    DWM_CHECK_SQLITE3()
    AC_SUBST(SQLITEDIR)
    ;;
esac

AC_SUBST(CXXFLAGS)

DWM_CHECK_LIBSTDCPPFS

BUILD_DOCS=""
AC_ARG_ENABLE([docs],[AS_HELP_STRING([--enable-docs],[build documentation])],
	      [BUILD_DOCS="yes"], [])
AC_SUBST(BUILD_DOCS)

AC_CONFIG_FILES([Makefile.vars packaging/libDwm.pc packaging/fbsd_manifest
                 packaging/debcontrol doc/Doxyfile
		 classes/src/DwmPackageInfo.cc])
AC_OUTPUT
