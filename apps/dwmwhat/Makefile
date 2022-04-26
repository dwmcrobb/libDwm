include ../../Makefile.vars

CXXFLAGS   += -g
INCS        = -I../../classes/include
OBJFILES    = dwmwhat.o
OBJDEPS	    = $(OBJFILES:%.o=deps/%_deps)
TARTARGETS  = ${TARDIR}/bin/dwmwhat \
	      ${TARDIR}/man/man1/dwmwhat.1
ifeq ("${MANDOC}", "mandoc")
MANHTML = ${MANDOC} -Thtml -Ostyle=../mcplexman.css -Oman=../html%S/%N.%S.html
TARTARGETS += ${TARDIR}/${HTMLMAN}/html1/dwmwhat.1.html
endif

dwmwhat: ${OBJFILES} ../../classes/lib/libDwm.la
	${LIBTOOL} --mode=link --tag=CXX ${CXX} ${PTHREADLDFLAGS} ${LDFLAGS} -rpath ${INSTALLPREFIX}/lib -o $@ $^ ${ZLIB} ${BZLIB} ${TERMCAPLIB} -lpcap ${OSLIBS}

tarprep: ${TARTARGETS}

${TARDIR}/bin/dwmwhat: dwmwhat
	${LIBTOOL} --mode=install ../../install-sh -s -c -m 555 $< $@

${TARDIR}/man/man1/dwmwhat.1: dwmwhat.1
	../../install-sh -c -m 644 $< $@

${TARDIR}/${HTMLMAN}/html1/dwmwhat.1.html: dwmwhat.1
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
	${LIBTOOL} --mode=clean rm -f dwmwhat ${OBJFILES}

clean-tarprep::
	${LIBTOOL} --mode=uninstall rm -f ${TARDIR}/bin/dwmwhat
