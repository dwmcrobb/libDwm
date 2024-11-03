load $(shell pkg-config --variable=libdir dwmgmk)/dwm_gmk.so(dwm_gmk_setup)
myfile := $(abspath $(lastword $(MAKEFILE_LIST)))
$(dwm_myns dwmnet)
$(dwm_aliasfn my,dwm_my)
$(my mydir := $(abspath $(dir $(myfile))))

$(dwm_include $(abspath $(my mydir)/../../classes/src/Makefile))

$(my CxxFlags   := ${CXXFLAGS} ${PTHREADCXXFLAGS})
$(my Incs       := -I$(abspath $(my mydir)/../../classes/include) ${EXTINCS})
$(my Lib        := $(abspath $(my mydir)/../../classes/lib/libDwm.la))
$(my Link       := ${LIBTOOL} --quiet --mode=link --tag=CXX ${CXX})
$(my ObjNames   := dwmnet.o)
$(my Objs       := $(patsubst %.o,$(my mydir)/%.o,$(my ObjNames)))
$(my ObjDeps    := $(patsubst %.o,$(my mydir)/deps/%_deps,$(my ObjNames)))
$(my Targets    := $(my dwmnet.mydir)/dwmnet)
$(my TarTargets := ${TARDIR}/bin/dwmnet)
$(my RmTargets  := $(my dwmnet.Targets) $(my dwmnet.mydir)/.libs/dwmnet)

$(eval DEPSTARGETS $(dwm_ifcwd :=,+=) $(my dwmnet.ObjDeps))
$(eval CLEANTARGETS $(dwm_ifcwd :=,+=) $(my dwmnet.RmTargets,dwmnet.Objs))
$(eval DISTCLEANTARGETS $(dwm_ifcwd :=,+=) $(my dwmnet.ObjDeps))
$(eval TARGETS $(dwm_ifcwd :=,+=) $(my dwmnet.Targets))
$(eval TARTARGETS $(dwm_ifcwd :=,+=) $(my dwmnet.TarTargets))

$(dwm_include $(abspath $(dir $(myfile))/../../Makefile.rules))

$(my mydir)/dwmnet: $(my Objs) $(my Lib)
	@dwmgmk_quiet "linking $(dwm_relpwd $@)" \
	 $(my dwmnet.Link) ${LDFLAGS} -rpath ${INSTALLPREFIX}/lib -o $@ $^ ${EXTLIBS} ${PTHREADLDFLAGS}

${TARDIR}/bin/dwmnet: $(my mydir)/dwmnet
	${INSTALL} -s -c -m 555 $< $@

#  generate dependency rule
$(eval $(dwm_cppdeps $(my dwmnet.mydir)/deps,\
$(my dwmnet.mydir),$(my dwmnet.mydir)/%.cc,\
${CXX} -MM $(my dwmnet.CxxFlags) $(my dwmnet.Incs) -c $<))

#  only include dependency makefiles if target is not 'clean' or 'distclean'
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
$(dwm_include $(my ObjDeps))
endif
endif

$(my mydir)/%.o: $(my mydir)/%.cc $(my mydir)/deps/%_deps
	@dwmgmk_quiet "compiling $(dwm_relpwd $<)" \
	 ${CXX} $(my dwmnet.CxxFlags) $(my dwmnet.Incs) -c $< -o $@

