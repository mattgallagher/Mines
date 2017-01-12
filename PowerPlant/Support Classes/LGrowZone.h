// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGrowZone.h					PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LGrowZone
#define _H_LGrowZone
#pragma once

#include <LBroadcaster.h>
#include <LPeriodical.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LGrowZone : public LBroadcaster,
					public LPeriodical {
public:
						LGrowZone( Size inReserveSize );

	virtual				~LGrowZone();

	static LGrowZone*	GetGrowZone()		{ return sGrowZone; }

	void				GiveWarning()		{ mGiveWarning = true; }

	virtual	void		SpendTime( const EventRecord& inMacEvent );

	virtual Boolean		MemoryIsLow();

	virtual SInt32		AskListenersToFree( Size inBytesNeeded );

	virtual SInt32		DoGrowZone( Size inBytesNeeded );

	virtual SInt32		UseLocalReserve( Size inBytesNeeded );

protected:
	static LGrowZone*	sGrowZone;
	static GrowZoneUPP	sGrowZoneUPP;

	Handle				mLocalReserve;
	Size				mReserveSize;
	Boolean				mGiveWarning;

	static pascal SInt32 GrowZoneCallBack( Size inBytesNeeded );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
