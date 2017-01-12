// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UPrinting.cp				PowerPlant 2.2.2	©1999-2005 Metrowerks Inc.
// ===========================================================================
//
//	Wrapper file for UPrinting implementation
//
//	There are 3 implementations:
//
//		USessionPrinting - Requires Carbon 1.1 or later. Supports multiple
//			print sessions.
//
//		UCarbonPrinting - Requires Carbon 1.0 or later.
//
//		UClassicPrinting - Classic 68K and PPC printing.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
	#undef PowerPlant_PCH			// So imp .cp files don't double include
#endif

#include <UPrinting.h>

// ---------------------------------------------------------------------------
// Include implementation file based on compiler flags

#if TARGET_API_MAC_CARBON

	#if PM_USE_SESSION_APIS
	
		#include "USessionPrinting.cp"
		
	#else

		#include "UCarbonPrinting.cp"
		
	#endif

#else

	#include <UClassicPrinting.cp>

#endif


// ---------------------------------------------------------------------------
//	Common implementation

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ PMRectToQDRect										 [static] [public]
// ---------------------------------------------------------------------------
//	Set values in a QD Rect (16-bit integer values) from a PMRect (double
//	floating point values)

void
UPrinting::PMRectToQDRect(
	const PMRect&	inPMRect,
	Rect&			outQDRect)
{
	outQDRect.top	 = (SInt16) inPMRect.top;
	outQDRect.left	 = (SInt16) inPMRect.left;
	outQDRect.bottom = (SInt16) inPMRect.bottom;
	outQDRect.right  = (SInt16) inPMRect.right;
}


PP_End_Namespace_PowerPlant
