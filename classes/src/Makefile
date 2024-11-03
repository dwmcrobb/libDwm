load $(shell pkg-config --variable=libdir dwmgmk)/dwm_gmk.so(dwm_gmk_setup)
libSrcMkFile := $(abspath $(lastword $(MAKEFILE_LIST)))
$(dwm_aliasfn my,dwm_my)
$(dwm_myns lib)
$(my mydir := $(abspath $(dir $(libSrcMkFile))))

$(dwm_include ../../Makefile.vars)

shlib_version := $(shell $(my mydir)/../../getvers.sh -s)

$(my CxxFlags   := ${CXXFLAGS} ${PTHREADCXXFLAGS} ${CLASSINC} ${EXTINCS})
$(my Compile    := ${LIBTOOLCOMPILE} $(my CxxFlags))
$(my Link       := ${LIBTOOLLINK})
$(my LinkRpath  := -rpath ${INSTALLPREFIX}/lib)
$(my LibVersion := -version-info ${shlib_version})
$(my LinkFlags  := $(my LinkRpath) ${EXTLIBS} $(my LibVersion) ${LDFLAGS})
$(my CppNames   := $(dwm_files $(my mydir),.*\.cc))
$(my CppSrcs    := $(patsubst %.cc,$(my mydir)/%.cc,$(my CppNames)))
$(my FlexNames  := $(subst .lex,.cc,$(dwm_files $(my mydir),.*\.lex)))
$(my FlexNames  += $(subst .ll,.cc,$(dwm_files $(my mydir),.*\.ll)))
$(my FlexSrcs   := $(patsubst %.cc,$(my mydir)/%.cc,$(my FlexNames)))
$(my BisonNames := $(subst .y,.cc,$(dwm_files $(my mydir),.*\.y)))
$(my BisonSrcs  := $(patsubst %.cc,$(my mydir)/%.cc,$(my BisonNames)))
$(my BisonHdrs  := $(patsubst %.cc,%.hh,$(my BisonSrcs)))
$(my SrcNames   := $(sort $(my CppNames,FlexNames,BisonNames)))
$(my ObjNames   := $(subst .cc,.o,$(my SrcNames)))
$(my ObjDir     := $(abspath $(my mydir)/../obj))
$(my DepsDir    := $(my mydir)/deps)
$(my Objs       := $(patsubst %,$(my ObjDir)/%,$(my ObjNames)))
$(my SharedObjs := $(patsubst %.o,%.lo,$(my Objs)))
$(my ObjDeps    := $(patsubst %.o,$(my DepsDir)/%_deps,$(my ObjNames)))
$(my LibDir     := $(abspath $(my mydir)/../lib))
$(my Lib        := $(my LibDir)/libDwm.la)
$(my Targets    := $(my Lib))
$(my CleanTargets := $(my Objs,SharedObjs,Lib,FlexSrcs,BisonSrcs,BisonHdrs))

$(eval TARGETS $(dwm_ifcwd :=,+=) $(my lib.Targets))
$(eval DEPSTARGETS $(dwm_ifcwd :=,+=) $(my lib.ObjDeps))
$(eval CLEANTARGETS $(dwm_ifcwd :=,+=) $(my lib.CleanTargets))
$(eval DISTCLEANTARGETS $(dwm_ifcwd :=,+=) $(my lib.ObjDeps))
$(eval TARTARGETS $(dwm_ifcwd :=,+=) ${TARDIR}/lib/libDwm.la)

#-----------------------------------------------------------------------------

$(my LibDir)/%.la: $(my lib.SharedObjs) $(my lib.Objs)
	@dwmgmk_quiet "creating $(dwm_relpwd $@)" \
	 $(my lib.Link) -o $@ $(my lib.SharedObjs) $(my lib.LinkFlags)

#  generate dependency rule
$(eval $(dwm_cppdeps $(my lib.DepsDir),$(my lib.ObjDir),\
$(my lib.mydir)/%.cc,${CXX} -MM $(my CxxFlags) -c $<,.o .lo))

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

$(dwm_include $(abspath $(my mydir)/../../Makefile.rules))
