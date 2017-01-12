// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	PP_Constants.cp				PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <PP_Prefix.h>

PP_Begin_Namespace_PowerPlant


const unsigned char	Str_Empty[]	= { 0 };

const unsigned char	Str_Dummy[]	= { 0x01, 0x58 }; // One character string "X"

const Point		Point_00		= { 0, 0 };
const Rect		Rect_0000		= { 0, 0, 0, 0 };


PP_End_Namespace_PowerPlant
