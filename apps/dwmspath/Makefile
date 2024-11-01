load $(shell pkg-config --variable=libdir dwmgmk)/dwm_gmk.so(dwm_gmk_setup)
myfile := $(abspath $(lastword $(MAKEFILE_LIST)))
$(dwm_myns dwmspath)
$(dwm_aliasfn my,dwm_my)
$(my mydir := $(abspath $(dir $(myfile))))

$(dwm_include $(abspath $(dir $(myfile))/../../Makefile.vars))

$(my CxxFlags := ${CXXFLAGS} ${PTHREADCXXFLAGS})
$(my Incs     := -I$(abspath $(my mydir)/../../classes/include) ${EXTINCS})
$(my Link     := ${LIBTOOL} --quiet --mode=link --tag=CXX ${CXX})
$(my ObjNames := dwmspath.o)
$(my Objs     := $(patsubst %.o,$(my mydir)/%.o,$(my ObjNames)))
$(my ObjDeps  := $(patsubst %.o,$(my mydir)/deps/%_deps,$(my ObjNames)))
$(my Targets  := $(my mydir)/dwmspath)

DEPSTARGETS       += $(my dwmspath.ObjDeps)
CLEANTARGETS      += $(my dwmspath.Targets) $(my dwmspath.Objs)
DISTCLEANTARGETS  += $(my dwmspath.ObjDeps)
TARGETS           += $(my dwmspath.Targets)
TARTARGETS        += ${TARDIR}/bin/dwmspath

$(dwm_include verbose,$(abspath $(my mydir)/../../classes/src/Makefile))

ifeq ("${CURDIR}","$(my mydir)")
DEPSTARGETS       := $(my ObjDeps)
CLEANTARGETS      := $(my Targets) $(my Objs)
DISTCLEANTARGETS  := $(my ObjDeps)
TARGETS           := $(my Targets)
TARTARGETS        := ${TARDIR}/bin/dwmspath
endif

$(my mydir)/dwmspath: $(my Objs) $(abspath $(my mydir)/../../classes/lib/libDwm.la)
	@dwmgmk_quiet "linking $(dwm_relpwd $@)" \
	 $(my dwmspath.Link) ${LDFLAGS} -rpath ${INSTALLPREFIX}/lib -o $@ $^ ${EXTLIBS} ${PTHREADLDFLAGS}

${TARDIR}/bin/dwmspath: $(my mydir)/dwmspath
	${INSTALL} -s -c -m 555 $< $@

#  generate dependency rule
$(eval $(dwm_cppdeps $(my dwmspath.mydir)/deps,\
$(my dwmspath.mydir),$(my dwmspath.mydir)/%.cc,\
${CXX} -MM $(my dwmspath.CxxFlags) $(my dwmspath.Incs) -c $<))

#  only include dependency makefiles if target is not 'clean' or 'distclean'
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
$(dwm_include $(my ObjDeps))
endif
endif

$(my mydir)/%.o: $(my mydir)/%.cc $(my mydir)/deps/%_deps
	@dwmgmk_quiet "compiling $(dwm_relpwd $<)" \
	 ${CXX} $(my dwmspath.CxxFlags) $(my dwmspath.Incs) -c $< -o $@

