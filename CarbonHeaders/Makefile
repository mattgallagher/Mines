installsrc:
	mkdir -p $(SRCROOT)
	cp -R . $(SRCROOT)

installhdrs: install

INCLUDE=$(DSTROOT)/usr/include
CORESERVICES=$(DSTROOT)/System/Library/Frameworks/CoreServices.framework
CARBONCORE=$(DSTROOT)/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework
CARBONCORE_HEADERS=$(CARBONCORE)/Versions/A/Headers

install:
	mkdir -p $(INCLUDE)
	cp AssertMacros.h $(INCLUDE)
	cp Availability.h $(INCLUDE)
	cp AvailabilityMacros.h $(INCLUDE)
	cp AvailabilityInternal.h $(INCLUDE)
	cp TargetConditionals.h $(INCLUDE)

	mkdir -p $(CARBONCORE_HEADERS)

	ln -hfs /System/Library/Frameworks/CoreServices.framework/Frameworks/CarbonCore.framework/Headers $(INCLUDE)/CarbonCore
	ln -hfs A $(CORESERVICES)/Versions/Current
	ln -hfs A $(CARBONCORE)/Versions/Current
	
	mkdir -p $(CORESERVICES)/Versions/Current/Headers

	ln -hfs Versions/Current/Frameworks $(CORESERVICES)/Frameworks
	ln -hfs Versions/Current/Headers $(CORESERVICES)/Headers
	ln -hfs Versions/Current/Headers $(CARBONCORE)/Headers

	cp Aliases.h $(CARBONCORE_HEADERS)
	cp ConditionalMacros.h $(CARBONCORE_HEADERS)
	cp DateTimeUtils.h $(CARBONCORE_HEADERS)
	cp Files.h $(CARBONCORE_HEADERS)
	cp Finder.h $(CARBONCORE_HEADERS)
	cp MacMemory.h $(CARBONCORE_HEADERS)
	cp MacErrors.h $(CARBONCORE_HEADERS)
	cp MacTypes.h $(CARBONCORE_HEADERS)
	cp MixedMode.h $(CARBONCORE_HEADERS)
	cp OSUtils.h $(CARBONCORE_HEADERS)
	cp TextCommon.h $(CARBONCORE_HEADERS)
	cp UTCUtils.h $(CARBONCORE_HEADERS)

