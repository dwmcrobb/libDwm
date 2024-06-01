include ../../Makefile.vars

CXXFLAGS   += -g
INCS	    = -I../../classes/include
OBJFILES    = dwmspath.o
OBJDEPS	    = $(OBJFILES:%.o=deps/%_deps)

dwmspath: ${OBJFILES} ../../classes/lib/libDwm.la
	${LIBTOOL} --mode=link --tag=CXX ${CXX} ${PTHREADLDFLAGS} ${LDFLAGS} -o $@ $^ ${ZLIB} ${BZLIB} -lpcap ${OSLIBS} -static

tarprep: ${TARDIR}/bin/dwmspath

${TARDIR}/bin/dwmspath: dwmspath
	${LIBTOOL} --mode=install ../../install-sh -s -c -m 555 $< $@

#  dependency rule
deps/%_deps: %.cc
	@echo "making dependencies for $<"
	@set -e; \
	${CXX} -MM ${CXXFLAGS} ${PTHREADCXXFLAGS} ${INCS} -c $< | \
	 sed 's/\($*\)\.o[ :]*/\1.o $(@D)\/$(@F) : /g' > $@ ; [ -s $@ ] || \
	 rm -f $@

#  only include dependency makefiles if target is not 'clean' or 'distclean'
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include ${OBJDEPS}
endif
endif

%.o: %.cc
	${CXX} ${CXXFLAGS} ${INCS} -c $<

../../classes/lib/libDwm.la::
	${MAKE} -C ../../classes/src

distclean:: clean
	rm -f ${OBJDEPS}

clean:: clean-tarprep
	${LIBTOOL} --mode=clean rm -f dwmspath ${OBJFILES}

clean-tarprep::
	${LIBTOOL} --mode=uninstall rm -f ${TARDIR}/bin/dwmspath
