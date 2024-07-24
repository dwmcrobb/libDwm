include ../../Makefile.vars

ALLINC          = -I../include ${EXTINCS}
CXXFLAGS	+= -g -Wno-deprecated-register -Wno-register
OBJFILESNP =	DwmASIO.o \
		DwmBZ2IO.o \
                DwmBootTime.o \
		DwmBase64.o \
		DwmCgi.o \
		DwmCvsTag.o \
		DwmDaemonUtils.o \
		DwmDateTime.o \
		DwmDescriptor.o \
		DwmDescriptorIO.o \
		DwmDirectoryEntry.o \
		DwmEngFormat.o \
		DwmExecutor.o \
		DwmFileIO.o \
		DwmFileLogger.o \
		DwmFileRoller.o \
		DwmGetAddrInfo.o \
                DwmGitVersion.o \
		DwmGZIO.o \
		DwmGroup.o \
		DwmIpAddress.o \
		DwmIpPrefix.o \
		DwmIpv4Address.o \
		DwmIpv4Prefix.o \
		DwmIpv4Utils.o \
		DwmIpv6Address.o \
		DwmIpv6Prefix.o \
		DwmLocalInterfaces.o \
		DwmMacAddress.o \
		DwmMplsLabel.o \
		DwmMplsLabelStack.o \
		DwmOptArgs.o \
		DwmPacer.o \
		DwmPackageInfo.o \
		DwmPassword.o \
		DwmPathUtils.o \
                DwmProcessInfo.o \
                DwmProcessTableLex.o \
                DwmProcessTableParse.o \
		DwmRusage.o \
		DwmSignal.o \
		DwmStreamIO.o \
		DwmStringUtils.o \
		DwmStrptime.o \
		DwmSunriseSunset.o \
		DwmSvnTag.o \
		DwmSysLogger.o \
		DwmTerminalTricks.o \
		DwmTermios.o \
		DwmTimeInterval.o \
		DwmTimeInterval64.o \
		DwmTimeUtil.o \
		DwmTimeValue.o \
		DwmTimeValue64.o \
		DwmTypeName.o \
		DwmUnitAssert.o \
                DwmUptime.o \
		DwmXDRUtils.o
shlib_version = $(shell ../../getvers.sh -s)

ifeq (,$(findstring iPhone,$(PLATFORM)))
OBJFILESNP +=	DwmPcap.o
endif
OBJFILES      = $(OBJFILESNP:%=../obj/%)
SHARED_OBJFILES = $(OBJFILES:%.o=%.lo)
OBJDEPS         = $(OBJFILESNP:%.o=deps/%_deps)

all:: ../lib/libDwm.la

../lib/libDwm.la: ${SHARED_OBJFILES}
	${LIBTOOL} --tag=CXX --mode=link ${CXX} -no-suppress -o $@ $^ -rpath ${INSTALLPREFIX}/lib ${EXTLIBS} -version-info ${shlib_version} ${LDFLAGS}

#  dependency rule
deps/%_deps: %.cc
	@echo "making dependencies for $<"
	@set -e; \
	${CXX} -MM ${CXXFLAGS} ${PTHREADCXXFLAGS} ${ALLINC} -c $< | \
	 sed 's/\($*\)\.o[ :]*/\1.o $(@D)\/$(@F) : /g' > $@ ; [ -s $@ ] || \
	 rm -f $@

#  only include dependency makefiles if target is not 'clean' or 'distclean'
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include ${OBJDEPS}
endif
endif

../obj/%.o ../obj/%.lo: %.cc deps/%_deps
	${LIBTOOL} --mode=compile --tag=CXX ${CXX} -no-suppress ${CXXFLAGS} ${PTHREADCXXFLAGS} ${ALLINC} -c $< -o $@

%.cc: %.ll
	flex -o$@ $<

tarprep: ${TARDIR}/lib/libDwm.la

${TARDIR}/lib/libDwm.la: ../lib/libDwm.la
	${LIBTOOL} --mode=install ../../install-sh -s -c $< ${TARDIR}/lib/libDwm.la

DwmProcessTableParse.hh: DwmProcessTableParse.cc

DwmProcessTableParse.cc: DwmProcessTableParse.y
	bison -d -o$@ $<

DwmProcessTableLex.cc: DwmProcessTableLex.lex DwmProcessTableParse.hh
	flex -f -o$@ $<

clean:: clean-tarprep
	${LIBTOOL} --mode=clean rm -f ../lib/libDwm.la ${OBJFILES} DwmCgi.cc \
	 DwmExecutor.cc DwmTimeUtil.cc ${SHARED_OBJFILES}

distclean:: clean
	rm -f deps/*_deps
	rm DwmPackageInfo.cc

clean-tarprep::
	${LIBTOOL} --mode=clean rm -f ${TARDIR}/lib/libDwm.la

