// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDebugMenuAttachment.h 		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub, MW IDE team.

#ifndef _H_LDebugMenuAttachment
#define _H_LDebugMenuAttachment
#pragma once

#include <PP_Debug.h>
#include <LAttachment.h>
#include <LTreeWindow.h>
#include <LHeapAction.h>
#include <LListener.h>

#if PP_QC_Support
	#include <LPeriodical.h>
	#include <UOnyx.h>
#endif

#include <Aliases.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	SDebugInfo contains the information used by LDebugMenuAttachment to
//	build its menu and execute its commands. You must create an instance
//	of the structure and pass it to LDebugMenuAttachment's constructor
//	when creating the Attachment (akin to how one uses SPaneInfo when
//	creating a Pane on-the-fly).
//
//	Setting a field to zero specifies you do not wish to support that option,
//	however depending upon the option this may or may not work (e.g. specifying
//	zero for the menu ID is pointless). Any other value will attempt to support
//	that option, and any incorrect/invalid values will be handled respective
//	to how that option handles incorrect values (e.g. an exception will be
//	thrown if the specified Resource is not found).

#pragma options align=mac68k

struct SDebugInfo {
	ResIDT			debugMenuID;			// ResIDT of MENU for Debug menu.
											//	Specifing zero will signal an error.
	ResIDT			debugMenuIconID;		// ResIDT of icon suite for Debug menu's title.
											//	Specifying zero will not use the icon and instead
											//	use the title given in the MENU resource.

	ResIDT			commanderTreePPobID;	// ResIDT of PPob for "Commander Chain" (LCommanderTree).
											//	Specifying zero will disable this feature.
	PaneIDT			commanderTreePaneID;	// PaneIDT of LCommanderTree in "Commander Chain" window.
											//	Specifying zero will use the default TreeWindow_Tree
											//	PaneIDT (PP_DebugConstants.h)
	UInt32			commanderTreeThreshold;	// Threshold (in seconds) for the CommanderTree's update
											//	interval. Specifying zero will disable auto-refreshing.

	ResIDT			paneTreePPobID;			// ResIDT of PPob for "Visual Hierarchy" (LPaneTree).
											//	Specifying zero will disable this feature.
	PaneIDT			paneTreePaneID;			// PaneIDT of LPaneTree in "Visual Hierarchy" window.
											//	Specifying zero will use the default TreeWindow_Tree
											//	PaneIDT (PP_DebugConstants.h)
	UInt32			paneTreeThreshold;		// Threshold (in seconds) for the PaneTree's update
											//	interval. Specifying zero will disable auto-refreshing.

	ResIDT			validPPobDlogID;		// ResIDT of "Validate PPob" dialog PPob.
											//	Specifying zero will disable this feature.
	PaneIDT			validPPobEditID;		// PaneIDT of edit field in "Validate PPob" dialog.
											//	Specifying zero will use the default
											//	ValidatePPob_EditResIDT PaneIDT (PP_DebugConstants.h).

	ResIDT			eatMemPPobDlogID;		// ResIDT of "Eat Memory" dialog PPob.
											//	Specifying zero will disable this feature.
	PaneIDT			eatMemRadioHandleID;	// PaneIDT of "Handle" radio button
											//	Specifying zero will use the default
											//	EatMemoryDialog_RadioHandle PaneIDT (PP_DebugConstants.h).
	PaneIDT			eatMemRadioPtrID;		// PaneIDT of "Ptr" radio button
											//	Specifying zero will use the default
											//	EatMemoryDialog_RadioPtr PaneIDT (PP_DebugConstants.h).
	PaneIDT			eatMemEditID;			// PaneIDT of edit field for amount/bytes
											//	Specifying zero will use the default
											//	EatMemoryDialog_EditAmount PaneIDT (PP_DebugConstants.h).
	PaneIDT			eatMemRadioGroupID;		// PaneIDT of LRadioGroupView containing radio buttons
											//	Specifying zero will use the default
											//	EatMemoryDialog_RadioGroup PaneIDT (PP_DebugConstants.h).
};

#pragma options align=reset


// ---------------------------------------------------------------------------

class LMenu;
class LCommanderTree;
class LPaneTree;

class LDebugMenuAttachment :	public LAttachment,
								public LListener
							#if PP_QC_Support
								, public LPeriodical
							#endif
{
public:

					// Simple installer
	static	void			InstallDebugMenu(
									LAttachable*		inMenuHost);

					// Construction
							LDebugMenuAttachment();

							LDebugMenuAttachment(
									SDebugInfo&			inDebugInfo);

	virtual					~LDebugMenuAttachment();

	virtual	void			InitDebugMenu();

					// Utilities
	static	void			SetDebugInfoDefaults(
									SDebugInfo&			outDebugInfo);

	static	bool			IsDebugMenuCommand(
									CommandT			inCommand);
	static	bool			DebugObeyCommand(
									CommandT			inCommand,
									void*				ioParam);
	static	void			DebugFindCommandStatus(
									CommandT			inCommand,
									Boolean&			outEnabled,
									Boolean&			outUsesMark,
									UInt16&				outMark,
									Str255				outName);

	static	ResIDT			FindUniqueMenuID(
									ResIDT				inStartID = 128,
									ResIDT				inStopID = max_Int16,
									bool				inSubMenu = false);

					// Support
	virtual	void			ListenToMessage(
									MessageT			inMessage,
									void*				ioParam);

#if PP_QC_Support
	virtual	void			SpendTime(
									const EventRecord&	inMacEvent);
#endif

protected:
	virtual	void			PreprocessMenu(
									LMenu*				inMenu);

	virtual void			ExecuteSelf(
									MessageT			inMessage,
									void*				ioParam);

	virtual	bool			EnableCommand(
									SCommandStatus&		ioCommand);
	virtual	Boolean			ObeyCommand(
									CommandT			inCommand,
									void*				ioParam = nil);

	static	LDebugMenuAttachment*	sDebugMenuAttachmentP;

			SDebugInfo		mDebugInfo;

			AliasHandle		mZRAlias;
			AliasHandle		mHeapBossAlias;

			LMenu*			mDebugMenuP;
			Handle			mMenuIconSuiteH;

			LHeapAction		mCompactAction;
			LHeapAction		mPurgeAction;
			LHeapAction		mCompactPurgeAction;

	#if PP_QC_Support
			LHeapAction		mQCAction;
			bool			mQCActive;
			Handle			mQCTestStatesH;

			ResIDT			mQCTestMenuID;
			QCTestHandle	mQCTestListH;
			QCStateHandle	mQCStateH;
			SInt32			mQCTestCount;
	#endif

			LTreeWindow*	mCommandTreeWindow;
			LCommanderTree*	mCommandTree;

			LTreeWindow*	mPaneTreeWindow;
			LPaneTree*		mPaneTree;

			bool			mInitialized;

private:
							LDebugMenuAttachment(const LDebugMenuAttachment &inOriginal);
	LDebugMenuAttachment	operator=(const LDebugMenuAttachment &inRhs);

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_LDebugMenuAttachment
