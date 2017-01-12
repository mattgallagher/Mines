// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInternetMapper.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	Encapsulates the idea of the domain name server for Internet hosts.
//	The mapper is the object that is used to find the address of another
//	computer on the network.
//
//	NOTE: For most connection-oriented applications, it is not necessary
//	to use the mapper interface. A name lookup will take place automatically
//	when necessary.
//
//	LInternetMapper is an abstract base class (thus its constructor
//	is declared protected). Use one of the predefined subclasses,
//	such as LMacTCPInetMapper or LOpenTptInetMapper, or use the
//	UNetworkFactory::CreateInternetMapper function to create the
//	appropriate mapper for the system software that's installed
//	on the user's machine.

#include <LInternetMapper.h>

PP_Begin_Namespace_PowerPlant

// ===========================================================================

#pragma mark === LInternetMapper ===

// ---------------------------------------------------------------------------
//		¥ LInternetMapper					[protected]
// ---------------------------------------------------------------------------
//	Constructor

LInternetMapper::LInternetMapper()
{
}


// ---------------------------------------------------------------------------
//		¥ ~LInternetMapper
// ---------------------------------------------------------------------------
//	Destructor

LInternetMapper::~LInternetMapper()
{
}

PP_End_Namespace_PowerPlant
