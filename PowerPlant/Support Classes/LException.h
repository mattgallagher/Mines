// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LException.h				PowerPlant 2.2.2	©1999-2005 Metrowerks Inc.
// ===========================================================================
//
//	PowerPlant Exception class

#ifndef _H_LException
#define _H_LException
#pragma once

#include <PP_Types.h>
#include <exception>

#ifndef _MSL_NO_THROW
	// If not using MSL, use an empty no throw specification
	#define _MSL_NO_THROW
#endif

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------

class LException : public PP_STD::exception {
public:
						LException(
								SInt32			inErrorCode,
								ConstStringPtr	inErrorString = nil) _MSL_NO_THROW;

						LException( const LException& inException ) _MSL_NO_THROW;

	LException&			operator = ( const LException& inException ) _MSL_NO_THROW;

#ifdef __GNUC__
	virtual				~LException() throw();
#else
	virtual				~LException() _MSL_NO_THROW;
#endif

#ifdef __GNUC__
	virtual const char*	what() const throw();
#else
	virtual const char*	what() const _MSL_NO_THROW;
#endif

	SInt32				GetErrorCode() const;

	void				SetErrorCode( SInt32 inErrorCode );

	ConstStringPtr		GetErrorString() const;

	void				SetErrorString( ConstStringPtr inErrorString );

protected:
	SInt32			mErrorCode;
	Str255			mErrorString;

	// -----------------------------------------------------------------------
	//	Class Functions

public:
	static void			Throw(	SInt32			inErr);

	static void			Throw(	SInt32			inErr,
								ConstStringPtr	inErrStr);
};


// ===========================================================================
//	Inline function definitions

// ---------------------------------------------------------------------------
//	¥ GetErrorCode											 [inline] [public]
// ---------------------------------------------------------------------------

inline SInt32
LException::GetErrorCode() const
{
	return mErrorCode;
}


// ---------------------------------------------------------------------------
//	¥ GetErrorString										 [inline] [public]
// ---------------------------------------------------------------------------

inline ConstStringPtr
LException::GetErrorString() const
{
	return mErrorString;
}


// ---------------------------------------------------------------------------

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
