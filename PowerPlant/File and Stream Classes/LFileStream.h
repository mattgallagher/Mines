// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFileStream.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A File which uses a Stream to access its data fork

#ifndef _H_LFileStream
#define _H_LFileStream
#pragma once

#include <LFile.h>
#include <LStream.h>

#include <Script.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LFileStream : public LFile,
					  public LStream {
public:
							LFileStream();

							LFileStream( const FSSpec& inFileSpec );

							LFileStream(
									AliasHandle		inAlias,
									Boolean&		outWasChanged,
									FSSpec*			inFromFile = nil);

	virtual					~LFileStream();

	virtual void			SetMarker(
									SInt32			inOffset,
									EStreamFrom		inFromWhere);

	virtual SInt32			GetMarker() const;

	virtual void			SetLength( SInt32 inLength );

	virtual SInt32			GetLength() const;

	virtual ExceptionCode	PutBytes(
									const void*		inBuffer,
									SInt32&			ioByteCount);

	virtual ExceptionCode	GetBytes(
									void*			outBuffer,
									SInt32&			ioByteCount);


private:								// Copy and Assignment not allowed
							LFileStream( const LFileStream& );

	LFileStream&			operator = ( const LFileStream& );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
