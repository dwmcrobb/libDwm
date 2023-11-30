include ../../Makefile.vars

OBJFILES = TestASIO.o \
	   TestBase64.o \
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
	   TestDwmSocket.o \
	   TestDwmSunriseSunset.o \
	   TestDwmTermios.o \
	   TestEngFormat.o \
	   TestFFIVar.o \
	   TestFileRoller.o \
	   TestGetAddrInfo.o \
	   TestGitVersion.o \
	   TestGroup.o \
	   TestHostPinger.o \
	   TestIODeque.o \
	   TestIOList.o \
	   TestIOMap.o \
	   TestIOMultiMap.o \
	   TestIOMultiSet.o \
	   TestIOSet.o \
	   TestIOVector.o \
	   TestIpv4Address.o \
	   TestIpv4AddrMap.o \
	   TestIpv4IcmpDestinationUnreachable.o \
	   TestIpv4IcmpEchoReply.o \
	   TestIpv4IcmpEchoRequest.o \
	   TestIpv4IcmpRedirect.o \
	   TestIpv4IcmpSourceQuench.o \
	   TestIpv4IcmpTimeExceeded.o \
	   TestIpv4PacketHeader.o \
	   TestIpv4Prefix.o \
	   TestIpv4PrefixMap.o \
	   TestIpv4Routes.o \
	   TestIpv4TcpHeader.o \
	   TestIpv4TcpPacket.o \
	   TestIpv4UdpHeader.o \
	   TestIpv4UdpPacket.o \
	   TestIpv6Address.o \
	   TestIpv6AddrMap.o \
	   TestIpv6Prefix.o \
	   TestIpv6PrefixMap.o \
	   TestMacAddress.o \
	   TestMplsLabel.o \
	   TestMplsLabelStack.o \
	   TestPatricia.o \
	   TestStringConverter.o \
	   TestStringUtils.o \
	   TestSvnTag.o \
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
	   TestValueRange.o \
	   TestVariantIO.o \
	   TestXDRUtils.o
ifneq ("${TARGETVENDOR}", "apple")
OBJFILES += TestProcessTable.o
endif
OBJDEPS	 = $(OBJFILES:%.o=deps/%_deps)
TESTS	 = $(OBJFILES:%.o=%)
CXXFLAGS += -g
# CXXFLAGS += -fsanitize=address
LDFLAGS  += -g
# LDFLAGS += -fsanitize=address
ALLINC   = -I../include ${BZINC} ${BOOSTINC} ${TIRPCINC}
OSLIBS   += -lxxhash ${TIRPCLIB}

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
	${LIBTOOL} --mode=link --tag=CXX ${CXX} ${LDFLAGS} -o $@ $^ ${ZLIB} ${BZLIB} ${BOOSTLIBS} ${TERMCAPLIB} ${OSLIBS} ${PTHREADLDFLAGS} ${KVMLIB} -lpcap
endif

%: %.o ../lib/libDwm.la
	${LIBTOOL} --mode=link --tag=CXX ${CXX} ${LDFLAGS} -o $@ $^ ${ZLIB} ${BZLIB} ${BOOSTLIBS} ${TERMCAPLIB} ${OSLIBS} ${PTHREADLDFLAGS} -lpcap

../lib/libDwm.la::
	${MAKE} -C ../src

clean:
	${LIBTOOL} --mode=clean rm -f ${TESTS} ${OBJFILES}

distclean:: clean
	rm -f deps/*

