// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LNetMessageQueue.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LNetMessageQueue
#define _H_LNetMessageQueue
#pragma once

#include <LInterruptSafeList.h>
#include <LPeriodical.h>
#include <LBroadcaster.h>
#include <UNetworkMessages.h>

#include <Processes.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LNetMessageQueue :	public LInterruptSafeList,
							public LPeriodical {
public:
						LNetMessageQueue( LBroadcaster &inBroadcaster );

	virtual				~LNetMessageQueue();

	virtual	void		SpendTime( const EventRecord & inMacEvent );

	virtual void		Append( LInterruptSafeListMember* inItem );

protected:
	LBroadcaster *			mBroadcaster;
	ProcessSerialNumber		mPSN;
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
