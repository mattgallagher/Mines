// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	PP_MacHeaders.c				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	A variation of the standard MacHeaders.c designed for use with
//	PowerPlant programs.

/*
 *	Required for c-style toolbox glue functions: c2pstr and p2cstr
 *	(the inverse operation (pointers_in_A0) is performed at the end...)
 */

#if __MC68K__ && !__CFM68K__
	#pragma d0_pointers on
#endif

/*
 *	Metrowerks-specific definitions
 *
 *	These definitions are commonly used but not in Apple's headers. We define
 *	them in our precompiled header so we can use the Apple headers without
 *  modification.
 */

#ifndef PtoCstr
	#define PtoCstr		p2cstr
#endif

#ifndef CtoPstr
	#define CtoPstr		c2pstr
#endif

#ifndef PtoCString
	#define PtoCString	p2cstr
#endif

#ifndef CtoPString
	#define CtoPString	c2pstr
#endif

#ifndef topLeft
	#define topLeft(r)	(((Point *) &(r))[0])
#endif

#ifndef botRight
	#define botRight(r)	(((Point *) &(r))[1])
#endif

#ifndef TRUE
	#define TRUE		true
#endif
#ifndef FALSE
	#define FALSE		false
#endif

/*
 *	Apple Universal Headers 3.3.1
 *
 *	Uncomment any additional #includes you want to add to your MacHeaders.
 *
 *  Update your code to use these newer headers introduced with UI 3.2

	#include <Desk.h>				Use Meuns.h, Devices.h and Events.h
	#include <FragLoad.h>			Use CodeFragments.h
	#include <GestaltEqu.h>			Use Gestalt.h
	#include <Language.h>			Use Script.h
	#include <Memory.h>				Use MacMemory.h
	#include <MP.h>					Use Multiprocessing.h
	#include <OSEvents.h>			Use Events.h
	#include <Picker.h>				Use ColorPicker.h
	#include <Strings.h>			Use TextUtils.h
 	#include <Types.h>				Use MacTypes.h
 	#include <Windows.h>			Use MacWindows.h

 *	Updates introduced with UI 3.3 and 3.3.1 (mostly OT related)

 	#include <cred.h>				Use OpenTransportProtocol.h
 	#include <dlpi.h>				Use OpenTransportProtocol.h
	#include <DriverSupport.h>		Use DriverServices.h
 	#include <Errors.h>				Use MacErrors.h
 	#include <Interrupts.h>			Use DriverServices.h
	#include <Kernel.h>				Use DriverServices.h
	#include <miioccom.h>			Use OpenTransportProtocol.h
	#include <mistream.h>			Use OpenTransportProtocol.h
	#include <modnames.h>			Use OpenTransportProtocol.h
	#include <OpenTptAppleTalk.h>	Use OpenTransportProviders.h
	#include <OpenTptClient.h>		Use OpenTransportProtocol.h
	#include <OpenTptCommon.h>		Use OpenTransportProtocol.h
	#include <OpenTptConfig.h>		Use OpenTransportProtocol.h
	#include <OpenTptDevLinks.h>	Use OpenTransportProviders.h
	#include <OpenTptInternet.h>	Use OpenTransportProviders.h
	#include <OpenTptISDN.h>		Use OpenTransportProviders.h
	#include <OpenTptLinks.h>		Use OpenTransportProviders.h
	#include <OpenTptModule.h>		Use OpenTransportKernel.h
	#include <OpenTptPCISupport.h>	Use OpenTransportKernel.h
	#include <OpenTptSerial.h>		Use OpenTransportProviders.h
	#include <OpenTptXTI.h>			Use OpenTransportUNIX.h
	#include <OTDebug.h>			Use OpenTransport.h
	#include <OTSharedLibs.h>		Use OpenTransportProtocol.h
	#include <strlog.h>				Use OpenTransportProtocol.h
	#include <stropts.h>			Use OpenTransportProtocol.h and OpenTransportUNIX.h
	#include <strstat.h>			Use OpenTransportProtocol.h
	#include <tihdr.h>				Use OpenTransportProtocol.h

 */

