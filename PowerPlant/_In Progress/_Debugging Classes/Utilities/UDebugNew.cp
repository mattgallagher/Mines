// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UDebugNew.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	A collection of mini-utilities to work with DebugNew. The main purpose
//	is to allow DebugNew and Spotlight to work nicely together.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UDebugNew.h>
#include <UOnyx.h>

#if PP_DebugNew_Support

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ ValidateAll
// ---------------------------------------------------------------------------
//	Performs a validation of all allocated blocks.

void
UDebugNew::ValidateAll()
{
#if DEBUG_NEW == DEBUG_NEW_LEAKS	// Only available when leak checking is enabled

		// Disable Spotlight as it doesn't like the funky
		// things DebugNew does.
	StSpotlightDisable_();

	DebugNewValidateAllBlocks();
#endif
}


// ---------------------------------------------------------------------------
//	¥ ValidatePtr
// ---------------------------------------------------------------------------
//	Try to validate that a pointer points to a valid, uncorrupted block.
//	Invokes DebugNew's error handler if validation fails. Does nothing when
//	DebugNew is disabled.

void
UDebugNew::ValidatePtr(
	const void*	inPtr)
{
		// Disable Spotlight as it doesn't like the funky
		// things DebugNew does.
	StSpotlightDisable_();

	DebugNewValidatePtr(const_cast<void*>(inPtr));
}


// ---------------------------------------------------------------------------
//	¥ GetPtrSize
// ---------------------------------------------------------------------------
//	Returns the size of the pointer.

SInt32
UDebugNew::GetPtrSize(
	const void*	inPtr)
{
		// Disable Spotlight as it doesn't like the funky
		// things DebugNew does.
	StSpotlightDisable_();

	SInt32 size = DebugNewGetPtrSize(const_cast<void*>(inPtr));

	return size;
}


// ---------------------------------------------------------------------------
//	¥ Report
// ---------------------------------------------------------------------------
// Call to write memory leak tracking status to a file called
// "leaks.log" in the application directory. Returns the
// number of leaks.

SInt32
UDebugNew::Report()
{
#if DEBUG_NEW >= DEBUG_NEW_BASIC

		// Disable Spotlight as it doesn't like the funky
		// things DebugNew does.
	StSpotlightDisable_();

	SInt32 leaks = DebugNewReportLeaks();

	return leaks;
#else

	return 0;

#endif
}


// ---------------------------------------------------------------------------
//	¥ Forget
// ---------------------------------------------------------------------------
// Call to tell DebugNew to ignore any currently allocated
// blocks in the leak report. Useful at app startup to ignore
// memory allocated at startup that doesn't get explicitly
// freed.

void
UDebugNew::Forget()
{
#if DEBUG_NEW >= DEBUG_NEW_BASIC

		// Disable Spotlight as it doesn't like the funky
		// things DebugNew does.
	StSpotlightDisable_();

	DebugNewForgetLeaks();
#endif
}


#if DEBUG_NEW >= DEBUG_NEW_BASIC
	// DebugNew does not allow an error handler if DEBUG_NEW
	// is off; therefore we cannot either.

// ---------------------------------------------------------------------------
//	¥ ErrorHandler
// ---------------------------------------------------------------------------
//	A replacement for DebugNew's error handler. Provides more integration
//	with PowerPlant's error reporting mechanisms.
//
//	This is not automatically installed, only provided as an alternative
//	mechanism. You can use UDebugNew::InstallDefaultErrorHandler() to
//	install it.

void
UDebugNew::ErrorHandler(
	SInt16	inErr)
{
	switch (inErr) {
		case dbgnewNullPtr:
			SignalStringLiteral_("UDebugNew: null pointer");
			break;

		case dbgnewTooManyFrees:
			SignalStringLiteral_("UDebugNew: more deletes than news");
			break;

		case dbgnewPointerOutsideHeap:
			SignalStringLiteral_("UDebugNew: delete or validate called for pointer outside application heap");
			break;

		case dbgnewFreeBlock:
			SignalStringLiteral_("UDebugNew: delete or validate called for free block");
			break;

		case dbgnewBadHeader:
			SignalStringLiteral_("UDebugNew: unknown block, block header was overwritten, or mismatched new and delete[]");
			break;

		case dbgnewBadTrailer:
			SignalStringLiteral_("UDebugNew: block trailer was overwritten");
			break;

		case dbgnewBlockNotInList:
			SignalStringLiteral_("UDebugNew: block valid but not in block list (internal error)");
			break;

		case dbgnewFreeBlockOverwritten:
			SignalStringLiteral_("UDebugNew: free block overwritten, could be dangling pointer");
			break;

		case dbgnewMismatchedNewDelete:
			SignalStringLiteral_("UDebugNew: mismatched regular/array new and delete (probably delete on new[] block)");
			break;

		default:
			SignalStringLiteral_("UDebugNew: undefined error");
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ SetErrorHandler
// ---------------------------------------------------------------------------
//	Set the DebugNew error handler to the given procedure. Returns a pointer
//	to the old procedure.

DebugNewErrorHandler_t
UDebugNew::SetErrorHandler(
	DebugNewErrorHandler_t	inHandler)
{
	return DebugNewSetErrorHandler(inHandler);
}


// ---------------------------------------------------------------------------
//	¥ InstallDefaultErrorHandler
// ---------------------------------------------------------------------------
//	Installs UDebugNew::ErrorHandler as the default error handler. Provided
//	for convenience.

DebugNewErrorHandler_t
UDebugNew::InstallDefaultErrorHandler()
{
	return DebugNewSetErrorHandler(UDebugNew::ErrorHandler);
}

#endif // DEBUG_NEW >= DEBUG_NEW_BASIC

PP_End_Namespace_PowerPlant

#endif // PP_DebugNew_Support
