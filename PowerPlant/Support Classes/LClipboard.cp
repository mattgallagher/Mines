// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LClipboard.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Manages the Clipboard
//
//	This implementation gets and sets data using the global Toolbox
//	clipboard (aka Scrap). Create a subclass if you wish to maintain a
//	private scrap to more efficiently manage the data used by your
//	program.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LClipboard.h>
#include <UMemoryMgr.h>
#include <UScrap.h>

#include <Events.h>

// ---------------------------------------------------------------------------
//	Preprocessor symbol for workaround for an annoying interaction
//	between the Scrap Manager and the MW Source Debugger. See comments
//	in GetDataSelf() for an explanation.

#ifndef PP_MW_Debug_Scrap_Workaround
	#define PP_MW_Debug_Scrap_Workaround			0	// Off by default
#endif

#if PP_MW_Debug_Scrap_Workaround
	#include <MetroNubUtils.h>
#endif

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Class Variables

LClipboard*		LClipboard::sClipboard = nil;


// ---------------------------------------------------------------------------
//	¥ LClipboard							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LClipboard::LClipboard()

	: LAttachment(msg_Event)
{
	sClipboard		= this;
	
	mScrapRef		= UScrap::GetReference();
	mImportPending	= true;
	mExportPending	= false;
}


// ---------------------------------------------------------------------------
//	¥ ~LClipboard							Destructor				  [public]
// ---------------------------------------------------------------------------

LClipboard::~LClipboard()
{
		// Depending on the circumstances when you delete a Clipboard
		// object, you may want to call ExportSelf() in your subclass's
		// destructor if mExportPending is true

	#ifdef Debug_Signal
	
		if (mExportPending) {
			SignalStringLiteral_("Custom Clipboard deleted while export is pending");
		}
		
	#endif
	
	if (sClipboard == this) {
		sClipboard = nil;
	}
}


// ---------------------------------------------------------------------------
//	¥ SetData														  [public]
// ---------------------------------------------------------------------------
//	Set the Clipboard contents to the data in a Handle
//
//	inReset specifies whether to clear the existing contents of the
//	Clipboard before storing the new data.

void
LClipboard::SetData(
	ResType		inDataType,
	Handle		inDataH,
	Boolean		inReset)
{
	StHandleLocker	lock(inDataH);
	SetData(inDataType, *inDataH, ::GetHandleSize(inDataH), inReset);
}


// ---------------------------------------------------------------------------
//	¥ SetData														  [public]
// ---------------------------------------------------------------------------
//	Set the Clipboard contents to the data specified by a pointer and length
//
//	inReset specifies whether to clear the existing contents of the
//	Clipboard before storing the new data.

void
LClipboard::SetData(
	ResType		inDataType,
	Ptr			inDataPtr,
	SInt32		inDataLength,
	Boolean		inReset)
{
	mImportPending = false;
	mExportPending = true;
	SetDataSelf(inDataType, inDataPtr, inDataLength, inReset);
}


// ---------------------------------------------------------------------------
//	¥ GetData														  [public]
// ---------------------------------------------------------------------------
//	Pass back the data in the Clipboard of the specified type in a Handle
//	and return the size of the data
//
//	If ioDataH is nil, the data is not passed back, but its size is returned.
//	Otherwise, ioDataH must be a valid Handle which is resized if necessary
//	to hold all the data.
//
//	This is a wrapper function which imports the global scrap if necessary
//	then calls a lower level function to actually retrieve the data

SInt32
LClipboard::GetData(
	ResType		inDataType,
	Handle		ioDataH)
{
	if (mImportPending) {
		ImportSelf();
	}

	return GetDataSelf(inDataType, ioDataH);
}


// ---------------------------------------------------------------------------
//	¥ ClearData													   [protected]
// ---------------------------------------------------------------------------

void
LClipboard::ClearData()
{
	UScrap::ClearData();
	
	mScrapRef = UScrap::GetReference();
}


// ---------------------------------------------------------------------------
//	¥ GetDataSelf												   [protected]
// ---------------------------------------------------------------------------
//	Pass back the data in the Clipboard of the specified type in a Handle
//	and return the size of the data
//
//	If ioDataH is nil, the data is not passed back, but its size is returned.
//	Otherwise, ioDataH must be a valid Handle which is resized if necessary
//	to hold all the data.
//
//	This implementation gets the data from the global scrap. Subclasses
//	should override to maintain a local scrap.

SInt32
LClipboard::GetDataSelf(
	ResType		inDataType,
	Handle		ioDataH)
{
	return UScrap::GetData(inDataType, ioDataH);
}


// ---------------------------------------------------------------------------
//	¥ SetDataSelf												   [protected]
// ---------------------------------------------------------------------------
//	Set the Clipboard contents to the data specified by a pointer and length
//
//	inReset specifies whether to clear the existing contents of the
//	Clipboard before storing the new data.
//
//	This implementation sets the data in the global scrap. Subclasses
//	should override to maintain a local scrap.

void
LClipboard::SetDataSelf(
	ResType		inDataType,
	Ptr			inDataPtr,
	SInt32		inDataLength,
	Boolean		inReset)
{
	UScrap::SetData(inDataType, inDataPtr, inDataLength, inReset);

	mExportPending = false;
}


// ---------------------------------------------------------------------------
//	¥ ImportSelf												   [protected]
// ---------------------------------------------------------------------------
//	Import the data in the global scrap to a local scrap
//
//	This implementation does nothing since this class uses the global
//	scrap when setting and getting clipboard data. Subclasses should
//	override if they maintain a local scrap.

void
LClipboard::ImportSelf()
{
}


// ---------------------------------------------------------------------------
//	¥ ExportSelf												   [protected]
// ---------------------------------------------------------------------------
//	Export the data in a local scrap to the global scrap
//
//	This implementation does nothing since this class uses the global
//	scrap when setting and getting clipboard data. Subclasses should
//	override if they maintain a local scrap.

void
LClipboard::ExportSelf()
{
}


// ---------------------------------------------------------------------------
//	¥ ExecuteSelf												   [protected]
// ---------------------------------------------------------------------------
//	Clipboard watches for Suspend and Resume events
//
//	inMessage will be msg_Event
//	ioParam will be an EventRecord*

void
LClipboard::ExecuteSelf(
	MessageT	/* inMessage */,
	void*		ioParam)
{
	EventRecord	*theEvent = (EventRecord*) ioParam;

	if (theEvent->what == osEvt) {
		UInt8	osEvtFlag = (UInt8) (((UInt32) theEvent->message) >> 24);

		if (osEvtFlag == suspendResumeMessage) {
			if (theEvent->message & resumeFlag) {		// Resume Event
			
					// If the scrap reference has changed since we
					// suspended, it means there's new data on the scrap.
			
				UScrap::RefType	currRef = UScrap::GetReference();
				
				mImportPending = (currRef != mScrapRef);
				mScrapRef = currRef;

			} else {									// Suspend Event
				if (mExportPending) {
					ExportSelf();
					mExportPending = false;
				}
				
				mScrapRef = UScrap::GetReference();
			}
		}
	}
}


PP_End_Namespace_PowerPlant