/*	#include <ADSP.h> */
/*	#include <ADSPSecure.h> */
	#include <AEDataModel.h>
	#include <AEObjects.h>
	#include <AEPackObject.h>
	#include <AERegistry.h>
	#include <AEUserTermTypes.h>
/*	#include <AIFF.h> */
	#include <Aliases.h>
	#include <Appearance.h>
	#include <AppleEvents.h>
	#include <AppleGuide.h>
	#include <AppleScript.h>
	#include <AppleTalk.h>
	#include <ASDebugging.h>
	#include <ASRegistry.h>
/*	#include <ATA.h> */
/*	#include <ATSLayoutTypes.h>			New for 3.2 */
/*	#include <ATSUnicode.h>				New for 3.2 */
/*	#include <AVComponents.h> */
/*  #include <AVLTree.h>				New for 3.3.1 */
	#include <Balloons.h>
/*	#include <CardServices.h> */
/*	#include <CMAcceleration.h> */
	#include <CMApplication.h>
/*	#include <CMCalibrator.h> */
/*	#include <CMComponent.h> */
/*	#include <CMConversions.h> */
	#include <CMICCProfile.h>
/*	#include <CMMComponent.h> */
/*	#include <CMPRComponent.h> */
/*	#include <CMScriptingPlugin.h> */
	#include <CodeFragments.h>
	#include <Collections.h>
	#include <ColorPicker.h>
/*	#include <ColorPickerComponents.h> */
/*	#include <CommResources.h> */
	#include <Components.h>
	#include <ConditionalMacros.h>
/*	#include <Connections.h> */
/*	#include <ConnectionTools.h> */
	#include <ControlDefinitions.h>		/* New for 3.3.1 */
	#include <Controls.h>
/*	#include <ControlStrip.h> */

/*  #include <CoreFoundation/CFArray.h>	New for 3.3.1 */
/*  #include <CoreFoundation/CFBag.h> */
/*  #include <CoreFoundation/CFBase.h> */
/*	#include <CoreFoundation/CFBundle.h> */
/*  #include <CoreFoundation/CFCharacterSet.h> */
/*  #include <CoreFoundation/CFData.h> */
/*  #include <CoreFoundation/CFDate.h> */
/*  #include <CoreFoundation/CFDictionary.h> */
/*  #include <CoreFoundation/CFNumber.h> */
/*  #include <CoreFoundation/CFPlugIn.h> */
/*  #include <CoreFoundation/CFPreferences.h> */
/*  #include <CoreFoundation/CFPropertyList.h> */
/*  #include <CoreFoundation/CFSet.h> */
/*  #include <CoreFoundation/CFString.h> */
/*  #include <CoreFoundation/CFStringEncodingExt.h> */
/*  #include <CoreFoundation/CFTimeZone.h> */
/*  #include <CoreFoundation/CFURL.h> */
/*  #include <CoreFoundation/CFURLAccess.h> */
/*  #include <CoreFoundation/CoreFoundation.h> */

/*	#include <CRMSerialDevices.h> */
/*	#include <CryptoMessageSyntax.h> 	New for 3.3.1 */
/*	#include <CTBUtilities.h> */
/*	#include <CursorDevices.h> */
/*	#include <DatabaseAccess.h> */
	#include <DateTimeUtils.h>
/*  #include <Debugging.h>				New for 3.3.1 */
/*	#include <DeskBus.h> */
/*  #include <DesktopPrinting.h>		New for 3.3.1 */
	#include <Devices.h>
	#include <Dialogs.h>
/*	#include <Dictionary.h> */
/*	#include <DigitalSignature.h> */
	#include <DiskInit.h>
/*	#include <Disks.h> */
	#include <Displays.h>
	#include <Drag.h>
/*	#include <DrawSprocket.h> */
	#include <DriverFamilyMatching.h>
/*	#include <DriverGestalt.h> */
/*	#include <DriverServices.h> */
/*	#include <DriverSynchronization.h>	New for 3.2 */
/*	#include <Editions.h> */
	#include <Endian.h>
/*	#include <ENET.h> */
	#include <EPPC.h>
	#include <Events.h>
/*	#include <fenv.h> */
	#include <Files.h>
