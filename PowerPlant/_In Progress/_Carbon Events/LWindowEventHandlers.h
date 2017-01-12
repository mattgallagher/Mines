// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LWindowEventHandlers.h		PowerPlant 2.2.2		 ©2001-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LWindowEventHandlers
#define _H_LWindowEventHandlers
#pragma once

#include <TEventHandler.h>
#include <TTimerTask.h>

PP_Begin_Namespace_PowerPlant

class	LWindow;

// ---------------------------------------------------------------------------

class	LWindowEventHandlers {
public:
						LWindowEventHandlers( LWindow* inWindow );
						
						~LWindowEventHandlers();
						
	void				InstallEventHandlers();

	OSStatus			DrawContent(
								EventHandlerCallRef	inCallRef,
								EventRef			inEventRef);

	OSStatus			Activated(
								EventHandlerCallRef	inCallRef,
								EventRef			inEventRef);

	OSStatus			Deactivated(
								EventHandlerCallRef	inCallRef,
								EventRef			inEventRef);

	OSStatus			ClickContentRgn(
								EventHandlerCallRef	inCallRef,
								EventRef			inEventRef);

	OSStatus			GetMinimumSize(
								EventHandlerCallRef	inCallRef,
								EventRef			inEventRef);

	OSStatus			GetMaximumSize(
								EventHandlerCallRef	inCallRef,
								EventRef			inEventRef);

	OSStatus			BoundsChanged(
								EventHandlerCallRef	inCallRef,
								EventRef			inEventRef);

	OSStatus			Zoom(	EventHandlerCallRef	inCallRef,
								EventRef			inEventRef);

	OSStatus			Close(	EventHandlerCallRef	inCallRef,
								EventRef			inEventRef);

	OSStatus			MouseMoved(
								EventHandlerCallRef	inCallRef,
								EventRef			inEventRef);
	
	OSStatus			InputText(
								EventHandlerCallRef	inCallRef,
								EventRef			inEventRef);
	
	OSStatus			ShowWindow (
								EventHandlerCallRef	inCallRef,
								EventRef			inEventRef);

private:
	LWindow*	mWindow;
	
	TEventHandler<LWindowEventHandlers>	mDrawContent;
	TEventHandler<LWindowEventHandlers>	mActivated;
	TEventHandler<LWindowEventHandlers>	mDeactivated;
	TEventHandler<LWindowEventHandlers>	mClickContentRgn;
	TEventHandler<LWindowEventHandlers>	mGetMinimumSize;
	TEventHandler<LWindowEventHandlers>	mGetMaximumSize;
	TEventHandler<LWindowEventHandlers>	mBoundsChanged;
	TEventHandler<LWindowEventHandlers>	mZoom;
	TEventHandler<LWindowEventHandlers>	mClose;
	TEventHandler<LWindowEventHandlers>	mMouseMoved;
	TEventHandler<LWindowEventHandlers>	mInputText;
	TEventHandler<LWindowEventHandlers>	mShowWindow;
};


PP_End_Namespace_PowerPlant

#endif
