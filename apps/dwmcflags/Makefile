load $(shell pkg-config --variable=libdir dwmgmk)/dwm_gmk.so(dwm_gmk_setup)
myfile := $(abspath $(lastword $(MAKEFILE_LIST)))
$(dwm_myns dwmcflags)
$(dwm_aliasfn my,dwm_my)
$(my mydir := $(abspath $(dir $(myfile))))

$(dwm_include $(abspath $(dir $(myfile))/../../Makefile.vars))

$(my CxxFlags := ${CXXFLAGS} ${PTHREADCXXFLAGS})
$(my Incs     := -I$(abspath $(my mydir)/../../classes/include))
$(my Link     := ${LIBTOOL} --quiet --mode=link --tag=CXX ${CXX})
$(my ObjNames := dwmcflags.o)
$(my Objs     := $(patsubst %.o,$(my mydir)/%.o,$(my ObjNames)))
$(my ObjDeps  := $(patsubst %.o,$(my mydir)/deps/%_deps,$(my ObjNames)))
$(my Targets  := $(my mydir)/dwmcflags)

DEPSTARGETS       += $(my dwmcflags.ObjDeps)
CLEANTARGETS      += $(my dwmcflags.Targets) $(my dwmcflags.Objs)
DISTCLEANTARGETS  += $(my dwmcflags.ObjDeps)
TARGETS           += $(my dwmcflags.Targets)
TARTARGETS        += ${TARDIR}/bin/dwmcflags ${TARDIR}/man/man1/dwmcflags.1

$(dwm_include verbose,$(abspath $(my mydir)/../../classes/src/Makefile))

ifeq ("${CURDIR}","$(my mydir)")
DEPSTARGETS       := $(my ObjDeps)
CLEANTARGETS      := $(my Targets) $(my Objs)
DISTCLEANTARGETS  := $(my ObjDeps)
TARGETS           := $(my Targets)
TARTARGETS        := ${TARDIR}/bin/dwmcflags ${TARDIR}/man/man1/dwmcflags.1
endif

ifeq ("${MANDOC}","mandoc")
MANHTML = ${MANDOC} -Thtml -Ostyle=../mcplexman.css -Oman=../html%S/%N.%S.html
TARTARGETS += ${TARDIR}/${HTMLMAN}/html1/dwmcflags.1.html
endif

$(my mydir)/dwmcflags: $(my Objs) $(abspath $(my mydir)/../../classes/lib/libDwm.la)
	@dwmgmk_quiet "linking $(dwm_relpwd $@)" \
	 $(my dwmcflags.Link) ${LDFLAGS} -rpath ${INSTALLPREFIX}/lib -o $@ $^ ${EXTLIBS} ${PTHREADLDFLAGS}

${TARDIR}/bin/dwmcflags: $(my mydir)/dwmcflags
	${INSTALL} -s -c -m 555 $< $@

${TARDIR}/man/man1/dwmcflags.1: $(my mydir)/dwmcflags.1
	${INSTALL} -c -m 644 $< $@

${TARDIR}/${HTMLMAN}/html1/dwmcflags.1.html: $(my mydir)/dwmcflags.1
	${MANHTML} $< > $<.html
	${INSTALL} -c -m 644 $<.html $@
	rm $<.html

#  generate dependency rule
$(eval $(dwm_cppdeps $(my dwmcflags.mydir)/deps,\
$(my dwmcflags.mydir),$(my dwmcflags.mydir)/%.cc,\
${CXX} -MM $(my dwmcflags.CxxFlags) $(my dwmcflags.Incs) -c $<))

#  only include dependency makefiles if target is not 'clean' or 'distclean'
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
$(dwm_include $(my ObjDeps))
endif
endif

$(my mydir)/%.o: $(my mydir)/%.cc $(my mydir)/deps/%_deps
	@dwmgmk_quiet "compiling $(dwm_relpwd $<)" \
	 ${CXX} $(my dwmcflags.CxxFlags) $(my dwmcflags.Incs) -c $< -o $@


