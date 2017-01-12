// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LHandleStream.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A Stream whose bytes are in a Handle block in memory

#ifndef _H_LHandleStream
#define _H_LHandleStream
#pragma once

#include <LStream.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LHandleStream : public LStream {
public:
							LHandleStream();

							LHandleStream( const LHandleStream& inOriginal );

							LHandleStream( Handle inHandle );

	LHandleStream&			operator = ( const LHandleStream& inOriginal );

	virtual					~LHandleStream();

	virtual void			SetLength( SInt32 inLength );

	virtual ExceptionCode	PutBytes(
									const void*		inBuffer,
									SInt32&			ioByteCount);

	virtual ExceptionCode	GetBytes(
									void*			outBuffer,
									SInt32&			ioByteCount);

	void					SetDataHandle( Handle inHandle );

	Handle					GetDataHandle()		{ return mDataH; }

	Handle					DetachDataHandle();

protected:
	Handle			mDataH;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
