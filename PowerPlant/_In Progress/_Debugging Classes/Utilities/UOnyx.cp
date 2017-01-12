// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UOnyx.cp					PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	A collection of simple utilities for QC and Spotlight, from Onyx
//	Technology <http://www.onyx-tech.com/>
//
//	Utilizing these utilities requires QC, the QC API, Spotlight, the
//	Spotlight API, and/or some combination therein. These products are
//	available from Onyx directly, many resellers, and demos may also be
//	available on the CodeWarrior CDs in the "Cool Tools, Demos, & SDKs"
//	folder.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UOnyx.h>
#include <LString.h>
#include <PP_DebugMacros.h>

#if PP_QC_Support

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//	¥ Spotlight support
// ===========================================================================
#pragma mark ** Spotlight
#pragma mark -

// All Spotlight support is located in the header file


// ===========================================================================
//	¥ QC
// ===========================================================================
#pragma mark ** QC
#pragma mark -

QCCallBackUPP	UQC::gQCErrorHandlerUPP = nil;

// ---------------------------------------------------------------------------
//	¥ IsInstalled
// ---------------------------------------------------------------------------
//	Determines if QC is installed or not.

bool
UQC::IsInstalled()
{
	QCErr	err = QCInstalled();

	return (err == kQCNoErr);
}


// ---------------------------------------------------------------------------
//	¥ IsActive
// ---------------------------------------------------------------------------
//	Determines if QC is currently active or not

bool
UQC::IsActive()
{
	return (IsInstalled() && QCIsActive());
}


// ---------------------------------------------------------------------------
//	¥ Activate
// ---------------------------------------------------------------------------
//	Activates QC on the given heap zone. Passing nil defaults to this
//	application's heap.

bool
UQC::Activate(
	const THz	inHeapZone)
{
	bool	activated = false;

	if (IsInstalled()) {
		QCErr err = QCActivate(inHeapZone);
		ThrowIfQCErr_(err);

		activated = true;
	}

	return activated;
}


// ---------------------------------------------------------------------------
//	¥ Deactivate
// ---------------------------------------------------------------------------
//	Deactivates QC.

bool
UQC::Deactivate()
{
	bool deactivated = false;

	if (IsInstalled()) {
		QCErr err = QCDeactivate();
		ThrowIfQCErr_(err);

		deactivated = true;
	}

	return deactivated;
}


// ---------------------------------------------------------------------------
//	¥ GetTestingHeap
// ---------------------------------------------------------------------------
//	Returns the heap zone that QC is currently testing. If no testing
//	is occuring, returns nil.

THz
UQC::GetTestingHeap()
{
	THz	theZone = nil;
	if (IsInstalled()) {
		theZone = QCTestingHeap();
	}

	return theZone;
}


// ---------------------------------------------------------------------------
//	¥ GetState
// ---------------------------------------------------------------------------
//	Obtains the QC State. Caller is responsible for disposing of the
//	QCStateHandle with QCDisposeState. Do not assume anything nor manipulate
//	the contents of the QCStateHandle as the format is internal and subject
//	to change (according to the QC API Documentation).

QCStateHandle
UQC::GetState()
{
		// QC has to be active to get the state
	StQCActivate	activate;

	return QCGetState();
}


// ---------------------------------------------------------------------------
//	¥ SetState
// ---------------------------------------------------------------------------
//	Set the QC State to the given state. Does not dispose of the QCStateHandle
//	that it is passed.

