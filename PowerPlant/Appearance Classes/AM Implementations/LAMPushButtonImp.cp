// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMPushButtonImp.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LAMPushButtonImp.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LAMPushButtonImp					Stream/Default Constructor	  [public]
// ---------------------------------------------------------------------------
//	Do not access inStream. It may be nil. Read Stream data in Init()

LAMPushButtonImp::LAMPushButtonImp(
	LStream*	inStream)

	: LAMControlImp(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LAMPushButtonImp						Destructor				  [public]
// ---------------------------------------------------------------------------

LAMPushButtonImp::~LAMPushButtonImp()
{
}


// ---------------------------------------------------------------------------
//	¥ GetStructureOffsets											  [public]
// ---------------------------------------------------------------------------

OSStatus
LAMPushButtonImp::GetStructureOffsets(
	Rect&	outOffsets) const
{
	if (LAMControlImp::GetStructureOffsets(outOffsets) != noErr) {
	
		// Control Manager was unable to calculate the structure
		// region, so we assume Platinum Theme, where the default
		// ring is 3 pixels thick.
		
		Boolean		isDefault;
		GetDataTag(0, kControlPushButtonDefaultTag, sizeof(Boolean), &isDefault);
		
		if (isDefault) {
			::MacSetRect(&outOffsets, 3, 3, 3, 3);
		}
	}
	
	return noErr;
}


PP_End_Namespace_PowerPlant
