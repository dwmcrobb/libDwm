include ../Makefile.vars

APPS = dwmcflags dwmnet dwmspath dwmwhat
APPSCLEAN = $(APPS:%=%-clean)
TARPREP   = $(APPS:%=%-tarprep)
DISTCLEAN = $(APPS:%=%-distclean)

.PHONY: all ${APPS}

all:: ${APPS}

${APPS}:: ../classes/lib/libDwm.la
	${MAKE} --no-print-directory -C $@ $(MAKECMDGOALS)

../classes/lib/libDwm.la::
	${MAKE} -s -C ../classes/src

tarprep:: ${TARPREP}
${TARPREP}::
	${MAKE} --no-print-directory -C $(patsubst %-tarprep,%,$@) tarprep

clean:: ${APPSCLEAN}
${APPSCLEAN}::
	${MAKE} --no-print-directory -C $(patsubst %-clean,%,$@) clean

distclean:: ${DISTCLEAN}
${DISTCLEAN}::
	${MAKE} -C $(patsubst %-distclean,%,$@) distclean
