include ../../Makefile.vars

CXXFLAGS   += -g
INCS        = -I../../classes/include
OBJFILES    = dwmcflags.o
OBJDEPS	    = $(OBJFILES:%.o=deps/%_deps)
TARTARGETS  = ${TARDIR}/bin/dwmcflags \
	      ${TARDIR}/man/man1/dwmcflags.1
ifeq ("${MANDOC}", "mandoc")
MANHTML = ${MANDOC} -Thtml -Ostyle=../mcplexman.css -Oman=../html%S/%N.%S.html
TARTARGETS += ${TARDIR}/${HTMLMAN}/html1/dwmcflags.1.html
endif

dwmcflags: ${OBJFILES} ../../classes/lib/libDwm.la
	${LIBTOOL} --mode=link --tag=CXX ${CXX} ${PTHREADLDFLAGS} ${LDFLAGS} -rpath ${INSTALLPREFIX}/lib -o $@ $^ ${ZLIB} ${BZLIB} ${BOOSTLIBS} ${TERMCAPLIB} -lpcap ${TIRPCLIB} ${OSLIBS}

tarprep: ${TARTARGETS}

${TARDIR}/bin/dwmcflags: dwmcflags
	${LIBTOOL} --mode=install ../../install-sh -s -c -m 555 $< $@

${TARDIR}/man/man1/dwmcflags.1: dwmcflags.1
	../../install-sh -c -m 644 $< $@

${TARDIR}/${HTMLMAN}/html1/dwmcflags.1.html: dwmcflags.1
	${MANHTML} $< > $<.html
	../../install-sh -c -m 644 $<.html $@
	rm $<.html

#  dependency rule
deps/%_deps: %.cc
	@echo "making dependencies for $<"
	@set -e; \
	${CXX} -MM ${CXXFLAGS} ${PTHREADCXXFLAGS} ${INCS} -c $< | \
	 sed 's/\($*\)\.o[ :]*/\1.o $(@D)\/$(@F) : /g' > $@ ; [ -s $@ ] || \
	 rm -f $@

#  only include dependency makefiles if target is not 'clean'
ifneq ($(MAKECMDGOALS),clean)
-include ${OBJDEPS}
endif

%.o: %.cc
	${CXX} ${CXXFLAGS} ${INCS} -c $<

../../classes/lib/libDwm.la::
	${MAKE} -C ../../classes/src

clean:: clean-tarprep
	${LIBTOOL} --mode=clean rm -f dwmcflags ${OBJFILES}

clean-tarprep::
	${LIBTOOL} --mode=uninstall rm -f ${TARDIR}/bin/dwmcflags
