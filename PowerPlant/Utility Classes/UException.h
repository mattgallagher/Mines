// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UException.h				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	Useful macros for throwing exceptions. The macros throw and exception
//	based on some test condition. The macros use the Throw_() macro,
//	which supports debugging options. See UDebugging.h for details.

#ifndef _H_UException
#define _H_UException
#pragma once

#include <UDebugging.h>

#include <MacErrors.h>
#include <MacMemory.h>
#include <Resources.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Exception codes

const ExceptionCode	err_NilPointer		= FOUR_CHAR_CODE('nilP');
const ExceptionCode	err_AssertFailed	= FOUR_CHAR_CODE('asrt');


// ---------------------------------------------------------------------------
//	Exception Handling Macros
//
//	These are obsolete. Use the C++ keywords try and catch instead.

#if PP_Obsolete_ThrowExceptionCode
	#define Try_			try
	#define Catch_(err)		catch(PP_PowerPlant::ExceptionCode err)
	#define EndCatch_
#endif


// ---------------------------------------------------------------------------
//	Useful macros for signaling common failures

#define ThrowIfOSStatus_(err)										\
	do {															\
		OSStatus	__theErr = err;									\
		if (__theErr != noErr) {									\
			Throw_(__theErr);										\
		}															\
	} while (false)

#define ThrowIfOSErr_(err)											\
	do {															\
		OSErr	__theErr = err;										\
		if (__theErr != noErr) {									\
			Throw_(__theErr);										\
		}															\
	} while (false)

#define ThrowIfError_(err)											\
	do {															\
		PP_PowerPlant::ExceptionCode	__theErr = err;				\
		if (__theErr != 0) {										\
			Throw_(__theErr);										\
		}															\
	} while (false)

#define ThrowOSErr_(err)	Throw_(err)

#define ThrowOSStatus_(err)	Throw_(err)

#define	ThrowIfNil_(ptr)											\
	do {															\
		if ((ptr) == nil) Throw_(PP_PowerPlant::err_NilPointer);	\
	} while (false)

#define	ThrowIfNULL_(ptr)	ThrowIfNil_(ptr)

#define	ThrowIfResError_()	ThrowIfOSErr_(::ResError())
#define	ThrowIfMemError_()	ThrowIfOSErr_(::MemError())
#define	ThrowIfQDError_()	ThrowIfOSErr_(::QDError())
#define ThrowIfPrError_()	ThrowIfOSErr_(::PrError())

#define	ThrowIfResFail_(h)											\
	do {															\
		if ((h) == nil) {											\
			OSErr	__theErr = ::ResError();						\
			if (__theErr == noErr) {								\
				__theErr = resNotFound;								\
			}														\
			Throw_(__theErr);										\
		}															\
	} while (false)

#define	ThrowIfMemFail_(p)											\
	do {															\
		if ((p) == nil) {											\
			OSErr	__theErr = ::MemError();						\
			if (__theErr == noErr) __theErr = memFullErr;			\
			Throw_(__theErr);										\
		}															\
	} while (false)

#define	ThrowIf_(test)												\
	do {															\
		if (test) Throw_(PP_PowerPlant::err_AssertFailed);			\
	} while (false)

#define	ThrowIfNot_(test)											\
	do {															\
		if (!(test)) Throw_(PP_PowerPlant::err_AssertFailed);		\
	} while (false)

#define	FailOSErr_(err)		ThrowIfOSErr_(err)
#define FailNIL_(ptr)		ThrowIfNil_(ptr)

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