void
UQC::SetState(
	const QCStateHandle	inStateH)
{
	if (IsInstalled()) {
		StQCActivate	activate;

		QCErr err = QCSetState(inStateH);
		ThrowIfQCErr_(err);
	} else {
		ThrowIfQCErr_(kQCNotInstalled);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetTestState
// ---------------------------------------------------------------------------
//	Obtain the state (on/off) of the given QC Test (QCTypes are denoted
//	in QCAPI.h).

bool
UQC::GetTestState(
	QCType	inTestSelector)
{
	bool	active = false;

	if (IsInstalled()) {
		SInt32	result;
		QCErr err = QCGetTestState(inTestSelector, &result);
		ThrowIfQCErr_(err);

		active = static_cast<bool>(result);
	}

	return active;
}


// ---------------------------------------------------------------------------
//	¥ SetTestState
// ---------------------------------------------------------------------------
//	Set the state of the given QC Test on or off. QC must be active to
//	set the state, so we will optionally turn it on for you. If you choose
//	to have it turned on automatically (the default), it is not turned off
//	(a handy side-effect).

bool
UQC::SetTestState(
	QCType	inTestSelector,
	bool	inState,
	bool	inActivateIfOff)
{
	bool	success = false;

	if (IsInstalled()) {
		if ((IsActive() == false) && inActivateIfOff) {
			if (Activate() == false) {
					// Didn't activate... this will fail
				ThrowIfQCErr_(kQCNotActive);
				return success;
			}
		}

		QCErr err = QCSetTestState(inTestSelector, static_cast<long>(inState));
		ThrowIfQCErr_(err);

		success = true;
	}

	return success;
}


// ---------------------------------------------------------------------------
//	¥ CheckHeap
// ---------------------------------------------------------------------------
//	Performs a heap check. Optionally will perform a heap check on the
//	system heap as well (default will not).

void
UQC::CheckHeap(
	bool	inAlsoCheckSysHeap)
{
	if (IsInstalled()) {
		StQCTestState	turnHeapOn(qcCheckHeap, true);
		StQCTestState	turnSysOn(qcCheckSystemHeap, inAlsoCheckSysHeap);

		QCErr err = QCHeapCheckNow();
		ThrowIfQCErr_(err);
	}
}


// ---------------------------------------------------------------------------
//	¥ ScrambleHeap
// ---------------------------------------------------------------------------
//	Scrambles the heap. Optionally scrambles the system heap as well (defaults
//	to not).

void
UQC::ScrambleHeap(
	bool	inAlsoScrambleSysHeap)
{
	if (IsInstalled()) {
		StQCTestState	turnHeapOn(qcScrambleHeap, true);
		StQCTestState	turnSysOn(qcCheckSystemHeap, inAlsoScrambleSysHeap);

		QCErr err = QCScrambleHeapNow();
		ThrowIfQCErr_(err);
	}
}


// ---------------------------------------------------------------------------
//	¥ BoundsCheck
// ---------------------------------------------------------------------------
//	Performs a bounds check on the heap. Optionally performs the check on
//	the system heap as well (defaults to not)

void
UQC::BoundsCheck(
	bool	inAlsoCheckSysHeap)
{
	if (IsInstalled()) {
		StQCTestState	turnHeapOn(qcBlockBoundsChecking, true);
		StQCTestState	turnSysOn(qcCheckSystemHeap, inAlsoCheckSysHeap);

		QCErr err = QCBlockBoundsCheckNow();
		ThrowIfQCErr_(err);
	}
}


// ---------------------------------------------------------------------------
//	¥ ErrorHandler
// ---------------------------------------------------------------------------
//	A custom error handler for QC that works with PowerPlant error handling
//	mechanisms. Must be manually installed (i.e. not installed automatically
//	for you); provided as a convenience.

Boolean
UQC::ErrorHandler(
	const QCPBPtr	inPB)
{
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	long	oldA5 = ::SetA5(inPB->data);
#endif

		// We just want to report what we found....
	SignalString_((unsigned char*)inPB->errString);

#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	::SetA5(oldA5);
#endif

	return false;	// false, didn't handle error. true, we handled error and QC shouldn't.
}


// ---------------------------------------------------------------------------
//	¥ InstallErrorHandler
// ---------------------------------------------------------------------------
//	Installs the given QC error handler as a C (not pascal) error handler.
//	Removes the previous error handler (if any) and returns it.
//
//	If nil is passed, our default QC ErrorHandler is utilized.

QCCallBackUPP
UQC::InstallErrorHandler(
	QCCallBackUPP	inHandler)
{
	if (!IsInstalled()) {
		SignalStringLiteral_("QC not installed.");
		return nil;
	}

	QCErr			err				= kQCNoErr;
	QCCallBackUPP	previousHandler	= gQCErrorHandlerUPP;

	RemoveErrorHandler();

	if (inHandler == nil) {
		inHandler = NewQCCallBackProc(ErrorHandler);
	}

	ThrowIfNil_(inHandler);
	gQCErrorHandlerUPP = inHandler;

	err = QCInstallHandler(gQCErrorHandlerUPP, ::SetCurrentA5());
	ThrowIfQCErr_(err);

	return previousHandler;
}


// ---------------------------------------------------------------------------
//	¥ RemoveErrorHandler
// ---------------------------------------------------------------------------
//	Removes the "default" error handler.

void
UQC::RemoveErrorHandler()
{
	if (IsInstalled() && (gQCErrorHandlerUPP != nil)) {
		QCErr err = QCRemoveHandler(gQCErrorHandlerUPP);
		ThrowIfQCErr_(err);
		DisposeRoutineDescriptor(gQCErrorHandlerUPP);
		gQCErrorHandlerUPP = nil;
	}
}


// ---------------------------------------------------------------------------
//	¥ AlertThrowAt
// ---------------------------------------------------------------------------
//	Display an Alert Box when a Throw occurs. Used by the ThrowIfQCErr_()
//	macro -- essentially the same as UDebugging::AlertThrowAt(), but a little
//	extra processing to get the QCErr in readable text instead of a number.

void
UQC::AlertThrowAt(
	QCErr				inErr,
	ConstStr255Param	inFile,
	long				inLine)
{
	LStr255	errorText(inErr);

	if (IsInstalled()) {
		Str255 qcErrText;
		qcErrText[0] = 0;
		QCGetErrorText(inErr, qcErrText);

		if (qcErrText[0] > 0) {
			errorText += StringLiteral_(" (");
			errorText += qcErrText;
			errorText += ')';
		}
	} else {
		errorText += StringLiteral_(" (This is a QC error)");
	}

	if (UDebugging::GetDebugThrow() == debugAction_Alert) {
		UDebugging::AlertThrowAt(errorText, "", inFile, inLine);
	} else if (UDebugging::GetDebugThrow() == debugAction_Debugger) {
		::DebugStr(errorText);
	}
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ QC Stack-based classes
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//	¥ StQCActivate
// ---------------------------------------------------------------------------
//	Activates QC on the specified heap (passing nil defaults to the current
//	application heap). Saves the previous active heap (if any) and restores
//	it upon deactivation. A "traditional save/change/restore" setup.

StQCActivate::StQCActivate(
	const THz		inHeapZone)
{
	mSaveHeap = UQC::GetTestingHeap();

	UQC::Activate(inHeapZone);	// Always activate, since we might be switching zones
}


StQCActivate::~StQCActivate()
{
	try {
		UQC::Deactivate();

		if (mSaveHeap != nil) {
			UQC::Activate(mSaveHeap);
		}

	} catch (...) {
		// Don't let exceptions propagate
	}
}


// ---------------------------------------------------------------------------
//	¥ StQCState
// ---------------------------------------------------------------------------
//	Save/restore the QC State. Sets the state to the given state. If the
//	given state is not nil, has the side effect of activating QC (since this
//	must be done to manipulate the state). The active state is also saved
//	and restored internally, so it should not have adverse effects on code.

StQCState::StQCState(
	const QCStateHandle	inStateH)
{
		// Save off the old state
	mActivateState = nil;
	mStateH = UQC::GetState();

	if (inStateH != nil) {

			// Make sure QC is active.
		mActivateState = new StQCActivate;
		ValidateSimpleObject_(mActivateState);

		UQC::SetState(inStateH);
	}
}


StQCState::~StQCState()
{
	try {
		if (mStateH != nil) {
			UQC::SetState( mStateH );
			ThrowIfQCErr_(QCDisposeState(mStateH));	// Clean up

			DisposeOfSimple_(mActivateState);	// Reset the previous state
		}
	} catch (...) {
		// Exceptions shouldn't propagate
	}
}


// ---------------------------------------------------------------------------
//	¥ StQCTestState
// ---------------------------------------------------------------------------
//	Save/set/restore the state of a QC test. Has the side-effect of activating
//	QC, as this must be done to set a test state (will be reset to the
//	previous active state in the destructor).

StQCTestState::StQCTestState(
	QCType	inTestSelector,
	bool	inState)
{
	mSelector = inTestSelector;
	mActivateState = nil;

	mState = UQC::GetTestState(mSelector);

	mActivateState = new StQCActivate;
	ValidateSimpleObject_(mActivateState);

	UQC::SetTestState(mSelector, inState, true);
}


StQCTestState::~StQCTestState()
{
	try {
		UQC::SetTestState(mSelector, mState, true);
	} catch (...) {
		// Don't let the exception propagate
	}

	DisposeOfSimple_(mActivateState);
}

PP_End_Namespace_PowerPlant

#endif // PP_QC_Support
