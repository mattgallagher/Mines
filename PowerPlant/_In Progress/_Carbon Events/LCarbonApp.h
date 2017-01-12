// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCarbonApp.h				PowerPlant 2.2.2		 ©2000-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LCarbonApp
#define _H_LCarbonApp
#pragma once

#include <LCommander.h>
#include <LModelObject.h>

#include <LEventHandlerFunctor.h>

#include <AERegistry.h>
#include <CarbonEvents.h>

PP_Begin_Namespace_PowerPlant

class	LMenuBar;

// ---------------------------------------------------------------------------

class	LCarbonApp : public LCommander,
					 public LModelObject {
public:
						LCarbonApp();
					
	virtual				~LCarbonApp();
	
	virtual	void		Run();

	virtual Boolean		ObeyCommand(
								CommandT			inCommand,
								void*				ioParam);
								
	virtual void		FindCommandStatus(
								CommandT			inCommand,
								Boolean&			outEnabled,
								Boolean&			outUsesMark,
								UInt16&				outMark,
								Str255				outName);

	virtual void		HandleAppleEvent(
								const AppleEvent&	inAppleEvent,
								AppleEvent&			outAEReply,
								AEDesc&				outResult,
								SInt32				inAENumber);
protected:

	virtual void		StartUp();
								
	virtual void		MakeMenuBar();
	
	virtual void		MakeModelDirector();
	
	virtual void		Initialize();

	virtual void		DoReopenApp();
	
	virtual void		DoPreferences();
	
	virtual void		DoQuit( SInt32 inSaveOption = kAEAsk );
	
	virtual void		MakeSelfSpecifier(
								AEDesc&				inSuperSpecifier,
								AEDesc&				outSelfSpecifier) const;
								
protected:
									// Carbon Event Handlers

	LEventHandlerFunctor		mSuspendHandler;
	LEventHandlerFunctor		mResumeHandler;
	LEventHandlerFunctor		mCommandHandler;
	LEventHandlerFunctor		mMenuEnableHandler;
	LEventHandlerFunctor		mKeyDownHandler;
	LEventHandlerFunctor		mKeyRepeatHandler;
	LEventHandlerFunctor		mMouseMovedHandler;
};

PP_End_Namespace_PowerPlant

#endif
