// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMImageWellImp.cp			PowerPlant 2.2.2	©1999-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LAMImageWellImp.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LAMImageWellImp					Stream/Default Constructor	  [public]
// ---------------------------------------------------------------------------
//	Do not access inStream. It may be nil. Read Stream data in Init()

LAMImageWellImp::LAMImageWellImp(
	LStream*	inStream)

	: LAMControlImp(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LAMImageWellImp						Destructor				  [public]
// ---------------------------------------------------------------------------

LAMImageWellImp::~LAMImageWellImp()
{
}


// ---------------------------------------------------------------------------
//	¥ GetStructureOffsets											  [public]
// ---------------------------------------------------------------------------

OSStatus
LAMImageWellImp::GetStructureOffsets(
	Rect&	outOffsets) const
{
	if (LAMControlImp::GetStructureOffsets(outOffsets) != noErr) {
	
		// Control Manager was unable to calculate the structure
		// region, so we assume Platinum Theme, where the shadow
		// border is 2 pixels thick.
		
		::MacSetRect(&outOffsets, 2, 2, 2, 2);
	}
	
	return noErr;
}


PP_End_Namespace_PowerPlant
