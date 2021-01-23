include ../../Makefile.vars

INCS            = -I../../classes/include
OBJFILES	= dwmnet.o
OBJDEPS		= $(OBJFILES:%.o=deps/%_deps)

dwmnet: ${OBJFILES} ../../classes/lib/libDwm.la
	${LIBTOOL} --mode=link --tag=CXX ${CXX} ${LDFLAGS} ${PTHREADLDFLAGS} -o $@ $^ ${ZLIB} ${BZLIB} -static

tarprep: ${TARDIR}/bin/dwmnet

${TARDIR}/bin/dwmnet: dwmnet
	../../install-sh -s -c -m 555 $< $@

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

dwmnet.o: dwmnet.cc
	${CXX} ${CXXFLAGS} -c $< ${INCS}

../../classes/lib/libDwm.la::
	${MAKE} -C ../../classes/src

clean:: clean-tarprep
	rm -f dwmnet ${OBJFILES}

clean-tarprep::
	rm -f ${TARDIR}/bin/dwmnet

