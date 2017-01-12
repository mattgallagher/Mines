// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInternetAsyncSendQueue.h	PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	An ordered collection of items. Item positions in the list are
//	one-based--the first item is at index 1.
//

#ifndef _H_LInternetAsyncSendQueue
#define _H_LInternetAsyncSendQueue
#pragma once

#include <LArray.h>
#include <LDynamicBuffer.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LInternetAsyncSendQueue : public LArray {
public:
						LInternetAsyncSendQueue();
	virtual				~LInternetAsyncSendQueue();

	virtual void		Enqueue( const LDynamicBuffer** inItem );

	virtual Boolean		Dequeue( LDynamicBuffer** outItem );

	virtual void		ResetQueue();
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
