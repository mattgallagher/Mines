// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UThread.h					PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde

#ifndef _H_UThread
#define _H_UThread
#pragma once

#include <LAttachment.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// forward class declarations
class	LMutexSemaphore;


// ===========================================================================
// ¥ StCritical													  StCritical ¥
// ===========================================================================

class StCritical
{
public:
	StCritical();
	~StCritical();
};


// ===========================================================================
// ¥ StMutex														 StMutex ¥
// ===========================================================================

class StMutex
{
public:
	StMutex(LMutexSemaphore& inMutex);
	~StMutex();
private:
	LMutexSemaphore&	mMutex;
};


// ===========================================================================
// ¥ LYieldAttachment										LYieldAttachment ¥
// ===========================================================================

class LYieldAttachment : public LAttachment
{
public:
					LYieldAttachment(SInt32 inQuantum = -1);
protected:
	virtual	void	ExecuteSelf(MessageT inMessage, void* ioParam);

	SInt32			mQuantum;
	UInt32			mNextTicks;
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif	// _H_UThread
