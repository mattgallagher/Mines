// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UHeapUtils.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub

#ifndef _H_UHeapUtils
#define _H_UHeapUtils
#pragma once

#include <PP_Debug.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

namespace UHeapUtils {

				// Compact and purge

	Size			CompactHeap(
							const THz	inHeapZone = nil);
	void			PurgeHeap(
							const THz	inHeapZone = nil);
	Size			CompactAndPurgeHeap(
							const THz	inHeapZone = nil);

				// Scrambling

	void			ScrambleHeap();

}



// ---------------------------------------------------------------------------

#if PP_Target_Classic		// No Zones in Carbon

class StZone {
public:
	StZone( const THz inNewZone )
		{
			mSaveZone = ::GetZone();
			::SetZone(inNewZone);
		}

	~StZone()
		{
			::SetZone(mSaveZone);
		}

private:
	THz	mSaveZone;
};

#endif

	// Use macro to declare a StZone object so that code works
	// under both Classic and Carbon

#if TARGET_API_MAC_CARBON
	#define	StZone_(inZone)
#else
	#define	StZone_(inZone)			PP_PowerPlant::StZone __theZone(inZone)
#endif


// ---------------------------------------------------------------------------

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_UHeapUtils
