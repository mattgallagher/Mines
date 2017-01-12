// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UExtractFromAEDesc.cp	  	PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Functions for extracting data from AppleEvent Descriptor (AEDesc) records

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UExtractFromAEDesc.h>
#include <UAppleEventsMgr.h>
#include <UMemoryMgr.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ ThePString
// ---------------------------------------------------------------------------
//	Extracts a Pascal String from an AppleEvent descriptor

void
UExtractFromAEDesc::ThePString(
	const AEDesc&	inDesc,
	StringPtr		outStringPtr,
	SInt32			inMaxSize)
{
	const AEDesc*	descPtr = &inDesc;

	StAEDescriptor	coerceDesc;
	
	if (inDesc.descriptorType != typeChar) {
	
			// Try to coerce descriptor into a string
	
		if (::AECoerceDesc(&inDesc, typeChar, coerceDesc) != noErr) {
			ThrowOSErr_(errAETypeError);	// Coercion failed
		}
		
		descPtr = coerceDesc;				// Use coerced descriptor
	}
	
		// Descriptor contains characters. For a Pascal string, we
		// need to get the length (enforcing the max size) and
		// store it as the first byte.
	
	SInt32	numChars = UExtractFromAEDesc::AEGetDescDataSize(*descPtr);
	
	if (numChars > (inMaxSize - 1)) {
		numChars = inMaxSize - 1;
	}
	outStringPtr[0] = (UInt8) numChars;
	
											// Copy characters into string
	OSErr err = UExtractFromAEDesc::AEGetDescData(
									*descPtr, outStringPtr + 1, numChars);
			
	ThrowIfOSErr_(err);
}

	
// ---------------------------------------------------------------------------
//	¥ TheAlias
// ---------------------------------------------------------------------------
//	Extracts an AliasHandle from an AppleEvent descriptor
//
//	Pass in an unallocated AliasHandle as a parameter. Routine passes back
//	a newly created AliasHandle which the caller is responsible for deleting.

void
UExtractFromAEDesc::TheAlias(
	const AEDesc&	inDesc,
	AliasHandle&	outAliasHandle)
{
	const AEDesc*	descPtr = &inDesc;

	StAEDescriptor	coerceDesc;
	
	if (inDesc.descriptorType != typeAlias) {
	
			// Try to coerce descriptor into an Alias
	
		if (::AECoerceDesc(&inDesc, typeAlias, coerceDesc) != noErr) {
			ThrowOSErr_(errAETypeError);	// Coercion failed
		}
		
		descPtr = coerceDesc;				// Use coerced descriptor
	}
	
			// Allocate an AliasHandle to contain all the descriptor data
	
	Size	dataSize = UExtractFromAEDesc::AEGetDescDataSize(*descPtr);
	
	outAliasHandle = (AliasHandle) ::NewHandle(dataSize);
	
	ThrowIfMemFail_(outAliasHandle);
	
											// Copy alias data
	StHandleLocker	lock((Handle) outAliasHandle);
	
	OSErr	err = UExtractFromAEDesc::AEGetDescData(
						*descPtr, *outAliasHandle, dataSize);

	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ GetDescData
// ---------------------------------------------------------------------------
//	Extracts data of a specfied AE data type from an AppleEvent descriptor

void
UExtractFromAEDesc::TheData(
	const AEDesc&	inDesc,
	DescType		inDescType,
	void*			inDataPtr,
	Size			inMaxSize)
{
	const AEDesc*	descPtr = &inDesc;

	StAEDescriptor	coerceDesc;
	
	if (inDesc.descriptorType != inDescType) {
	
			// Input descriptor has a type that is different from
			// the desired data type. Try to coerce it.
	
		if (::AECoerceDesc(&inDesc, inDescType, coerceDesc) != noErr) {
			ThrowOSErr_(errAETypeError);	// Coercion failed
		}
		
		descPtr = coerceDesc;				// Use coerced descriptor
	}
	
	OSErr err = UExtractFromAEDesc::AEGetDescData(
										*descPtr, inDataPtr, inMaxSize);
			
	ThrowIfOSErr_(err);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AEGetDescData
// ---------------------------------------------------------------------------
//	Copy the data from an AppleEvent descriptor into a buffer
//
//	This is a wrapper for the Toolbox routine AEGetDescData, which exists
//	on Carbon but not on Classic

OSErr
UExtractFromAEDesc::AEGetDescData(
	const AEDesc&	inDesc,
	void*			inDataPtr,
	Size			inMaxSize)
{
#if ACCESSOR_CALLS_ARE_FUNCTIONS

	return ::AEGetDescData(&inDesc, inDataPtr, inMaxSize);
	
#else

		// We need to extract the data ourselves by directly accessing
		// the Handle within the AE descriptor

	OSErr	err = errAEDescIsNull;
	
	if (inDesc.dataHandle != nil) {
		Size	bytesToCopy	= inMaxSize;
		Size	dataSize	= ::GetHandleSize(inDesc.dataHandle);
		
		if (dataSize < bytesToCopy) {
			bytesToCopy = dataSize;
		}
		
		::BlockMoveData(*(inDesc.dataHandle), inDataPtr, bytesToCopy);
		err = noErr;
	}
	
	return err;

#endif
}
	

// ---------------------------------------------------------------------------
//	¥ AEGetDescDataSize
// ---------------------------------------------------------------------------
//	Returns the size in bytes of the data within an AppleEvent descriptor
//
//	This is a wrapper for the Toolbox routine AEGetDescDataSize, which exists
//	on Carbon but not on Classic

Size
UExtractFromAEDesc::AEGetDescDataSize(
	const AEDesc&	inDesc)
{
#if ACCESSOR_CALLS_ARE_FUNCTIONS

	return ::AEGetDescDataSize(&inDesc);
	
#else

	Size	dataSize = 0;
	
	if (inDesc.dataHandle != nil) {
		dataSize = ::GetHandleSize(inDesc.dataHandle);
	}
	
	return dataSize;

#endif
}


PP_End_Namespace_PowerPlant
