// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	InterruptDisableLib.h		PowerPlant 2.2.2	©1999-2005 Metrowerks Inc.
// ===========================================================================
//
//	Based on Apple Sample Code written by Quinn "The Eskimo!"
//	Portions ©1998 by Apple Computer, Inc.

#include <MacTypes.h>

extern "C" {

extern pascal UInt16 GetInterruptMask(void);
	// Returns the current interrupt mask, as a value
	// from 0 to 7.

extern pascal UInt16 SetInterruptMask(UInt16 newMask);
	// Sets the current interrupt mask, as a value
	// from 0 to 7.

}
