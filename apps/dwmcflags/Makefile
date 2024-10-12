include ../../Makefile.vars

CXXFLAGS   += -g
ALLINC      = -I../../classes/include
OBJFILES    = dwmcflags.o
OBJDEPS	    = $(OBJFILES:%.o=deps/%_deps)
TARTARGETS  = ${TARDIR}/bin/dwmcflags \
	      ${TARDIR}/man/man1/dwmcflags.1
ifeq ("${MANDOC}", "mandoc")
MANHTML = ${MANDOC} -Thtml -Ostyle=../mcplexman.css -Oman=../html%S/%N.%S.html
TARTARGETS += ${TARDIR}/${HTMLMAN}/html1/dwmcflags.1.html
endif

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

dwmcflags: ${OBJFILES} ../../classes/lib/libDwm.la
	$(call Link,$@,$^)

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
	 sed 's/\($*\)\.o[ :]*/\1.o $(@D)\/$(@F) : /g' > $@ ; \
	[ -s $@ ] || rm -f $@

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
	@${LIBTOOL} --quiet --mode=clean rm -f dwmcflags ${OBJFILES}

clean-tarprep::
	@${LIBTOOL} --mode=uninstall rm -f ${TARDIR}/bin/dwmcflags