/*  #include <FileSigning.h>			New for 3.3.1 */
/*	#include <FileTransfers.h> */
/*	#include <FileTransferTools.h> */
	#include <FileTypesAndCreators.h>
/*	#include <FindByContent.h>			New for 3.2 */
	#include <Finder.h>
	#include <FinderRegistry.h>
	#include <FixMath.h>
	#include <Folders.h>
	#include <Fonts.h>
/*  #include <FontSync.h>				New for 3.3.1 */
/*	#include <fp.h> */
/*	#include <FSM.h> */
	#include <Gestalt.h>
/*	#include <GXEnvironment.h> */
/*	#include <GXErrors.h> */
/*	#include <GXFonts.h> */
/*	#include <GXGraphics.h> */
/*	#include <GXLayout.h> */
/*	#include <GXMath.h> */
/*	#include <GXMessages.h> */
/*	#include <GXPrinterDrivers.h> */
/*	#include <GXPrinting.h> */
/*	#include <GXTypes.h> */
	#include <HFSVolumes.h>
/*  #include <HID.h>					New for 3.3.1 */
/*  #include <HTMLRendering.h>			New for 3.3.1 */
/*	#include <HyperXCmd.h> */
/*  #include <IAExtractor.h>			New for 3.3.1 */
	#include <Icons.h>
/*	#include <ImageCodec.h> */
	#include <ImageCompression.h>
/*	#include <InputSprocket.h> */
/*  #include <InternetConfig.h>			New for 3.3.1 */
	#include <IntlResources.h>
/*	#include <JManager.h> */
/*  #include <Keychain.h>				New for 3.3.1 */
/*  #include <LanguageAnalysis.h>		New for 3.3.1 */
	#include <Lists.h>
/*	#include <LocationManager.h> */
	#include <LowMem.h>
    #include <MacErrors.h>				/* New for 3.3.1 */
	#include <MacHelp.h>				/* New for 3.3.1 */
/*	#include <MachineExceptions.h> */
/*  #include <MacLocales.h>				New for 3.3.1 */
	#include <MacMemory.h>
/*	#include <MacTCP.h> */
/*  #include <MacTextEditor.h>			New for 3.3.1 */
	#include <MacTypes.h>
	#include <MacWindows.h>
/*	#include <Math64.h> */
/*	#include <MediaHandlers.h> */
	#include <Menus.h>
/*	#include <MIDI.h> */
	#include <MixedMode.h>
	#include <Movies.h>
/*	#include <MoviesFormat.h> */
/*	#include <Multiprocessing.h> */
	#include <NameRegistry.h>
	#include <Navigation.h>				/* New for 3.2	 */
/*	#include <NetSprocket.h> */
/*  #include <NetworkSetup.h>			New for 3.3.1 */
	#include <Notification.h>
/*  #include <NSL.h>					New for 3.3.1 */
	#include <NumberFormatting.h>
/*	#include <OCE.h> */
/*	#include <OCEAuthDir.h> */
/*	#include <OCEErrors.h> */
/*	#include <OCEMail.h> */
/*	#include <OCEMessaging.h> */
/*	#include <OCEStandardDirectory.h> */
/*	#include <OCEStandardMail.h> */
/*	#include <OCETemplates.h> */

/*  #include <OpenTptGlobalNew.h>		New for 3.3.1 */
/*  #include <OpenTransport.h> */
/*  #include <OpenTransportKernel.h> */
/*  #include <OpenTransportProtocol.h> */
/*  #include <OpenTransportProviders.h> */
/*  #include <OpenTransportUNIX.h> */

	#include <OSA.h>
	#include <OSAComp.h>
	#include <OSAGeneric.h>
	#include <OSUtils.h>
/*	#include <Packages.h> */
	#include <Palettes.h>
	#include <Patches.h>
/*	#include <PCCard.h> */
/*  #include <PCCardAdapterPlugin.h>	New for 3.3.1 */
/*	#include <PCCardEnablerPlugin.h> */
/*	#include <PCCardTuples.h> */
/*	#include <PCI.h> */
/*	#include <PEFBinaryFormat.h> */
/*	#include <PictUtil.h> */
/*	#include <PictUtils.h> */
/*  #include <PLStringFuncs.h>			New for 3.3.1 */
/*  #include <PMApplication.h>			New for 3.3.1 */
/*	#include <Power.h> */
	#include <PPCToolbox.h>
	#include <Printing.h>
	#include <Processes.h>
