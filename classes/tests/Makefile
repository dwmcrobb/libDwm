include ../../Makefile.vars

OBJFILES = TestASIO.o \
	   TestBase64.o \
           TestBootTime.o \
	   TestConcurrentQueue.o \
	   TestDwmBZ2IO.o \
	   TestDwmDataLog.o \
	   TestDwmDateTime.o \
	   TestDwmDescriptor.o \
	   TestDwmDiff.o \
	   TestDwmDirectoryEntry.o \
	   TestDwmFileLogger.o \
	   TestDwmGZIO.o \
	   TestDwmIO.o \
	   TestDwmLoadBalancer.o \
	   TestDwmLocalInterfaces.o \
	   TestDwmOptArgs.o \
	   TestDwmPacer.o \
	   TestDwmPassword.o \
	   TestDwmRusage.o \
	   TestDwmSignal.o \
	   TestDwmSunriseSunset.o \
	   TestDwmTermios.o \
	   TestEngFormat.o \
           TestEnumIO.o \
	   TestFFIVar.o \
	   TestFileRoller.o \
           TestFSyslog.o \
	   TestGetAddrInfo.o \
	   TestGitVersion.o \
	   TestGroup.o \
	   TestIODeque.o \
	   TestIOList.o \
	   TestIOMap.o \
	   TestIOMultiMap.o \
	   TestIOMultiSet.o \
	   TestIOSet.o \
	   TestIOVector.o \
	   TestIpv4Address.o \
	   TestIpv4AddrMap.o \
	   TestIpv4Prefix.o \
	   TestIpv4PrefixMap.o \
	   TestIpv4Routes.o \
	   TestIpv6Address.o \
	   TestIpv6AddrMap.o \
	   TestIpv6Prefix.o \
	   TestIpv6PrefixMap.o \
	   TestMacAddress.o \
	   TestMplsLabel.o \
	   TestMplsLabelStack.o \
           TestProcessTable.o \
	   TestStringConverter.o \
	   TestStringUtils.o \
	   TestSvnTag.o \
           TestSyslog.o \
	   TestThreadPool.o \
	   TestThreadQueue.o \
	   TestTimeInterval64.o \
	   TestTimeUtils.o \
	   TestTupleIO.o \
	   TestTypeName.o \
	   TestUnitAssert.o \
	   TestUnorderedMapIO.o \
	   TestUnorderedMultimapIO.o \
	   TestUnorderedMultisetIO.o \
	   TestUnorderedSetIO.o \
           TestUptime.o \
	   TestValueRange.o \
	   TestVariantIO.o \
	   TestXDRUtils.o
OBJDEPS	 = $(OBJFILES:%.o=deps/%_deps)
TESTS	 = $(OBJFILES:%.o=%)
CXXFLAGS += -g
# CXXFLAGS += -fsanitize=address
LDFLAGS  += -g
# LDFLAGS += -fsanitize=address
ALLINC   = -I../include ${EXTINCS}

all: ${TESTS}

runtests: all
	@ for tp in ${TESTS} ; do \
		printf "%-36s " $$tp ; \
		out=`./$$tp` ; \
		if [ $$? -eq 0 ]; then \
		  printf "%25s\n" "$$out" ; \
		else \
		  printf "\n%s" "$$out" ; \
		fi ; \
	done

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

%.o: %.cc
	${CXX} ${CXXFLAGS} ${PTHREADCXXFLAGS} ${ALLINC} -c $<

ifneq ("${TARGET_VENDOR}","apple")
TestProcessTable: TestProcessTable.o ../lib/libDwm.la
	${LIBTOOL} --mode=link --tag=CXX ${CXX} ${LDFLAGS} -o $@ $^ ${EXTLIBS} ${PTHREADLDFLAGS}
endif

%: %.o ../lib/libDwm.la
	${LIBTOOL} --mode=link --tag=CXX ${CXX} ${LDFLAGS} -o $@ $^ ${EXTLIBS} ${PTHREADLDFLAGS}

../lib/libDwm.la::
	${MAKE} -C ../src

clean:
	${LIBTOOL} --mode=clean rm -f ${TESTS} ${OBJFILES}

distclean:: clean
	rm -f deps/*

