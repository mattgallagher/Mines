// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UMemoryEater.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	A class that consumes memory in a controlled manner. Aids in testing
//	low memory handling situations.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UMemoryEater.h>
#include <TArrayIterator.h>
#include <UModalDialogs.h>
#include <LRadioGroupView.h>
#include <LWindow.h>
#include <PP_DebugMacros.h>

PP_Begin_Namespace_PowerPlant

	// Static variables
TArray<Handle>*				UMemoryEater::sHandleList	= nil;
TArray<Ptr>*				UMemoryEater::sPointerList	= nil;
UMemoryEater::EMemoryType	UMemoryEater::sMemoryType	= UMemoryEater::memoryType_Pointer;
Size						UMemoryEater::sMemorySize	= 10 * 1024; // 10K to start out.

	// Memory eaten is filled with this value for easy identification
	// of the eaten blocks in debuggers or utilities such as ZoneRanger.
#ifndef PP_UMemoryEater_ZapValue
	#define PP_UMemoryEater_ZapValue		0xF5D1F5D1L
#endif


// ---------------------------------------------------------------------------
//	¥ Eat									[static, public]
// ---------------------------------------------------------------------------
//	Eats a chunk of memory. Eats based upon the "last settings" (of size
//	and type).

void
UMemoryEater::Eat()
{
	switch (sMemoryType) {
		case memoryType_Pointer:
			UMemoryEater::EatPointer(sMemorySize);
			break;

		case memoryType_Handle:
			UMemoryEater::EatHandle(sMemorySize);
			break;

		default:
			SignalStringLiteral_("Unknown memory type");
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ EatPointer							[static, public]
// ---------------------------------------------------------------------------
//	Eats a Ptr (a non-relocatable block of memory) of the specified
//	size.

void
UMemoryEater::EatPointer(
	Size	inBytesToEat)
{
	if (sPointerList == nil) {
		sPointerList = new TArray<Ptr>;
	}

	ValidateObject_(sPointerList);

	Ptr thePtr = ::NewPtr(inBytesToEat);
	ThrowIfMemFail_(thePtr);
	ValidatePtr_(thePtr);

	FillBlock(thePtr, inBytesToEat, PP_UMemoryEater_ZapValue);

	sMemoryType = memoryType_Pointer;
	sMemorySize = inBytesToEat;

	sPointerList->AddItem(thePtr);
}


// ---------------------------------------------------------------------------
//	¥ EatHandle								[static, public]
// ---------------------------------------------------------------------------
//	Eats a Handle (a relocatable block of memory) of the specified size.

void
UMemoryEater::EatHandle(
	Size	inBytesToEat)
{
	if (sHandleList == nil) {
		sHandleList = new TArray<Handle>;
	}

	ValidateObject_(sHandleList);

	Handle theHandle = ::NewHandle(inBytesToEat);
	ThrowIfMemFail_(theHandle);
	ValidateHandle_(theHandle);

	FillBlock(*theHandle, inBytesToEat, PP_UMemoryEater_ZapValue);

	sMemoryType = memoryType_Handle;
	sMemorySize = inBytesToEat;

	sHandleList->AddItem(theHandle);
}


// ---------------------------------------------------------------------------
//	¥ FillBlock								[static, public]
// ---------------------------------------------------------------------------
//	Fill a block of memory with a specified pattern.

void
UMemoryEater::FillBlock(
	void*			inPtr,
	register Size	inPtrSize,
	register long	inValue)
{
	register long*	thePtr = static_cast<long*>(inPtr);
	inPtrSize /= sizeof(long);
	while (inPtrSize-- > 0) {
		*thePtr++ = inValue;
	}
}


// ---------------------------------------------------------------------------
//	¥ DeleteMemoryLists						[static, public]
// ---------------------------------------------------------------------------
//	Cleans up the "eaten memory pools". We track the memory eating and allow
//	this for cleanup if you wish to perhaps terminate low-memory testing
//	early, or perhaps to clean up to allow leak-checking utilities to not
//	report the pools as leaks.

void
UMemoryEater::DeleteMemoryLists()
{
	if (sPointerList != nil) {
		TArrayIterator<Ptr> iterate(*sPointerList);

		Ptr thePtr;
		while (iterate.Next(thePtr)) {
			sPointerList->Remove(thePtr);
			::DisposePtr(thePtr);
		}

		DisposeOf_(sPointerList);
	}

	if (sHandleList != nil) {
		TArrayIterator<Handle> iterate(*sHandleList);

		Handle theHandle;
		while (iterate.Next(theHandle)) {
			sHandleList->Remove(theHandle);
			::DisposeHandle(theHandle);
		}

		DisposeOf_(sHandleList);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetTotalPoolSize							[static, public]
// ---------------------------------------------------------------------------
//	Obtains the size of our eaten memory pools.
//
//	Not 100% accurate at reporting the totally memory used by the memory
//	eating mechanisms (there is memory used by the TArray lists, by the
//	LArray internal storage, and other such overhead).

UInt32
UMemoryEater::GetTotalPoolSize()
{
	UInt32	ptrSize		= UMemoryEater::GetPointerPoolSize();
	UInt32	handleSize	= UMemoryEater::GetHandlePoolSize();

	return ptrSize + handleSize;
}


// ---------------------------------------------------------------------------
//	¥ GetPointerPoolSize						[static, public]
// ---------------------------------------------------------------------------
//	Obtains the size of the pointer pool

UInt32
UMemoryEater::GetPointerPoolSize()
{
	UInt32	poolSize = 0;

	if (sPointerList != nil) {

		TArrayIterator<Ptr>	iterate(*sPointerList);

		Ptr thePtr;
		while (iterate.Next(thePtr)) {
			Assert_(thePtr != nil);
			poolSize += static_cast<UInt32>(::GetPtrSize(thePtr));
		}
	}

	return poolSize;
}


// ---------------------------------------------------------------------------
// ¥ GetHandlePoolSize							[static, public]
// ---------------------------------------------------------------------------
//	Obtains the size of the Handle pool.

UInt32
UMemoryEater::GetHandlePoolSize()
{
	UInt32	poolSize = 0;

	if (sHandleList != nil) {

		TArrayIterator<Handle> iterate(*sHandleList);

		Handle theHandle;
		while (iterate.Next(theHandle)) {
			Assert_(theHandle != nil);
			poolSize += static_cast<UInt32>(::GetHandleSize(theHandle));
		}
	}

	return poolSize;
}


// ---------------------------------------------------------------------------
//	¥ SetParameters								[static, public]
// ---------------------------------------------------------------------------
//	Presents the given dialog box to allow the user to specify how much
//	and what kind of memory to eat.
//
//	The arguments specify the various information needed to properly set
//	and extra information from the dialog. The radio buttons can be of
//	any radio type (e.g. LStdRadioButton, LRadioButton) and same for
//	the edit field (e.g. LEditText, LEditField). The radio group MUST
//	be an LRadioGroupView (or inherited class).
//
//	The PaneIDT's for the radio buttons should directly coorespond to
//	the related EMemoryType, e.g.
//
//		inRadioHandleID = UMemoryEater::EMemoryType::memoryType_Handle;
//
//	This is done to facilitate the logic involved in determining
//	which radio is on and therefore what memory type to eat. Using
//	other PaneIDT's could lead to unpredictable results.

bool
UMemoryEater::SetParameters(
	LCommander*	inSuper,
	bool		inEatNow,
	ResIDT		inPPobID,
	PaneIDT		inEditID,
	PaneIDT		inRadioGroupID,
	PaneIDT		inRadioHandleID,
	PaneIDT		inRadioPtrID)
{
	bool	inOk = false;	// Did the user OK or Cancel?

		// Set up the dialog
	StDialogHandler	theHandler(inPPobID, inSuper);
	LWindow*		theDialog = theHandler.GetDialog();
	Assert_(theDialog != nil);

		// Obtain pointers to panes for ease of reference. Generic
		// LPane's are used to allow the edit field and radio buttons
		// to be of varying types (LEditField/LEditText, LStdRadioButton/LRadioButton).
		// Do be careful tho as this could lead to strange behaviors if
		// the provided PaneIDT's are incorrect.

	LRadioGroupView*	theGroup		= FindPaneByID_(theDialog, inRadioGroupID, LRadioGroupView);
	LPane*				theAmount		= FindPaneByID_(theDialog, inEditID, LPane);

		// Assumes the PaneIDT's of the radio buttons correspond to the
		// equivalent EMemoryType (makes things easier)
	theGroup->SetCurrentRadioID(sMemoryType);

	theDialog->SetLatentSub(DebugCastNoThrow_(theAmount, LPane, LCommander));
	theAmount->SetValue(sMemorySize);
	theDialog->Show();

	MessageT theMessage;

	do {
		theMessage = theHandler.DoDialog();
	} while ((theMessage != msg_OK) && (theMessage != msg_Cancel));

	if (theMessage == msg_OK) {
		sMemoryType = static_cast<EMemoryType>(theGroup->GetCurrentRadioID());
		sMemorySize = theAmount->GetValue();

		if (inEatNow) {
			UMemoryEater::Eat();
		}

		inOk = true;
	}

	return inOk;
}


PP_End_Namespace_PowerPlant
