// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UDebugNew.h					PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub

#ifndef _H_UDebugNew
#define _H_UDebugNew
#pragma once

#include <PP_Debug.h>

#if PP_DebugNew_Support
	#include "DebugNew.h"
#endif

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	UDebugNew
// ---------------------------------------------------------------------------
//	A wrapper for DebugNew that extends its functionality. Only available
//	if PP_DebugNew_Support is on.

#if PP_DebugNew_Support

namespace UDebugNew {

	void						ValidateAll();
	void						ValidatePtr(
										const void*				inPtr);
	SInt32						GetPtrSize(
										const void*				inPtr);
	SInt32						Report();
	void						Forget();

#if DEBUG_NEW >= DEBUG_NEW_BASIC
		// DebugNew does not allow an error handler if DEBUG_NEW
		// is DEBUG_NEW_OFF
	void						ErrorHandler(
										SInt16					inErr);
	DebugNewErrorHandler_t		SetErrorHandler(
										DebugNewErrorHandler_t	inHandler);
	DebugNewErrorHandler_t		InstallDefaultErrorHandler();
#endif

	inline	UInt32				GetAllocCount()
									{
									#if DEBUG_NEW >= DEBUG_NEW_BASIC
										return gDebugNewAllocCount;
									#else
										return 0;
									#endif
									}
	inline	UInt32				GetAllocCurr()
									{
									#if DEBUG_NEW >= DEBUG_NEW_BASIC
										return gDebugNewAllocCurr;
									#else
										return 0;
									#endif
									}
	inline	UInt32				GetAllocMax()
									{
									#if DEBUG_NEW >= DEBUG_NEW_BASIC
										return gDebugNewAllocMax;
									#else
										return 0;
									#endif
									}

										// Flags are defined in DebugNew.h
	inline	void				SetFlags( UInt32				inFlags)
									{
									#if DEBUG_NEW >= DEBUG_NEW_BASIC
										gDebugNewFlags = inFlags;
									#else
										inFlags;
									#endif
									}
	inline	UInt32				GetFlags()
									{
									#if DEBUG_NEW >= DEBUG_NEW_BASIC
										return gDebugNewFlags;
									#else
										return 0;
									#endif
									}
}

#endif // PP_DebugNew_Support

// ---------------------------------------------------------------------------
//	API Macros
// ---------------------------------------------------------------------------
//	These macros allow you to access the DebugNew API. These macros map tp
//	UDebugNew and not DebugNew to gain the benefits of UDebugNew. When
//	PP_DebugNew_Support is disabled, the calls to UDebugNew will be
//	preprocessed out; this provides you with an easy way to remove the
//	supports when you don't want them (e.g. release builds).

#if PP_DebugNew_Support
	#define DebugNewValidateAllBlocks_()	PP_PowerPlant::UDebugNew::ValidateAll()
	#define DebugNewValidatePtr_(p)			PP_PowerPlant::UDebugNew::ValidatePtr(p)
	#define DebugNewGetPtrSize_(p)			PP_PowerPlant::UDebugNew::GetPtrSize(p)
	#define DebugNewReportLeaks_()			PP_PowerPlant::UDebugNew::Report()
	#define DebugNewForgetLeaks_()			PP_PowerPlant::UDebugNew::Forget()
#else
	#define DebugNewValidateAllBlocks_()	(void)0
	#define DebugNewValidatePtr_(p)			(void)0
	#define DebugNewGetPtrSize_(p)			(SInt32)0
	#define DebugNewReportLeaks_()			(SInt32)0
	#define DebugNewForgetLeaks_()			(void)0
#endif

#if PP_DebugNew_Support && (DEBUG_NEW >= DEBUG_NEW_BASIC)

	#define DebugNewSetErrorHandler_(handler)		PP_PowerPlant::UDebugNew::SetErrorHandler(handler)
	#define PP_DebugNewInstallPPErrorHandler_()		PP_PowerPlant::UDebugNew::InstallDefaultErrorHandler()

#else

	#define DebugNewSetErrorHandler_(handler)		(void)0
	#define PP_DebugNewInstallPPErrorHandler_()		(void)0

#endif

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_UDebugNew
