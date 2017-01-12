// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMClockImp.cp				PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//	Implementation for an AM Clock control

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LAMClockImp.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LAMClockImp						Stream/Default Constructor	  [public]
// ---------------------------------------------------------------------------
//	Do not access inStream. It may be nil. Read Stream data in Init()

LAMClockImp::LAMClockImp(
	LStream*	inStream)

	: LAMControlImp(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LAMClockImp							Destructor				  [public]
// ---------------------------------------------------------------------------

LAMClockImp::~LAMClockImp()
{
}


// ---------------------------------------------------------------------------
//	¥ GetStructureOffsets											  [public]
// ---------------------------------------------------------------------------

OSStatus
LAMClockImp::GetStructureOffsets(
	Rect&	outOffsets) const
{
	if (LAMControlImp::GetStructureOffsets(outOffsets) != noErr) {
	
		// Control Manager was unable to calculate the structure
		// region, so we assume Platinum Theme, where the focus
		// ring is 3 pixels thick.
		
		::MacSetRect(&outOffsets, 3, 3, 3, 3);
	}
	
	return noErr;
}


PP_End_Namespace_PowerPlant
