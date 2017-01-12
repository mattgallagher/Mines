// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UControlMgr.cp				PowerPlant 2.2.2	©1999-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UControlMgr.h>
#include <UEnvironment.h>

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//	StControlActionUPP
// ===========================================================================
//	Constructor allocates a ControlActionUPP
//	Destructor disposes of it

StControlActionUPP::StControlActionUPP(
	ControlActionProcPtr	inProcPtr)
{
	mControlActionUPP = nil;

	if (inProcPtr != nil) {
		mControlActionUPP = NewControlActionUPP(inProcPtr);
	}
}


StControlActionUPP::~StControlActionUPP()
{
	if (mControlActionUPP != nil) {
		DisposeControlActionUPP(mControlActionUPP);
	}
}

#pragma mark -

// ===========================================================================
//	StControlDefUPP
// ===========================================================================
//	Constructor allocates a ControlDefUPP
//	Destructor disposes of it

StControlDefUPP::StControlDefUPP(
	ControlDefProcPtr	inProcPtr)
{
	mControlDefUPP = nil;

	if (inProcPtr != nil) {
		mControlDefUPP = NewControlDefUPP(inProcPtr);
	}
}


StControlDefUPP::~StControlDefUPP()
{
	if (mControlDefUPP != nil) {
		DisposeControlDefUPP(mControlDefUPP);
	}
}

#pragma mark -

// ===========================================================================
//	StControlColorUPP
// ===========================================================================
//	Constructor allocates a ControlColorUPP
//	Destructor disposes of it
//
//	ControlColorUPPs are available in ControlsLib 8.5 and CarbonLib 1.1.
//
//	To support CarbonLib 1.0.x on Mac OS 8/9, we need to do some extra work
//	by extracting the underlying functions from InterfaceLib/ControlsLib
//	at runtime.

typedef	UniversalProcPtr	(*NewRoutineDescriptorFuncPtr)(ProcPtr, ProcInfoType, ISAType);
typedef void				(*DisposeRoutineDescriptorFuncPtr)(UniversalProcPtr);
typedef OSStatus			(*SetControlColorProcFuncPtr)(ControlRef, ControlColorUPP);

#if OPAQUE_UPP_TYPES
	enum { uppControlColorProcInfo = 0x00001AF0 };
#endif

const unsigned char Str_InterfaceLib[]	= "\pInterfaceLib";
const unsigned char Str_ControlsLib[]	= "\pControlsLib";


StControlColorUPP::StControlColorUPP(
	ControlColorProcPtr	inProcPtr)
{
	mControlColorUPP = nil;
	
	if (inProcPtr != nil) {
	
		#if TARGET_API_MAC_CARBON
		
			if (CFM_AddressIsResolved_(::NewControlColorUPP)) {
			
				mControlColorUPP = ::NewControlColorUPP(inProcPtr);
				
			} else {
				NewRoutineDescriptorFuncPtr	newDesc = (NewRoutineDescriptorFuncPtr) UEnvironment::GetSharedLibSymbol(Str_InterfaceLib, "\pNewRoutineDescriptor");
				if (newDesc != nil) {
					mControlColorUPP = (ControlColorUPP) (*newDesc)((ProcPtr) inProcPtr, uppControlColorProcInfo, GetCurrentArchitecture());
				}
			}
			
		#else
	
			mControlColorUPP = NewControlColorUPP(inProcPtr);
			
		#endif
	}
}


StControlColorUPP::~StControlColorUPP()
{
	if (mControlColorUPP != nil) {
	
		#if TARGET_API_MAC_CARBON
		
			if (CFM_AddressIsResolved_(::DisposeControlColorUPP)) {
			
				::DisposeControlColorUPP(mControlColorUPP);
				
			} else {
				DisposeRoutineDescriptorFuncPtr	disposeDesc = (DisposeRoutineDescriptorFuncPtr) UEnvironment::GetSharedLibSymbol(Str_InterfaceLib, "\pDisposeRoutineDescriptor");
				if (disposeDesc != nil) {
					(*disposeDesc)((UniversalProcPtr) mControlColorUPP);
				}
			}
		
		#else
	
			DisposeControlColorUPP(mControlColorUPP);
			
		#endif
	}
}


OSStatus
StControlColorUPP::SetColorProc(
	ControlRef	inMacControl)
{
	#if TARGET_API_MAC_CARBON
	
		if (CFM_AddressIsResolved_(::SetControlColorProc)) {
		
			return ::SetControlColorProc(inMacControl, mControlColorUPP);
			
		} else {
			SetControlColorProcFuncPtr	setProc = (SetControlColorProcFuncPtr) UEnvironment::GetSharedLibSymbol(Str_ControlsLib, "\pSetControlColorProc");
			if (setProc != nil) {
				return (*setProc)(inMacControl, mControlColorUPP);
			}
			return cfragNoSymbolErr;
		}
	
	#else
	
		return ::SetControlColorProc(inMacControl, mControlColorUPP);
		
	#endif
}


PP_End_Namespace_PowerPlant
