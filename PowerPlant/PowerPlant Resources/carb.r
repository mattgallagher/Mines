// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	carb.r			  			PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================
//
//	Mac OS X looks for a 'carb' resource with ID 0 to determine whether to
//	launch a program under Classic or Carbon
//
//	Add this file to Carbon targets, but NOT to Classic targets

type 'carb' {
	longint = 0;				// Four zero bytes
};


resource 'carb'(0) {
};
