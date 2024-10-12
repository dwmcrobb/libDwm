include ../../Makefile.vars

ALLINC          = -I../../classes/include ${EXTINCS}
OBJFILES	= dwmnet.o
OBJDEPS		= $(OBJFILES:%.o=deps/%_deps)

#-----------------------------------------------------------------------------
define Compile
 @echo compiling $(1)
 @cxx_cmd="${CXX} ${CXXFLAGS} ${PTHREADCXXFLAGS} ${ALLINC} -c $(1) -o $(2)"; \
 cxx_cmd_out=`$${cxx_cmd} 2>&1`; \
 if [ $$? -ne 0 ]; then \
   printf "%s\n%s\n" "$${cxx_cmd}" "$${cxx_cmd_out}"; \
   exit 1; \
 fi
endef

define Link
 @echo linking $(1)
 @${LIBTOOL} --quiet --mode=link --tag=CXX ${CXX} ${LDFLAGS} -rpath ${INSTALLPREFIX}/lib -o $(1) $(2) ${EXTLIBS} ${PTHREADLDFLAGS}
endef
#-----------------------------------------------------------------------------

dwmnet: ${OBJFILES} ../../classes/lib/libDwm.la
	$(call Link,$@,$^)

tarprep: ${TARDIR}/bin/dwmnet

${TARDIR}/bin/dwmnet: dwmnet
	${LIBTOOL} --mode=install ../../install-sh -s -c -m 555 $< $@

#  dependency rule
deps/%_deps: %.cc
	@echo "making dependencies for $<"
	@set -e; \
	${CXX} -MM ${CXXFLAGS} ${PTHREADCXXFLAGS} ${ALLINC} -c $< | \
	 sed 's/\($*\)\.o[ :]*/\1.o $(@D)\/$(@F) : /g' > $@ ; [ -s $@ ] || \
	 rm -f $@

#  only include dependency makefiles if target is not 'clean' or 'distclean'
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include ${OBJDEPS}
endif
endif

%.o: %.cc
	$(call Compile,$<,$@)

../../classes/lib/libDwm.la::
	@${MAKE} -s -C ../../classes/src

distclean:: clean
	@rm -f ${OBJDEPS}

clean:: clean-tarprep
	@${LIBTOOL} --quiet --mode=clean rm -f dwmnet ${OBJFILES}

clean-tarprep::
	@${LIBTOOL} --quiet --mode=uninstall rm -f ${TARDIR}/bin/dwmnet