/*	#include <QD3D.h> */
/*	#include <QD3DAcceleration.h> */
/*	#include <QD3DCamera.h> */
/*	#include <QD3DController.h> */
/*	#include <QD3DCustomElements.h> */
/*	#include <QD3DDrawContext.h> */
/*	#include <QD3DErrors.h> */
/*	#include <QD3DExtension.h> */
/*	#include <QD3DGeometry.h> */
/*	#include <QD3DGroup.h> */
/*	#include <QD3DIO.h> */
/*	#include <QD3DLight.h> */
/*	#include <QD3DMath.h> */
/*	#include <QD3DPick.h> */
/*	#include <QD3DRenderer.h> */
/*	#include <QD3DSet.h> */
/*	#include <QD3DShader.h> */
/*	#include <QD3DStorage.h> */
/*	#include <QD3DString.h> */
/*	#include <QD3DStyle.h> */
/*	#include <QD3DTransform.h> */
/*	#include <QD3DView.h> */
/*	#include <QD3DViewer.h> */
/*	#include <QD3DWinViewer.h> */
	#include <QDOffscreen.h>
/*	#include <QTML.h> */
/*  #include <QTSMovie.h>				New for 3.3.1 */
/*  #include <QTStreamingComponents.h>	New for 3.3.1 */
	#include <Quickdraw.h>
	#include <QuickdrawText.h>
/*	#include <QuickTimeComponents.h> */
/*	#include <QuickTimeMusic.h> */
/*  #include <QuickTimeStreaming.h>		New for 3.3.1 */
/*	#include <QuickTimeVR.h> */
/*	#include <QuickTimeVRFormat.h> */
/*	#include <RAVE.h> */
/*	#include <RAVESystem.h>				New for 3.2 */
	#include <Resources.h>
/*	#include <Retrace.h> */
/*	#include <ROMDefs.h> */
/*	#include <ScalerStreamTypes.h> */
/*	#include <ScalerTypes.h> */
	#include <Scrap.h>
	#include <Script.h>
/*	#include <SCSI.h> */
	#include <SegLoad.h>
/*	#include <Serial.h> */
/*	#include <SFNTLayoutTypes.h> */
/*	#include <SFNTTypes.h> */
/*	#include <ShutDown.h> */
/*	#include <Slots.h> */
/*	#include <SocketServices.h> */
	#include <Sound.h>
/*	#include <SoundComponents.h> */
/*	#include <SoundInput.h> */
/*	#include <SoundSprocket.h> */
/*	#include <Speech.h> */
/*	#include <SpeechRecognition.h> */
/*	#include <SpeechSynthesis.h> */
	#include <StandardFile.h>
/*	#include <Start.h>	 */
	#include <StringCompare.h>
/*	#include <Telephones.h> */
/*	#include <Terminals.h> */
/*	#include <TerminalTools.h> */
	#include <TextCommon.h>
	#include <TextEdit.h>
/*	#include <TextEncodingConverter.h> */
/*	#include <TextServices.h> */
	#include <TextUtils.h>
	#include <Threads.h>
	#include <Timer.h>
	#include <ToolUtils.h>
/*	#include <Translation.h> */
/*	#include <TranslationExtensions.h> */
	#include <Traps.h>
/*	#include <TSMTE.h> */
/*	#include <Unicode.h> */
/*	#include <UnicodeConverter.h> */
/*	#include <UnicodeUtilities.h> */
/*  #include <URLAccess.h>				New for 3.3.1 */
/*  #include <USB.h>					New for 3.3.1 */
	#include <UTCUtils.h>				/* New for 3.3.1 */
	#include <Video.h>
/*	#include <VideoServices.h> */
/*	#include <WorldScript.h> */
/*	#include <ZoomedVideo.h> */


/*
 *	Required for c-style toolbox glue functions: c2pstr and p2cstr
 *	(matches the inverse operation at the start of the file...)
 */

#if __MC68K__ && !__CFM68K__
 #pragma d0_pointers reset
#endif
