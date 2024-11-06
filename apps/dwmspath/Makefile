load $(shell pkg-config --variable=libdir dwmgmk)/dwm_gmk.so(dwm_gmk_setup)
myfile := $(abspath $(lastword $(MAKEFILE_LIST)))
$(dwm_myns dwmspath)
$(dwm_aliasfn my,dwm_my)
$(my mydir := $(abspath $(dir $(myfile))))

$(dwm_include_once $(abspath $(my mydir)/../../classes/src/Makefile))

$(my CxxFlags    := ${CXXFLAGS} ${PTHREADCXXFLAGS})
$(my Incs        := -I$(abspath $(my mydir)/../../classes/include) ${EXTINCS})
$(my Lib         := $(abspath $(my mydir)/../../classes/lib/libDwm.la))
$(my Link        := ${LIBTOOL} --quiet --mode=link --tag=CXX ${CXX})
$(my ObjNames    := dwmspath.o)
$(my Objs        := $(patsubst %.o,$(my mydir)/%.o,$(my ObjNames)))
$(my ObjDeps     := $(patsubst %.o,$(my mydir)/deps/%_deps,$(my ObjNames)))
$(my Targets     := $(my mydir)/dwmspath)
$(my TarTargets  := ${TARDIR}/bin/dwmspath)
$(my Clean       := $(my Objs))
$(my LtClean     := $(my Targets))

$(eval DEPSTARGETS      $(dwm_ifcwd :=,+=) $(my dwmspath.ObjDeps))
$(eval CLEANTARGETS     $(dwm_ifcwd :=,+=) $(my dwmspath.Clean))
$(eval LTCLEANTARGETS   $(dwm_ifcwd :=,+=) $(my dwmspath.LtClean))
$(eval DISTCLEANTARGETS $(dwm_ifcwd :=,+=) $(my dwmspath.ObjDeps))
$(eval TARGETS          $(dwm_ifcwd :=,+=) $(my dwmspath.Targets))
$(eval TARTARGETS       $(dwm_ifcwd :=,+=) $(my dwmspath.TarTargets))

$(dwm_include $(abspath $(dir $(myfile))/../../Makefile.rules))


$(my mydir)/dwmspath: $(my Objs) $(my Lib)
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

