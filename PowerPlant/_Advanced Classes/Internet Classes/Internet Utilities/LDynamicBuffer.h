// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDynamicBuffer.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Simple class for dealing with a resizeable Buffer.

#ifndef _H_LDynamicBuffer
#define _H_LDynamicBuffer
#pragma once

#include <UMemoryMgr.h>
#include <cstring>
#include <TextUtils.h>

#include <LStream.h>
#include <LTemporaryFileStream.h>
#include <LHandleStream.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

#ifndef	kLDynBuf_MemoryThreshold
#define kLDynBuf_MemoryThreshold	32000	//~32k for max HandleStream size... if over this, or if
#endif										//memory is low, we go to LTemporaryFileStream

// ---------------------------------------------------------------------------

class	LDynamicBuffer {
public:

					LDynamicBuffer(SInt32 inExpectedSize = 0);
					LDynamicBuffer(const char * inBuffer);
					LDynamicBuffer(const char * inBuffer, SInt32 inByteCount);
					LDynamicBuffer(LStream * inStream, SInt32 inByteCount = -1);
					LDynamicBuffer(const LDynamicBuffer& copyBuffer);

	virtual			~LDynamicBuffer();

	virtual void	ResetBuffer();

	//Accessors
	virtual LStream*	GetBufferStream();

	virtual Boolean	GetBufferHandle(Handle& outHandle) const;
	virtual Boolean	GetBufferFile(FSSpec &outFileSpec);

	virtual Handle	GetBufferDataH(SInt32 inLength = 0);
	virtual Boolean	BufferToHandle(Handle outHandle, SInt32 inLength = 0, Boolean inTerminate = false);
	virtual	void	BufferToFile(LFile& outFile);

	inline UInt32	GetMemoryThreshold() {
						return mMemoryThreshold;
					}

	inline void		SetMemoryThreshold(UInt32 inSize = kLDynBuf_MemoryThreshold) {
						mMemoryThreshold = inSize;
					}

	inline SInt32	GetBufferLength() const {
						if (!mStream)
							return 0;
						return mStream->GetLength();
					}
	inline void		SetBufferLength(SInt32 NewSize) {
						CheckStreamSize(NewSize);
						mStream->SetLength(NewSize);
					}

	LStream *		DetachStream() {
						LStream * theStream = mStream;
						InitMembers();
						return theStream;
					}

	virtual void	SetStream(LStream * inStream, Boolean inOwnStream = true);

	//	SetBuffer
	inline void		SetBuffer(const char * inBuffer){
							SetBuffer(inBuffer, (SInt32) PP_CSTD::strlen(inBuffer));
						}
	virtual void	SetBuffer(const char * inBuffer, SInt32 inByteCount);
	virtual void	SetBuffer(LStream * inStream, SInt32 inByteCount = -1);
	virtual void	SetBuffer(LDynamicBuffer * inBuffer);

	//	ConcatenateBuffer
	inline void		ConcatenateBuffer(const char * inBuffer){
							if ((inBuffer) && (*inBuffer))
								ConcatenateBuffer(inBuffer, (SInt32) PP_CSTD::strlen(inBuffer));
						}
	virtual void	ConcatenateBuffer(const char * inBuffer, SInt32 inByteCount);
	virtual void	ConcatenateBuffer(LDynamicBuffer * inBuffer);
	virtual void	ConcatenateBuffer(LStream * inStream, SInt32 inByteCount = -1);

	//	PrependBuffer
	inline void		PrependBuffer(const char * inBuffer) {
							if ((inBuffer) && (*inBuffer))
								PrependBuffer(inBuffer, (SInt32) PP_CSTD::strlen(inBuffer));
						}
	virtual void	PrependBuffer(const char * inBuffer, SInt32 inByteCount);

	//	InsertBuffer
	inline void		InsertBuffer(const char * inBuffer, SInt32 position) {
							if ((inBuffer) && (*inBuffer))
								InsertBuffer(inBuffer, position, (SInt32) PP_CSTD::strlen(inBuffer));
						}
	virtual void	InsertBuffer(const char * inBuffer, SInt32 position, SInt32 inByteCount);
	virtual void	RemoveFromBuffer(SInt32 inPosition, SInt32 inByteCount);

	virtual void	LowerBuffer();
	virtual void	UpperBuffer();

	LDynamicBuffer&	operator=(
						const LDynamicBuffer& copyBuffer)
					{
						mMemoryThreshold = copyBuffer.mMemoryThreshold;
						ResetBuffer();
						if (copyBuffer.mStream)
							ConcatenateBuffer(copyBuffer.mStream);
						return *this;
					}

	LDynamicBuffer&	operator=(
						const char * copyString)
					{
						ResetBuffer();
						ConcatenateBuffer(copyString);
						return *this;
					}

	LDynamicBuffer&	operator+=(
						const char * copyString)
					{
						ConcatenateBuffer(copyString);
						return *this;
					}
	LDynamicBuffer&	operator+=(
						LDynamicBuffer * inBuffer)
					{
						ConcatenateBuffer(inBuffer);
						return *this;
					}
	LDynamicBuffer&	operator+=(
						LStream * inStream)
					{
						ConcatenateBuffer(inStream);
						return *this;
					}
protected:
	virtual void	CheckStreamSize(SInt32 inGrowBy);

	virtual void	InitMembers();

	UInt32			mMemoryThreshold;
	LStream *		mStream;
	LHandleStream *	mHandleStream;
	LTemporaryFileStream * mTempFileStream;

	Boolean			mOwnsStream;

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
