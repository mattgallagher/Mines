// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTreeWindow.h				PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub

#ifndef _H_LTreeWindow
#define _H_LTreeWindow
#pragma once

#include <LWindow.h>
#include <LBroadcaster.h>
#include <LListener.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LTree;

class LTreeWindow :	public LWindow,
					public LBroadcaster,
					public LListener {
public:
			enum { class_ID = FOUR_CHAR_CODE('twin') };

	static	LTreeWindow*	CreateTreeWindow(
								ResIDT				inPPobID,
								LCommander*			inSuperCommander,
								UInt32				inThreshold);

						LTreeWindow();
						LTreeWindow(
								const SWindowInfo&	inWindowInfo,
								PaneIDT				inTreeID);
						LTreeWindow(
								ResIDT				inWINDid,
								UInt32				inAttributes,
								LCommander*			inSuperCommander,
								PaneIDT				inTreeID);
						LTreeWindow(
								LCommander*			inSuperCommander,
								const Rect&			inGlobalBounds,
								ConstStringPtr		inTitle,
								SInt16				inProcID,
								UInt32				inAttributes,
								WindowPtr			inBehind,
								PaneIDT				inTreeID);
						LTreeWindow(
								LStream*			inStream);

	virtual				~LTreeWindow();

			void		SetTree(
								LTree*				inTree);
			LTree*		GetTree() const
							{
								return mTree;
							}

			void		SetTreeID(
								PaneIDT				inTreeID);
			PaneIDT		GetTreeID() const
							{
								return mTreeID;
							}

			void		SetOldThreshold(
								UInt32				inOldThreshold)
							{
								mOldThreshold = inOldThreshold;
							}

	virtual	void		ListenToMessage(
								MessageT			inMessage,
								void*				ioParam);

protected:
			LTree*		mTree;
			UInt32		mOldThreshold;
			PaneIDT		mTreeID;

	virtual	void		FinishCreateSelf();

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_LTreeWindow
