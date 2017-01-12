// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LEventSemaphore.h			PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde

#ifndef _H_LEventSemaphore
#define _H_LEventSemaphore
#pragma once

#include <LSemaphore.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------

class LEventSemaphore : public LSemaphore {
public:

	// constructors / destructors
					LEventSemaphore();
					LEventSemaphore(Boolean posted);
	virtual			~LEventSemaphore();

	// blocking
	virtual void	Signal();
	virtual UInt32	Reset();

protected:

	// member variables
	UInt32			mPostCount;		// # of times Signal() was called
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_LEventSemaphore

