// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UControlMgr.h				PowerPlant 2.2.2	©1999-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UControlMgr
#define _H_UControlMgr
#pragma once

#include <PP_Prefix.h>
#include <Controls.h>
#include <ControlDefinitions.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif


PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------

namespace UControlMgr {

	inline
	SInt32		SendControlMessage(
						ControlHandle			inMacControlH,
						SInt16					inMessage,
						void*					inParam)
					{
					#if TARGET_API_MAC_CARBON

						return ::SendControlMessage(inMacControlH, inMessage,
													inParam);

					#else

						// SendControlMessage is an Appearance Mgr call.
						// But, prior to Carbon, the CallControlDefProc works
						// whether or not AM is present

						return CallControlDefProc(
									(ControlDefUPP)
										*((**inMacControlH).contrlDefProc),
									0, inMacControlH, inMessage, (SInt32) inParam);

					#endif
					}
}

#pragma mark -

// ---------------------------------------------------------------------------
#pragma mark StControlActionUPP

class StControlActionUPP {
public:
			StControlActionUPP( ControlActionProcPtr inProcPtr );
			~StControlActionUPP();

			operator ControlActionUPP()		{ return mControlActionUPP; }

protected:
	ControlActionUPP	mControlActionUPP;
};


// ---------------------------------------------------------------------------
#pragma mark StControlDefUPP

class StControlDefUPP {
public:
			StControlDefUPP( ControlDefProcPtr inProcPtr );
			~StControlDefUPP();

			operator ControlDefUPP()		{ return mControlDefUPP; }

protected:
	ControlDefUPP	mControlDefUPP;
};


// ---------------------------------------------------------------------------
#pragma mark StControlColorUPP

class StControlColorUPP {
public:
			StControlColorUPP( ControlColorProcPtr inProcPtr );
			~StControlColorUPP();
			
			operator ControlColorUPP()		{ return mControlColorUPP; }
			
	OSStatus	SetColorProc( ControlRef inMacControl );
			
protected:
	ControlColorUPP	mControlColorUPP;
};


PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
