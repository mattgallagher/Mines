// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UScrap.cp				PowerPlant 2.2.2		©1999-2005 Metrowerks Inc.
// ===========================================================================
//
//	The UScrap namespace contains 5 functions for managing the Scrap, also
//	known as the Clipboard. There are two separate implementations of the
//	functions. One for Classic, and the other for Carbon.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UScrap.h>
#include <UMemoryMgr.h>

// ---------------------------------------------------------------------------
//	Preprocessor symbol for workaround for an annoying interaction
//	between the Scrap Manager and the MW Source Debugger. See comments
//	in GetData() for an explanation.

#ifndef PP_MW_Debug_Scrap_Workaround
	#define PP_MW_Debug_Scrap_Workaround			0	// Off by default
#endif

#if PP_MW_Debug_Scrap_Workaround
	#include <MetroNubUtils.h>
#endif

PP_Begin_Namespace_PowerPlant


// ===========================================================================
//	Classic Implementation
// ===========================================================================

#if PP_Target_Classic
#pragma mark === Classic ===

// ---------------------------------------------------------------------------
//	¥ GetReference													  [public]
// ---------------------------------------------------------------------------
//	Return reference that uniquely identifies the scrap
//
//	Reference changes every time scrap is cleared

UScrap::RefType
UScrap::GetReference()
{
	return ::InfoScrap()->scrapCount;
}


// ---------------------------------------------------------------------------
//	¥ GetData														  [public]
// ---------------------------------------------------------------------------
//	Pass back the data in the Scrap of the specified type in a Handle
//	and return the size of the data
//
//	If ioData is nil, the data is not passed data, but its size is returned
//	Otherwise, ioData is resized if necessary to hold all the data
//
//	If ioData is nil, we do not throw an error when there is a problem getting
//	the data size. Function just returns 0.

SInt32
UScrap::GetData(
	ScrapFlavorType		inDataType,
	Handle				ioData)
{
	SInt32	offset;
	SInt32	byteCount = ::GetScrap(ioData, inDataType, &offset);

	if (byteCount == noTypeErr) {	// Requested Type not available
		byteCount = 0;

	} else if (byteCount < 0) {		// Negative byteCount is an error code

		// There is a kink in the debugger support in the Mac OS's Process
		// Manager. It is an reentrancy problem in the Process Manager's
		// scrap coercion engine. Due to this, stepping over a call to
		// ::GetScrap() can somtimes result in ::GetScrap() returning an
		// error even though it should have succeeded. Typically this error
		// is a memFullErr and seems to show up more often when graphic/bitmap
		// data is on the scrap (as opposed to other data types, like TEXT).
		//
		// This seems to only occur when running under the Metrowerks Debugger.
		// If you hit this situation, chances are it is just a side-effect
		// of this problem, and running without the MW Debug should help you
		// determine if it's a real problem or just this interaction effect.
		//
		// To work around this situation, we use the MetroNub API to see if
		// we are currently being debugged. If so, and then if the error
		// is a memFullErr, we will short-curcuit and return noErr.
		// You will need to add the MetroNubUtils.c file to your project (and
		// you may need to add an explicit access path to this file). You
		// will also need to #define PP_MW_Debug_Scrap_Workaround 1 somewhere
		// in your project (prefix file, precompiled header, etc.).

	#if	PP_MW_Debug_Scrap_Workaround
		if ( AmIBeingMWDebugged() && (byteCount == memFullErr) ) {

			return 0;				// Probably hit the kink. Just ignore it
		}
	#endif

		if (ioData != nil) {			// Error getting scrap. Throw only
			ThrowOSErr_(byteCount);		//   if caller wants the data.
		}
	}

	return byteCount;
}


// ---------------------------------------------------------------------------
//	¥ HasData                                                         [public]
// ---------------------------------------------------------------------------
//	Return whether the specified flavor is on the Scrap

bool
UScrap::HasData(
	ScrapFlavorType		inDataType)
{
	SInt32	offset;
	SInt32	byteCount = ::GetScrap(nil, inDataType, &offset);

	return (byteCount > 0);
}


// ---------------------------------------------------------------------------
//	¥ SetData														  [public]
// ---------------------------------------------------------------------------
//	Set the Scrap contents to the data specified by a pointer and length
//
//	inClear specifies whether to clear the existing contents of the
//	Clipboard before storing the new data.

void
UScrap::SetData(
	ScrapFlavorType		inDataType,
	const void*			inDataPtr,
	SInt32				inDataLength,
	bool				inClear)
{
	if (inClear) {
		UScrap::ClearData();
	}

	ThrowIfOSErr_( (OSErr) ::PutScrap(inDataLength, inDataType, inDataPtr) );
}


