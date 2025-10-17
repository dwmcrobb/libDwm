load $(shell pkg-config --variable=libdir dwmgmk)/dwm_gmk.so(dwm_gmk_setup)
examplesMkFile := $(abspath $(lastword $(MAKEFILE_LIST)))
$(dwm_aliasfn my,dwm_my)
$(dwm_myns examples)
$(my mydir := $(abspath $(dir $(examplesMkFile))))

$(dwm_include_once $(abspath $(my mydir)/../../../classes/src/Makefile))

$(my CxxFlags   := ${CXXFLAGS} ${PTHREADCXXFLAGS} ${CLASSINC} ${EXTINCS})
$(my Lib        := $(abspath $(my mydir)/../../../classes/lib/libDwm.la))
$(my Link       := ${LIBTOOL} --quiet --mode=link --tag=CXX ${CXX})
$(my Srcs       := $(dwm_files $(my mydir),Example.*\.cc))
$(my ObjNames   := $(subst .cc,.o,$(my Srcs)))
$(my ObjDir     := $(my mydir))
$(my DepsDir    := $(my mydir)/deps)
$(my Objs       := $(patsubst %,$(my ObjDir)/%,$(my ObjNames)))
$(my ObjDeps    := $(patsubst %.o,$(my DepsDir)/%_deps,$(my ObjNames)))
$(my Exes       := $(patsubst %.o,%,$(my Objs)))
$(my Clean      := $(my Exes))
$(my Clean      += $(patsubst %.o,$(my ObjDir)/.libs/%,$(my ObjNames)))

$(eval TARGETS          $(dwm_ifcwd :=,+=) $(my examples.Exes))
$(eval DEPSTARGETS      $(dwm_ifcwd :=,+=) $(my examples.ObjDeps))
$(eval CLEANTARGETS     $(dwm_ifcwd :=,+=) $(my examples.Clean,examples.Objs))
$(eval DISTCLEANTARGETS $(dwm_ifcwd :=,+=) $(my examples.ObjDeps))

$(dwm_include $(abspath $(my mydir)/../../../Makefile.rules))

.SECONDARY: $(my examples.Objs) $(my examples.ObjDeps)

runexamples: $(my Exes)
	@ for tp in $(my examples.Exes) ; do \
		printf "%-36s " `basename $$tp` ; \
		out=`$$tp 2>&1` ; \
		if [ $$? -eq 0 ]; then \
		  printf "%25s\n" "$$out" ; \
		else \
		  printf '\n%s\n' "$$out" ; \
		fi ; \
	done

#  generate dependency rule
$(eval $(dwm_cppdeps $(my examples.mydir)/deps,(my examples.mydir),\
$(my examples.mydir)/%.cc,${CXX} -MM $(my examples.CxxFlags) $<))

#  only include dependency makefiles if target is not a 'clean' target
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
ifneq ($(MAKECMDGOALS),clean-examples)
$(dwm_include $(my examples.ObjDeps))
endif
endif
endif

$(my mydir)/Example%.o: $(my mydir)/Example%.cc $(my DepsDir)/Example%_deps
	@dwmgmk_quiet "compiling $(dwm_relpwd $<)" \
	 ${CXX} $(my examples.CxxFlags) -c $< -o $@

$(my mydir)/Example%: $(my mydir)/Example%.o $(my Lib)
	@dwmgmk_quiet "linking $(dwm_relpwd $@)" \
	 $(my examples.Link) ${LDFLAGS} -o $@ $^ ${EXTLIBS} ${PTHREADLDFLAGS}
