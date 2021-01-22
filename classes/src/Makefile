include ../../Makefile.vars

ALLINC          = -I../include ${BOOSTINC} ${BZINC} ${PCAPINC}
CXXFLAGS	+= -g -Wno-deprecated-register -Wno-register
OBJFILESNP =	DwmASIO.o \
		DwmBZ2IO.o \
		DwmBase64.o \
		DwmCgi.o \
		DwmConditionVariable.o \
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
		DwmGZIO.o \
		DwmGroup.o \
		DwmIpAddress.o \
		DwmIpPrefix.o \
		DwmIpv4Address.o \
		DwmIpv4IcmpDestinationUnreachable.o \
		DwmIpv4IcmpEchoReply.o \
		DwmIpv4IcmpEchoRequest.o \
		DwmIpv4IcmpMessage.o \
		DwmIpv4IcmpRedirect.o \
		DwmIpv4IcmpSourceQuench.o \
		DwmIpv4IcmpTimeExceeded.o \
		DwmIpv4PacketHeader.o \
		DwmIpv4Prefix.o \
		DwmIpv4TcpHeader.o \
		DwmIpv4TcpPacket.o \
		DwmIpv4TcpPayload.o \
		DwmIpv4UdpHeader.o \
		DwmIpv4UdpPacket.o \
		DwmIpv4UdpPayload.o \
		DwmIpv4Utils.o \
		DwmIpv6Address.o \
		DwmIpv6Prefix.o \
		DwmLocalInterfaces.o \
		DwmMacAddress.o \
		DwmMplsLabel.o \
		DwmMplsLabelStack.o \
		DwmMutex.o \
		DwmOptArgs.o \
		DwmPacer.o \
		DwmPassword.o \
		DwmPathUtils.o \
		DwmPthreadLocker.o \
		DwmPthreadReadWriteLock.o \
		DwmPthreadSignal.o \
		DwmRegistry.o \
		DwmRusage.o \
		DwmSignal.o \
		DwmSocket.o \
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
		DwmXDRUtils.o \
		DwmXmlAttribute.o \
		DwmXmlElement.o
shlib_version = $(shell ../../getvers.sh -s)

ifneq ("${TARGETVENDOR}", "apple")
OBJFILESNP +=	DwmProcessInfo.o DwmProcessTable.o
endif
ifeq (,$(findstring iPhone,$(PLATFORM)))
OBJFILESNP +=	DwmHostPinger.o DwmPcap.o
OBJFILESNP +=	DwmPingDestination.o DwmPingDestinationEntry.o 
endif
OBJFILES      = $(OBJFILESNP:%=../obj/%)
SHARED_OBJFILES = $(OBJFILES:%.o=%.lo)
OBJDEPS         = $(OBJFILESNP:%.o=deps/%_deps)

all:: ../lib/libDwm.la

../lib/libDwm.la: ${SHARED_OBJFILES}
	${LIBTOOL} --tag=CXX --mode=link ${CXX} -o $@ $^ -rpath ${INSTALLPREFIX}/lib ${ZLIB} ${BZLIB} ${PCAPLIB} ${KVMLIB} -version-info ${shlib_version} ${LDFLAGS}

../lib/libDwm${SHARED_LIB_EXT}: ${OBJFILES}
	${CXX} ${LD_SHARED_FLAGS} -o $@ $^

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

../obj/%.lo ../obj/%.o: %.cc deps/%_deps
	${LIBTOOL} --mode=compile --tag=CXX ${CXX} ${CXXFLAGS} ${PTHREADCXXFLAGS} ${ALLINC} -c $< -o $@

%.cc: %.ll
	flex -o$@ $<

DwmXmlElementParse.cc DwmXmlElementParse.hh: DwmXmlElementParse.bb
	bison -d -o$@ $<

DwmFreeBSDPkgManifestParse.hh: DwmFreeBSDPkgManifestParse.cc

DwmFreeBSDPkgManifestParse.cc: DwmFreeBSDPkgManifestParse.yy
	bison -d -o$@ $<

install::
	../../install-sh -c -m 444 ../lib/.libs/libDwm.a ${INSTALLPREFIX}/lib/libDwm.a

uninstall: clean-install

tarprep: ${TARDIR}/lib/libDwm.la

${TARDIR}/lib/libDwm.la: ../lib/libDwm.la
	${LIBTOOL} --mode=install ../../install-sh -s -c $< ${TARDIR}/lib/libDwm.la

clean:: clean-tarprep
	${LIBTOOL} --mode=clean rm -f ../lib/libDwm.la ${OBJFILES} DwmCgi.cc \
	 DwmRegistry.cc DwmFreeBSDPkgManifestLex.cc *Parse.cc *Parse.hh \
	 DwmTimeUtil.cc ${SHARED_OBJFILES}

distclean:: clean
	rm -f deps/*_deps

clean-install::
	rm -f ${INSTALLPREFIX}/lib/libDwm.a

clean-tarprep::
	${LIBTOOL} --mode=clean rm -f ${TARDIR}/lib/libDwm.la

