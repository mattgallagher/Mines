// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDataStream.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A Stream whose bytes are in block of memory defined by a pointer to
//	the first byte and a byte count

#ifndef _H_LDataStream
#define _H_LDataStream
#pragma once

#include <LStream.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LDataStream : public LStream {
public:
						LDataStream();

						LDataStream( const LDataStream& inOriginal );

						LDataStream(
								void*				inBuffer,
								SInt32				inLength);

	LDataStream&		operator = ( const LDataStream& inOriginal );

	virtual				~LDataStream();

	virtual void		SetBuffer(
								void*				inBuffer,
								SInt32				inLength);

	virtual void*		GetBuffer()			{ return mBuffer; }

	virtual ExceptionCode	PutBytes(
								const void*			inBuffer,
								SInt32&				ioByteCount);

	virtual ExceptionCode	GetBytes(
								void*				outBuffer,
								SInt32&				ioByteCount);

protected:
	void*			mBuffer;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
