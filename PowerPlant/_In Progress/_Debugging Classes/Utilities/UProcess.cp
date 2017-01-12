// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UProcess.cp		   			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	A collection of utility routines for the Process Manager.
//
//	LCleanupTask (one of the networking classes) might also be of interest
//	as it provides a way to ensure anything that needs to be cleaned up
//	before terminating is performed.


#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UProcess.h>
#include <UVolume.h>
#include <UMemoryMgr.h>
#include <TArray.h>
#include <UAppleEventsMgr.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	 Constants

const	ProcessSerialNumber		PSN_NoProcess = { 0, kNoProcess };
const	ProcessSerialNumber		PSN_CurrentProcess = { 0, kCurrentProcess };


// ---------------------------------------------------------------------------

#pragma mark UProcess::AmIFront		// Declared inline in the header

#pragma mark UProcess::MakeMeFront	// Declared inline in the header


// ---------------------------------------------------------------------------
//	¥ SetFront
// ---------------------------------------------------------------------------
//	Sets the front process to the given PSN

void
UProcess::SetFront(
	const ProcessSerialNumber&	inPSN)
{
	OSErr err = ::SetFrontProcess(&inPSN);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ GetFront
// ---------------------------------------------------------------------------
//	Obtains the front process's PSN

ProcessSerialNumber
UProcess::GetFront()
{
	ProcessSerialNumber	thePSN;
	OSErr err = ::GetFrontProcess(&thePSN);
	ThrowIfOSErr_(err);

	return thePSN;
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ IsSame
// ---------------------------------------------------------------------------
//	Are the 2 given PSN's referring to the same process?

bool
UProcess::IsSame(
	const ProcessSerialNumber&	inProc1,
	const ProcessSerialNumber&	inProc2)
{
	Boolean	isSameProcess = false;
	OSErr err = ::SameProcess(&inProc1, &inProc2, &isSameProcess);
	Assert_(err == noErr);

	return ((err == noErr) && isSameProcess);
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetPSN
// ---------------------------------------------------------------------------
//	Get the ProcessSerialNumber. Takes the creator/signature of the
//	process and the file type (defaults to 'APPL').

ProcessSerialNumber
UProcess::GetPSN(
	OSType		inCreator,
	OSType		inType)
{
	ProcessSerialNumber	myPSN = PSN_NoProcess;

		// Walk the process list until we find the requested PSN

	SProcessInfo	theInfo;

	OSErr	err		= noErr;
	bool	foundIt	= false; // Assume failure

	while (not GetNextPSN(myPSN)) {
		err = ::GetProcessInformation(&myPSN, theInfo);
		Assert_(err == noErr);

		if ( (err == noErr)  &&
			 (theInfo.info.processSignature == inCreator) &&
			 (theInfo.info.processType == inType) ) {

			foundIt = true;
			break;
		}
	}

	if (foundIt == false) {
		Throw_(procNotFound);
	}

	return myPSN;
}


// ---------------------------------------------------------------------------
//	¥ GetPSN
// ---------------------------------------------------------------------------
//	Get the ProcessSerialNumber. Takes the name of the process and
//	the filetype (defaults to 'APPL').

ProcessSerialNumber
UProcess::GetPSN(
	ConstStr255Param	inName,
	OSType				inType,
	bool				inCaseSensitive,
	bool				inDiacSens)
{
	ProcessSerialNumber	myPSN = PSN_NoProcess;

		// Walk the process list until we find our PSN

	SProcessInfo	theInfo;

	OSErr	err = noErr;
	bool	foundIt = false; // Assume failure

	while (not GetNextPSN(myPSN)) {
		err = ::GetProcessInformation(&myPSN, theInfo);
		Assert_(err == noErr);

		if ( (err == noErr)  &&
			 (theInfo.info.processType == inType)  &&
			 ::EqualString( theInfo.info.processName,
			 				inName,
			 				inCaseSensitive,
			 				inDiacSens ) ) {

			foundIt = true;
			break;
		}
	}

	if (foundIt == false) {
		Throw_(procNotFound);
	}

	return myPSN;
}


// ---------------------------------------------------------------------------
//	¥ GetMyPSN
// ---------------------------------------------------------------------------
//	Returns the actual PSN for the calling process.

ProcessSerialNumber
UProcess::GetMyPSN()
{
	ProcessSerialNumber	thePSN;
	OSErr err = ::MacGetCurrentProcess(&thePSN);
	ThrowIfOSErr_(err);

	return thePSN;
}


// ---------------------------------------------------------------------------
//	¥ GetCurrentPSN
// ---------------------------------------------------------------------------
//	Obtains the current process's PSN. If you're calling this, it will be
//	you :-) Therefore we just return PSN_CurrentProcess.
//
//	If you want the actual PSN for your application, call GetMyPSN instead.

ProcessSerialNumber
UProcess::GetCurrentPSN()
{
	return PSN_CurrentProcess;
}


// ---------------------------------------------------------------------------
//	¥ GetNextPSN
// ---------------------------------------------------------------------------
//	Obtain the ProcessSerialNumber of the next process. On input, ioPSN
//	is the "starting point" (find the next process relative to this
//	process). On output, ioPSN contains the PSN of the next process.
//	The return type specifies if we're at the end of the list or not.
//	If true, we are at the end of the process list (false if not).

bool
UProcess::GetNextPSN(
	ProcessSerialNumber&	ioPSN)
{
	bool	atEndOfList = false;

	OSErr err = ::GetNextProcess(&ioPSN);

	if (err == procNotFound) {
		if (ioPSN == PSN_NoProcess) {
				// We're at the end of the list
			atEndOfList = true;
			err = noErr;
		}
	}

	ThrowIfOSErr_(err);

	return atEndOfList;
}


// ---------------------------------------------------------------------------
//	¥ GetPreviousPSN
// ---------------------------------------------------------------------------
//	Obtain the ProcessSerialNumber of the previous process. On input, ioPSN
//	is the "starting point" (find the previous process relative to this
//	process). On output, ioPSN contains the PSN of the previous process.
//	The return type specifies if we're at the beginning of the list or not.
//	If true, we are at the beginning of the process list (false if not).

bool
UProcess::GetPreviousPSN(
	ProcessSerialNumber&	ioPSN)
{
	bool	atStartOfList = false;

		// Since the OS doesn't provide a way to do this, we'll do it
		// ourselves.

		// First, make our own list of the running processes.
	ProcessSerialNumber	thePSN = PSN_NoProcess;

	TArray<ProcessSerialNumber>		theProcList;

	while (!GetNextPSN(thePSN)) {
		theProcList.AddItem(thePSN);
	}

		// Locate the "index" process
	ArrayIndexT	theIndex = theProcList.FetchIndexOf(ioPSN);
	if (theIndex <= LArray::index_Bad) {
		Throw_(procNotFound);
			// Throw short-circuits the search
	}

		// And get the previous PSN
	if (theIndex == 1) {
			// We're at the beginning already, so there is no
			// previous process.
		atStartOfList = true;
	} else {
			// We've got more than one process, so we'll find
			// the previous
		--theIndex;
	}

	theProcList.FetchItemAt(theIndex, ioPSN);

	return atStartOfList;
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Wake
// ---------------------------------------------------------------------------
//	Wakes up the specified process.

void
UProcess::Wake(
	const ProcessSerialNumber&	inPSN)
{
	OSErr err = ::WakeUpProcess(&inPSN);
	ThrowIfOSErr_(err);
}

	// Declared inline in the header
#pragma mark UProcess::WakeMe
#pragma mark UProcess::WakeFront


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetInfo
// ---------------------------------------------------------------------------
//	Returns information about the specified process.

void
UProcess::GetInfo(
	const ProcessSerialNumber&	inPSN,
	SProcessInfo&				outProcInfo)
{
	OSErr err = ::GetProcessInformation(&inPSN, (ProcessInfoRecPtr)&outProcInfo);
	ThrowIfOSErr_(err);
}


	// Declared inline in header
#pragma mark UProcess::GetMyInfo
#pragma mark UProcess::GetFrontInfo

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Kill
// ---------------------------------------------------------------------------
//	Attempts to terminate the specified process

void
UProcess::Kill(
	const ProcessSerialNumber&	inPSN)
{
	StAEDescriptor	killAE(typeProcessSerialNumber, &inPSN, sizeof(ProcessSerialNumber));

	AppleEvent	theAE;
	OSErr err = ::AECreateAppleEvent(	kCoreEventClass,
										kAEQuitApplication,
										killAE,
										kAutoGenerateReturnID,
										kAnyTransactionID,
										&theAE );
	ThrowIfOSErr_(err);

	UAppleEventsMgr::SendAppleEvent(theAE);
}

	// Declared inline in header
#pragma mark UProcess::KillMe
#pragma mark UProcess::KillFront
#pragma mark UProcess::KillAll


// ---------------------------------------------------------------------------
//	¥ KillAllButMe
// ---------------------------------------------------------------------------
//	Attempts to terminate all running processes but calling process (and
//	optionally the Finder).

void
UProcess::KillAllButMe(
	bool	inKillFinder)
{
		// Walk the process list terminating as we go along.
		// However, we will not terminate the Finder until last
		// just in case "puppet strings" need to be pulled.

		// Obtain the Finder's PSN
	ProcessSerialNumber finderPSN = PSN_NoProcess;
	try {
			// It's doubtful the Finder won't be running, but in
			// case it is not....
		StDisableDebugThrow_();
		finderPSN = GetPSN(FOUR_CHAR_CODE('MACS'), FOUR_CHAR_CODE('FNDR'));
	}

	catch (...) { }

	ProcessSerialNumber thePSN = PSN_NoProcess;
	while (!GetNextPSN(thePSN)) {

			// Check for calling process or the Finder
		if (IsSame(thePSN, GetMyPSN()) || IsSame(thePSN, finderPSN)) {
			continue;
		}

		Kill(thePSN);
	}

		// Now we can kill the Finder
	if (inKillFinder && (finderPSN != PSN_NoProcess)) {
		Kill(finderPSN);
	}
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ IsRunning
// ---------------------------------------------------------------------------
//	Given a signature (and optional filetype), determine if a given
//	process is running or not.

bool
UProcess::IsRunning(
	OSType		inCreator,
	OSType		inType)
{
	bool	isRunning = true; // Assume success

	try {

		StDisableDebugThrow_();
		GetPSN(inCreator, inType);

	}

	catch (const LException& iErr) {
		if (iErr.GetErrorCode() == procNotFound) {
			isRunning = false;
		} else {
			throw; // Something bad happened
		}
	}

	return isRunning;
}


// ---------------------------------------------------------------------------
//	¥ IsRunning
// ---------------------------------------------------------------------------
//	Given a filename, determine if a given process is running or not.

bool
UProcess::IsRunning(
	ConstStr255Param	inName,
	OSType				inType,
	bool				inCaseSensative,
	bool				inDiacSens)
{
	bool	isRunning = true; // Assume success

	try {

		StDisableDebugThrow_();
		GetPSN(inName, inType, inCaseSensative, inDiacSens);

	}

	catch (const LException& iErr) {
		if (iErr.GetErrorCode() == procNotFound) {
			isRunning = false;
		} else {
			throw; // Something bad happened
		}
	}

	return isRunning;
}


#pragma mark -

// ---------------------------------------------------------------------------
//	Launching applications
//
//	What follows are some routines to facilitate the programatic
//	launching of other applications. Much of the routines used herein
//	come from the UVolume class.
//
//	Code based upon SignatureToApp, an Apple DTS code snippet originally
//	authored by Jens Alfke in 1991. Snippet from the documentation:
//
//		To find an appropriate application on disk, SignatureToApp asks
//		each mounted volume's desktop database to find an application
//		with the desired signature. It first checks the volume containing
//		the active System file, then all other mounted volumes.
//
//		SignatureToApp cannot find applications on floppy disks since
//		volumes less than 2MB in size do not have a desktop database.
//
//		Occasionally SignatureToApp may not be able to find an appliaction
//		on a file server. This turns ou to be the fault of the desktop
//		database on the server. A lot of server volumes seem to have
//		messed-up desktop databases which may not have entries for all the
//		applications on the volume. The solution is for the administrator
//		to rebuild the desktops on the file server volumes.
//
//	Also, some code optionally uses Jim Luther/Apple DTS's fabulous
//	MoreFiles package (available freely on the net).
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//	¥ LaunchApp
// ---------------------------------------------------------------------------
//	Launch the specified application with the specified parameters. Takes
//	an FSSpec to the application (does not find the application for you)

ProcessSerialNumber
UProcess::LaunchApp(
	const FSSpec&	inFileSpec,
	LaunchFlags		inFlags)
{
	LaunchParamBlockRec	pb;

	pb.launchBlockID		= extendedBlock;
	pb.launchEPBLength		= extendedBlockLen;
	pb.launchFileFlags		= launchNoFileFlags;
	pb.launchControlFlags	= static_cast<UInt16>(inFlags | launchNoFileFlags);
	FSSpec theSpec			= inFileSpec; // constness
	pb.launchAppSpec		= &theSpec;
	pb.launchAppParameters	= nil;

	OSErr err = ::LaunchApplication(&pb);
	ThrowIfOSErr_(err);

	return pb.launchProcessSN;
}


// ---------------------------------------------------------------------------
//	¥ LaunchApp
// ---------------------------------------------------------------------------
//	Launch the specified application with the specified parameters. Takes
//	an AliasHandle to the application (does not find the application for you)

ProcessSerialNumber
UProcess::LaunchApp(
	AliasHandle		inFileSpec,
	LaunchFlags		inFlags)
{
	FSSpec	appSpec;
	Boolean	wasChanged;

	OSErr err = ::ResolveAlias(nil, inFileSpec, &appSpec, &wasChanged);
	ThrowIfOSErr_(err);

	return LaunchApp(appSpec, inFlags);
}


// ---------------------------------------------------------------------------
//	¥ Launch
// ---------------------------------------------------------------------------
//	Launch an application. Will automatically find the application for
//	you. Optionally returns the FSSpec and ProcessSerialNumber of the
//	appliaction.
//
//	Takes the signature of the application to launch.

bool
UProcess::Launch(
	OSType					inSignature,
	OSType					inType,
	LaunchFlags				inFlags,
	FSSpec*					outFSSpec,
	ProcessSerialNumber*	outPSN)
{
	bool				launched	= false;
	ProcessSerialNumber	thePSN		= PSN_NoProcess;

		// See if it's already running
	if (IsRunning(inSignature, inType)) {

		launched = true;

		thePSN = GetPSN(inSignature, inType);

		if (outPSN != nil) {
			*outPSN = thePSN;
		}

		if (outFSSpec != nil) {
			UVolume::FindApp(inSignature, *outFSSpec);
		}

			// Do you want to pull the app to the front?
		if ((inFlags & launchDontSwitch) == 0) {
			SetFront(thePSN);
		}

			// Do you want to kill ourselves in response?
		if ((inFlags & launchContinue) == 0) {
			KillMe();
		}
	} else {
		// not running.

#ifdef Debug_Signal
			// Have to special case here for the Finder... it should
			// normally always be running so we should never get here. But
			// in the off-chance it's not running, well, ::LaunchApplication()
			// can't deal with it. So we'll just have to stop short :-(
		if ((inSignature == FOUR_CHAR_CODE('MACS')) && (inType == FOUR_CHAR_CODE('FNDR'))) {
			SignalStringLiteral_( "Can't launch the Finder");
			return launched;
		}
#endif
			// Find it
		FSSpec	theSpec;
		if (UVolume::FindApp(inSignature, theSpec)) {
			LaunchApp(theSpec, inFlags);
			launched = true;
			if (outFSSpec != nil) {
				*outFSSpec = theSpec;
			}
			if (outPSN != nil) {
				*outPSN = GetPSN(inSignature);
			}
		}
	}

	return launched;
}


// ---------------------------------------------------------------------------
//	¥ Launch
// ---------------------------------------------------------------------------
//	Launch an application. Will automatically find the application for
//	you. Optionally returns the FSSpec and ProcessSerialNumber of the
//	application.
//
//	Takes the EXACT filename of the application to launch
//
//	Only available if MoreFiles is being used

#if PP_MoreFiles_Support

bool
UProcess::Launch(
	ConstStr255Param		inName,
	OSType					inType,
	LaunchFlags				inFlags,
	FSSpec*					outFSSpec,
	ProcessSerialNumber*	outPSN)
{
	bool				launched	= false;
	ProcessSerialNumber	thePSN		= PSN_NoProcess;

		// See if it's already running
	if (IsRunning(inName, inType)) {

		launched = true;

		thePSN = GetPSN(inName, inType);

		if (outPSN != nil) {
			*outPSN = thePSN;
		}

		if (outFSSpec != nil) {
			UVolume::FindApp(inName, *outFSSpec, false);
		}

			// Do you want to pull the app to the front?
		if ((inFlags & launchDontSwitch) == 0) {
			SetFront(thePSN);
		}

			// Do you want to kill ourselves in response?
		if ((inFlags & launchContinue) == 0) {
			KillMe();
		}
	} else {
		// Not running.

#ifdef Debug_Signal
			// Have to special case here for the Finder... it should
			// normally always be running so we should never get here. But
			// in the off-chance it's not running, well, ::LaunchApplication()
			// can't deal with it. So we'll just have to stop short :-(
		if (::EqualString(inName, StringLiteral_("Finder"), false, false)) {
			SignalStringLiteral_("Can't launch the Finder");
			return launched;
		}
#endif

			// Find it
		FSSpec	theSpec;
		if (UVolume::FindApp(inName, theSpec, false)) {
			LaunchApp(theSpec, inFlags);
			launched = true;
			if (outFSSpec != nil) {
				*outFSSpec = theSpec;
			}
			if (outPSN != nil) {
				*outPSN = GetPSN(inName);
			}
		}
	}

	return launched;
}

#endif // PP_MoreFiles_Support

PP_End_Namespace_PowerPlant

#pragma mark -

// ---------------------------------------------------------------------------
//	Other routines declared inline in the header
// ---------------------------------------------------------------------------

#pragma mark UProcess::ProcessManagerIsPresent
