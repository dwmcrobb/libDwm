load $(shell pkg-config --variable=libdir dwmgmk)/dwm_gmk.so(dwm_gmk_setup)
libSrcMkFile := $(abspath $(lastword $(MAKEFILE_LIST)))
$(dwm_aliasfn my,dwm_my)
$(dwm_myns lib)
$(my mydir := $(abspath $(dir $(libSrcMkFile))))

$(dwm_include verbose,../../Makefile.vars)

ALLINC          := -I$(abspath $(my mydir)/../include) ${EXTINCS}
LINK            := ${LIBTOOL} --mode=link --tag=CXX ${CXX}

$(my CppSrcs    := $(dwm_files $(my mydir),.*\.cc))
$(my FlexSrcs   := $(subst .lex,.cc,$(dwm_files $(my mydir),.*\.lex)))
$(my FlexSrcs   := $(my FlexSrcs) $(subst .ll,.cc,$(dwm_files $(my mydir),.*\.ll)))
$(my BisonSrcs  := $(subst .y,.cc,$(dwm_files $(my mydir),.*\.y)))
$(my Srcs       := $(sort $(my CppSrcs) $(my FlexSrcs) $(my BisonSrcs)))
$(my ObjNames   := $(subst .cc,.o,$(my Srcs)))

ifeq (,$(findstring iPhone,$(PLATFORM)))
$(my ObjNames   := $(sort $(my ObjNames) DwmPcap.o))
endif

$(my CxxFlags   := ${CXXFLAGS} ${PTHREADCXXFLAGS} ${ALLINC})
$(my Compile    := ${LIBTOOL} --mode=compile --tag=CXX ${CXX} $(my CxxFlags))
$(my ObjDir     := $(abspath $(my mydir)/../obj))
$(my DepsDir    := $(my mydir)/deps)
$(my Objs       := $(patsubst %,$(my ObjDir)/%,$(my ObjNames)))
$(my SharedObjs := $(patsubst %.o,%.lo,$(my Objs)))
$(my ObjDeps    := $(patsubst %.o,$(my mydir)/deps/%_deps,$(my ObjNames)))
$(my LibDir     := $(abspath $(my mydir)/../lib))
$(my Lib        := $(my LibDir)/libDwm.la)
$(my Targets    := $(my Lib))

shlib_version := $(shell $(my mydir)/../../getvers.sh -s)

TARGETS          += $(my Targets)
DEPSTARGETS      += $(my ObjDeps)
CLEANTARGETS     += $(my Objs) $(my SharedObjs) $(my Lib)
DISTCLEANTARGETS += $(my ObjDeps) $(my BisonSrcs) $(my FlexSrcs) \
                    $(patsubst %.cc,%.hh,$(my BisonSrcs))
TARTARGETS       += ${TARDIR}/lib/libDwm.la

ifeq ("$(CURDIR)","$(my mydir)")
TARGETS          := $(my Targets)
DEPSTARGETS      := $(my ObjDeps)
CLEANTARGETS     := $(my Objs) $(my SharedObjs) $(my Lib)
DISTCLEANTARGETS := $(my ObjDeps) $(my BisonSrcs) $(my FlexSrcs) \
                    $(patsubst %.cc,%.hh,$(my BisonSrcs))
TARTARGETS       := ${TARDIR}/lib/libDwm.la
endif

# $(dwm_include verbose,$(abspath $(my mydir)/../../Makefile.rules))

#-----------------------------------------------------------------------------

$(my LibDir)/%.la: $(my lib.SharedObjs) $(my lib.Objs)
	@dwmgmk_quiet "creating $(dwm_relpwd $@)" \
	 $(LINK) -o $@ $(my lib.SharedObjs) -rpath ${INSTALLPREFIX}/lib \
          ${EXTLIBS} -version-info ${shlib_version} ${LDFLAGS}

#  generate dependency rule
$(eval $(dwm_cppdeps $(my lib.mydir)/deps,\
$(abspath $(my lib.mydir)/../obj),$(my lib.mydir)/%.cc,\
${CXX} -MM $(my CxxFlags) -c $<,.o .lo))

#  only include dependency makefiles if target is not 'clean' or 'distclean'
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
$(dwm_include $(my lib.ObjDeps))
endif
endif

$(my ObjDir)/%.o $(my ObjDir)/%.lo: $(my mydir)/%.cc $(my DepsDir)/%_deps
	@dwmgmk_quiet "compiling $(dwm_relpwd $<)" \
         $(my lib.Compile) -c $< -o $@

${TARDIR}/lib/%.la: $(my lib.Lib)
	${INSTALL} -s -c $< $@

$(my mydir)/%Parse.cc $(my mydir)/%Parse.hh &: $(my mydir)/%Parse.y
	bison -d -o $(patsubst %.hh,%.cc,$@) $<

$(my mydir)/%Lex.cc: $(my mydir)/%Lex.lex $(my mydir)/%Parse.hh
	flex -f -o$@ $<

$(my mydir)/%.cc: $(my mydir)/%.ll
	flex -o$@ $<

$(dwm_include verbose,$(abspath $(my mydir)/../../Makefile.rules))
