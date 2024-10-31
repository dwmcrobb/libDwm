load $(shell pkg-config --variable=libdir dwmgmk)/dwm_gmk.so(dwm_gmk_setup)
testsMkFile := $(abspath $(lastword $(MAKEFILE_LIST)))
$(dwm_aliasfn my,dwm_my)
$(dwm_myns tests)
$(my mydir := $(abspath $(dir $(testsMkFile))))

$(dwm_include verbose,$(abspath $(my mydir)/../../Makefile.vars))

$(my CxxFlags   := ${CXXFLAGS} ${PTHREADCXXFLAGS})
$(my Incs       := -I$(abspath $(my mydir)/../include) ${EXTINCS})
$(my Link       := ${LIBTOOL} --quiet --mode=link --tag=CXX ${CXX})
$(my Srcs       := $(dwm_files $(my mydir),Test.*\.cc))
$(my ObjNames   := $(subst .cc,.o,$(my Srcs)))
$(my ObjDir     := $(my mydir))
$(my DepsDir    := $(my mydir)/deps)
$(my Objs       := $(patsubst %,$(my ObjDir)/%,$(my ObjNames)))
$(my ObjDeps    := $(patsubst %.o,$(my DepsDir)/%_deps,$(my ObjNames)))
$(my Exes       := $(patsubst %.o,%,$(my Objs)))

TARGETS          += $(my Exes)
DEPSTARGETS      += $(my ObjDeps)
CLEANTARGETS     += $(my Exes) $(my Objs)
DISTCLEANTARGETS += $(my ObjDeps)

$(dwm_include verbose,$(abspath $(my mydir)/../../Makefile.rules))
$(dwm_include verbose,$(abspath $(my mydir)/../src/Makefile))

ifeq ("${CURDIR}","$(my tests.mydir)")
TARGETS          := $(my Exes)
DEPSTARGETS      := $(my ObjDeps)
CLEANTARGETS     := $(my Exes) $(my Objs)
DISTCLEANTARGETS := $(my ObjDeps)
endif


.PHONY: runtests

runtests: $(my Exes)
	@ for tp in $(my tests.Exes) ; do \
		printf "%-36s " `basename $$tp` ; \
		out=`./$$tp` ; \
		if [ $$? -eq 0 ]; then \
		  printf "%25s\n" "$$out" ; \
		else \
		  printf "\n%s" "$$out" ; \
		fi ; \
	done

#  generate dependency rule
$(eval $(dwm_cppdeps $(my mydir)/deps,\
$(my mydir),$(my mydir)/%.cc,\
${CXX} -MM $(my tests.CxxFlags) $(my tests.Incs) -c $<))

#  only include dependency makefiles if target is not 'clean' or 'distclean'
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
$(dwm_include $(my tests.ObjDeps))
endif
endif

$(my mydir)/Test%.o: $(my mydir)/Test%.cc $(my tests.DepsDir)/Test%_deps
	@dwmgmk_quiet "compiling $(dwm_relpwd $<)" \
	 ${CXX} $(my tests.CxxFlags) $(my tests.Incs) -c $< -o $@

$(my mydir)/Test%: $(my mydir)/Test%.o $(abspath $(my mydir)/../lib/libDwm.la)
	@dwmgmk_quiet "linking $(dwm_relpwd $@)" \
	 $(my tests.Link) ${LDFLAGS} -o $@ $^ ${EXTLIBS} ${PTHREADLDFLAGS}