// ---------------------------------------------------------------------------
//	¥ SetData														  [public]
// ---------------------------------------------------------------------------
//	Set the Scrap contents to the data in a Handle
//
//	inClear specifies whether to clear the existing contents of the
//	Clipboard before storing the new data.

void
UScrap::SetData(
	ScrapFlavorType		inDataType,
	Handle				inData,
	bool				inClear)
{
	StHandleLocker	lock(inData);
	UScrap::SetData(inDataType, *inData, ::GetHandleSize(inData), inClear);
}


// ---------------------------------------------------------------------------
//	¥ ClearData														  [public]
// ---------------------------------------------------------------------------
//	Make the Scrap empty

void
UScrap::ClearData()
{
	ThrowIfOSErr_( (OSErr) ::ZeroScrap() );
}

#endif // PP_Target_Classic


// ===========================================================================
//	Carbon Implementation
// ===========================================================================

#if PP_Target_Carbon
#pragma mark -
#pragma mark === Carbon ===

// ---------------------------------------------------------------------------
//	¥ GetReference													  [public]
// ---------------------------------------------------------------------------
//	Return reference that uniquely identifies the scrap
//
//	Reference changes every time scrap is cleared

UScrap::RefType
UScrap::GetReference()
{
	RefType		ref = kScrapRefNone;
	
	::GetCurrentScrap(&ref);
	
	return ref;
}


// ---------------------------------------------------------------------------
//	¥ GetData														  [public]
// ---------------------------------------------------------------------------
//	Pass back the data in the Scrap of the specified type in a Handle
//	and return the size of the data
//
//	If ioData is nil, the data is not passed data, but its size is returned
//	Otherwise, ioData is resized if necessary to hold all the data
//
//	If ioData is nil, we do not throw an error when there is a problem getting
//	the data size. Function just returns 0.

SInt32
UScrap::GetData(
	ScrapFlavorType		inDataType,
	Handle				ioData)
{
	ScrapRef	scrap;
	ThrowIfOSStatus_( ::GetCurrentScrap(&scrap) );

	SInt32		byteCount = 0;
	OSStatus	status = ::GetScrapFlavorSize(scrap, inDataType, &byteCount);
	
		// Copy data from scrap if caller wants the data and data exists
	
	if ( (ioData != nil) &&
		 (status != noTypeErr) &&
		 (status != cantGetFlavorErr) ) {

		ThrowIfOSStatus_(status);		// Error getting size of data

		::SetHandleSize(ioData, byteCount);
		ThrowIfMemError_();

		StHandleLocker	lock(ioData);
		ThrowIfOSStatus_( ::GetScrapFlavorData(scrap, inDataType,
												&byteCount, *ioData) );
	}

	return byteCount;
}


// ---------------------------------------------------------------------------
//	¥ HasData                                                         [public]
// ---------------------------------------------------------------------------
//	Return whether the specified flavor is on the Scrap

bool
UScrap::HasData(
	ScrapFlavorType		inDataType)
{
	ScrapRef			scrap;
	ScrapFlavorFlags	flags;

	ThrowIfOSStatus_( ::GetCurrentScrap(&scrap) );

	return ( ::GetScrapFlavorFlags(scrap, inDataType, &flags) == noErr );
}


// ---------------------------------------------------------------------------
//	¥ SetData														  [public]
// ---------------------------------------------------------------------------
//	Set the Scrap contents to the data specified by a pointer and length
//
//	inClear specifies whether to clear the existing contents of the
//	Clipboard before storing the new data.

void
UScrap::SetData(
	ScrapFlavorType		inDataType,
	const void*			inDataPtr,
	SInt32				inDataLength,
	bool				inClear)
{
	if (inClear) {
		UScrap::ClearData();
	}

	ScrapRef	scrap;
	ThrowIfOSStatus_( ::GetCurrentScrap(&scrap) );

	ThrowIfOSStatus_( ::PutScrapFlavor( scrap, inDataType,
										kScrapFlavorMaskNone,
										inDataLength, inDataPtr ) );
}


// ---------------------------------------------------------------------------
//	¥ SetData														  [public]
// ---------------------------------------------------------------------------
//	Set the Scrap contents to the data in a Handle
//
//	inClear specifies whether to clear the existing contents of the
//	Clipboard before storing the new data.

void
UScrap::SetData(
	ScrapFlavorType		inDataType,
	Handle				inData,
	bool				inClear)
{
	StHandleLocker	lock(inData);
	UScrap::SetData(inDataType, *inData, ::GetHandleSize(inData), inClear);
}


// ---------------------------------------------------------------------------
//	¥ ClearData														  [public]
// ---------------------------------------------------------------------------
//	Make the Scrap empty

void
UScrap::ClearData()
{
	ThrowIfOSStatus_( ::ClearCurrentScrap() );
}

#endif // PP_Target_Carbon


PP_End_Namespace_PowerPlant
