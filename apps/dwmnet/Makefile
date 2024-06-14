include ../../Makefile.vars

INCS            = -I../../classes/include ${EXTINCS}
OBJFILES	= dwmnet.o
OBJDEPS		= $(OBJFILES:%.o=deps/%_deps)

dwmnet: ${OBJFILES} ../../classes/lib/libDwm.la
	${LIBTOOL} --mode=link --tag=CXX ${CXX} ${LDFLAGS} ${PTHREADLDFLAGS} -o $@ $^ -rpath ${INSTALLPREFIX}/lib ${ZLIB} ${BOOSTLIBS} ${EXTLIBS}

tarprep: ${TARDIR}/bin/dwmnet

${TARDIR}/bin/dwmnet: dwmnet
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

dwmnet.o: dwmnet.cc
	${CXX} ${CXXFLAGS} -c $< ${INCS}

../../classes/lib/libDwm.la::
	${MAKE} -C ../../classes/src

distclean:: clean
	rm -f ${OBJDEPS}

clean:: clean-tarprep
	${LIBTOOL} --mode=clean rm -f dwmnet ${OBJFILES}

clean-tarprep::
	${LIBTOOL} --mode=uninstall rm -f ${TARDIR}/bin/dwmnet

