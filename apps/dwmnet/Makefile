load $(shell pkg-config --variable=libdir dwmgmk)/dwm_gmk.so(dwm_gmk_setup)
myfile := $(abspath $(lastword $(MAKEFILE_LIST)))
$(dwm_myns dwmnet)
$(dwm_aliasfn my,dwm_my)
$(my mydir := $(abspath $(dir $(myfile))))

$(dwm_include $(abspath $(dir $(myfile))/../../Makefile.vars))

$(my CxxFlags := ${CXXFLAGS} ${PTHREADCXXFLAGS})
$(my Incs     := -I$(abspath $(my mydir)/../../classes/include) ${EXTINCS})
$(my Link     := ${LIBTOOL} --quiet --mode=link --tag=CXX ${CXX})
$(my ObjNames := dwmnet.o)
$(my Objs     := $(patsubst %.o,$(my mydir)/%.o,$(my ObjNames)))
$(my ObjDeps  := $(patsubst %.o,$(my mydir)/deps/%_deps,$(my ObjNames)))
$(my Targets  := $(my mydir)/dwmnet)

DEPSTARGETS       += $(my ObjDeps)
CLEANTARGETS      += $(my Targets) $(my Objs)
DISTCLEANTARGETS  += $(my ObjDeps)
TARGETS           += $(my Targets)
TARTARGETS        += ${TARDIR}/bin/dwmnet

$(dwm_include verbose,$(abspath $(my mydir)/../../Makefile.rules))
$(dwm_include verbose,$(abspath $(my mydir)/../../classes/src/Makefile))

ifeq ("${CURDIR}","$(my mydir)")
DEPSTARGETS       := $(my ObjDeps)
CLEANTARGETS      := $(my Targets) $(my Objs)
DISTCLEANTARGETS  := $(my ObjDeps)
TARGETS           := $(my Targets)
TARTARGETS        := ${TARDIR}/bin/dwmnet
endif

$(my mydir)/dwmnet: $(my Objs) $(abspath $(my mydir)/../../classes/lib/libDwm.la)
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

