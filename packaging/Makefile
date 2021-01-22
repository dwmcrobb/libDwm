include ../Makefile.vars

ifeq ("${OSNAME}", "freebsd")
PKGPCDIR = libdata/pkgconfig
else
PKGPCDIR = lib/pkgconfig
endif

TARTARGS = ${TARDIR}/${PKGPCDIR}/libDwm.pc

all::
	echo "nothing to be done for 'all' target."

tarprep: ${TARTARGS}

${TARDIR}/${PKGPCDIR}/libDwm.pc: libDwm.pc
	../install-sh -c -m 444 $< $@

