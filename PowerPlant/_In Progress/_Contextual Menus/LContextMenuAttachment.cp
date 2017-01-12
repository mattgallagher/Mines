/********************************************************************************/
/*																				*/
/*	© 2006, Aurbach & Associates, Inc.  All rights reserved.					*/
/*																				*/
/*	Redistribution and use in source and binary forms, with or without			*/
/*	modification, are permitted provided that the following condition			*/
/*	are met:																	*/
/*																				*/
/*	  ¥	Redistributions of source code must retain the above copyright			*/
/*		notice, this list of conditions and the following disclaimer.			*/
/*																				*/
/*	  ¥	Redistributions in binary form must reproduce the above copyright		*/
/*		notice, this list of conditions and the following disclaimer in the		*/
/*		documentation and/or other materials provided with the distribution.	*/
/*																				*/
/*	  ¥	Neither the name of Aurbach & Associates, Inc. nor the names of any		*/
/*		of its employees may be used to endorse or promote products derived		*/
/*		from this software without specific prior written permission.			*/
/*																				*/
/*	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS			*/
/*	ÒAS ISÓ AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT			*/
/*	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A		*/
/*	PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER	*/
/*	OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,	*/
/*	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,			*/
/*	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; */
/*	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,	*/
/*	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR		*/
/*	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF		*/
/*	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.									*/
/*																				*/
/********************************************************************************/
//
//	Module Name:	LContextMenuAttachment
//
//	SuperClass:		LAttachment
//
//	LContextMenuAttachment implements a contextual menu.
//
//	To implement contextual menu support in your application via LContextMenuAttachment:
//
//	-	Add the following files to your project:
//
//			LContextMenuAttachment.cp
//			LContextMenuHelper.cp
//			UCMMUtils.cp
//
//	-	Add LContextMenuAttachments to whatever objects (in your PPob's) you
//		wish to have contextual menus, such as EditText's, Window's,
//		etc. Since this is a menu, commands are used for action dispatch
//		(the same way menus work throughout PowerPlant, e.g. LMenu).
//		Due to this, there is a notion of CommandTarget -- the Commander/target
//		to which the menu command will ultimately be dispatched. This
//		can be the Attachment's host, another Pane in the hierarchy,
//		or a Commander object that you specify. Due to this design,
//		the LContextMenuAttachment can be attached to Commander/Pane objects,
//		but doesn't necessarily have to be.
//
//		Set the proprties for your LContextMenuAttachment accordingly. The
//		various CTYP properties are listed below.
//
//	-	RegisterClass_(LContextMenuAttachment), if you create any instances
//		from a PPob DataStream.
//
//	Alternately, if you are already subclassing the visual object that should have a
//	contextual menu, an alternate approach is to override the object's ContextClickSelf()
//	method.
//
//	LContextMenuAttachment tries to do as much for you as possible, automatically.
//	It creates a little overhead as it has to scan around, but for the flexibility of not
//	always needing to subclass, ease of changing a resource vs. having to recompile, and 
//	aiming for safety over speed (but speed is still an important factor).
//
//	The CTYP layout for LContextMenuAttachment is as follows:
//
//	- Execute Message
//		Doesn't matter. The class will adjust its message value appropriately.
//
//	- Execute Host
//		Doesn't matter. The attachment will "do the right thing".
//
//	- Host Owns Me
//		This can be set as you'd like, but typically should be set to true.
//
//	- Base Menu ID
//		The default value is 0, which means that the attachment creates a new empty menu,
//		which is filled in by the attachment. If a non-zero value is specified, the class
//		will attempt to build the contextual menu based on using the MENU with that ID.
//		In this case, the additional data specified by the attachment is appended to the
//		bottom of this menu.
//
//	- Help Type
//		The type of Help your application supports: no help, Apple Guide, some other help 
//		mechanism, remove help (10.2 or later). This constant is passed ::ContextualMenuSelect().
//		LContextMenuAttachment does not implement any help mechanism(s), but does provide a 
//		ShowHelp() method where you can hook in your help for this context.
//
//	- Help Item String
//		The menu item text for the Help menu item. This is passed to ::ContextualMenuSelect(). 
//		Typically, you should not need to specify a value here, because if none is specified, 
//		the contents of STR# 200, index 5 (= 'Help') is used.
//
//	- Item List
//		This is a list of items that the attachment will add to the contextual menu for you.
//		To add items to the menu, select the "Menu Items" property in the Property Inspector,
//		then select "New Item" from the Edit menu (cmd-K) to add an item. Items will be added
//		to the contextual menu in their Property Inspector display order.
//
//		Each item consists of an item type and a value:
//		  o	Item Type = Menu Item
//			The value should be the value of the command you wish to add. The command must
//			be defined in the current menubar; the attachment will look up the menu item title
//			used in the menubar and use that value for the item.
//
//			Unfortunately, Constructor cannot provide a popup list of standard values here,
//			so you will need to specify the command number manually. (See PP_Messages.h)
//
//		  o	Item Type = Menu ID
//			The value should be the resource ID of a menu. The attachment will append the
//			items from that menu into the contextual menu.
//
//		  o	Item Type = Separator
//			The value is ignored. A separator line will be appended to the menu.
//
//	Who			Date		Description
// --------------------------------------------------------------------------------------
//	RLA		28-Sep-2006		Original Code

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LContextMenuAttachment.h>
#include <LContextMenuHelper.h>
#include <LMenu.h>
#include <LMenuBar.h>
#include <LStream.h>
#include <PP_KeyCodes.h>
#include <PP_Messages.h>
#include <UMemoryMgr.h>

#include <Menus.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LContextMenuAttachment				Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LContextMenuAttachment::LContextMenuAttachment(
	LStream *					inStream)	:	LAttachment(inStream)
{
	SInt16						helpType;
	UInt16						numEntries;

	*inStream >> mMENUid;

	*inStream >> helpType;
	mHelpType = helpType;

	inStream->ReadPString(mHelpString);

	*inStream >> numEntries;

	for (SInt16 i = 1; i <= numEntries; i++) {
		MENTRY					entry;
		*inStream >> entry.mtype;
		*inStream >> entry.mval;
		mEntryList.push_back(entry);
	}
	mMessage = msg_ContextClick;
}


// --------------------------------------------------------------------------
//	¥ LContextMenuAttachment			Parameterized Constructor	[public]
// --------------------------------------------------------------------------

LContextMenuAttachment::LContextMenuAttachment (
	MessageT					inMessage,
	Boolean						inExecuteHost,
	SInt16						inBaseMenuID,
	UInt32						inHelpType,
	const LStr255 &				inHelpString,
	SInt16						inEntryCount,
	MENTRY *					inEntries )			:	LAttachment ( inMessage,
																	  inExecuteHost )
{
	mMENUid = inBaseMenuID;
	mHelpType = inHelpType;
	mHelpString.Assign(inHelpString);
	
	SInt16						numEntries = inEntryCount;
	for (SInt16 i = 0; i < numEntries; i++) {
		MENTRY					entry = inEntries[i];
		mEntryList.push_back(entry);
	}
}


// ---------------------------------------------------------------------------
//	¥ ExecuteSelf										[protected, virtual]
// ---------------------------------------------------------------------------

void
LContextMenuAttachment::ExecuteSelf(
	MessageT					inMessage,
	void*						ioParam)
{
	SetExecuteHost(true);

	if (inMessage == msg_ContextClick) {
		LCommander *			tgt = FindCommandTarget();
		if (tgt) {
			StDeleter<LContextMenuHelper>	helper(CreateHelper(mMENUid, tgt));

			LPane *				pane = dynamic_cast<LPane*>(mOwnerHost);
			helper->SetContextPane(pane);
			helper->SpecifyHelpString(mHelpString);
			helper->SpecifyHelpType(mHelpType);
			PrepareForMenu(helper);
			
			std::vector<MENTRY>::iterator	iter = mEntryList.begin();
			while (iter != mEntryList.end()) {
				switch (iter->mtype) {
					case kCMAItemSeparator: {
						helper->AppendMenuSeparator();
						break;
					}
					case kCMAItemMenuItem: {
						LStr255		cmdStr;
						CommandT	cmd = (CommandT) iter->mval;
						if (FindCommandString(cmd, cmdStr)) {
							helper->AppendMenuCommand(cmdStr, cmd);
						}
						break;
					}
					case kCMAItemMenuID: {
						SInt16	mID = (SInt16) iter->mval;
						helper->AppendMenuCommandList(mID);
						break;
					}
				}
				iter++;
			}
			
			Point				globalPt = *(Point*) ioParam;
			helper->TrackMenu(globalPt);
		}
		SetExecuteHost(tgt == nil);
	}
}


// ---------------------------------------------------------------------------
//	¥ CreateHelper										[protected, virtual]
// ---------------------------------------------------------------------------
//	Create the helper object used in ExecuteSelf. If you are overriding
//	LContextMenuHelper (for example, to handle help requests), then you
//	should also override this method to instantiate your new class.

LContextMenuHelper *
LContextMenuAttachment::CreateHelper (
	SInt16						inMenuID,
	LCommander *				inCtxTarget )
{
	LContextMenuHelper *		helper = nil;
	if (inMenuID == 0) {
		helper = new LContextMenuHelper(inCtxTarget);
	} else {
		helper = new LContextMenuHelper(inMenuID, inCtxTarget);
	}
	return helper;
}


// ---------------------------------------------------------------------------
//	¥ FindCommandString									[protected, virtual]
// ---------------------------------------------------------------------------
//	Search the current menu bar for a command with the specified value.
//	If found, return its item title.
//
//	This function was based on LCMAttachment::AddCommand() by John C. Daub,
//	with input from David Catmull.

bool
LContextMenuAttachment::FindCommandString(
	CommandT					inCommand,
	LString &					outString)
{
	bool						cmdFound = false;

	if (inCommand != cmd_Nothing) {
		LMenuBar*				theMenuBar = LMenuBar::GetCurrentMenuBar();
		Assert_(theMenuBar != nil);

		LMenu*					theBarMenu = nil;
		while (theMenuBar->FindNextMenu(theBarMenu)) {
			SInt16				theIndex = theBarMenu->IndexFromCommand(inCommand);
			if (theIndex == 0) {		// Command not in that menu, keep looking
				continue;
			}

			MenuHandle			theMacMenuH = theBarMenu->GetMacMenuH();
			Assert_(theMacMenuH != nil);

			::GetMenuItemText(theMacMenuH, theIndex, outString);

			cmdFound = true;
			break;
		}
	}

	return cmdFound;
}


// ---------------------------------------------------------------------------
//	¥ FindCommandTarget									[protected, virtual]
// ---------------------------------------------------------------------------
//	Attempt to locate the CommandTarget (the Commander/Target to which the
//	menu command will ultimately be dispatched).
//	If the attachment's owner is a visual object,
//		If it is also a commander,
//			It is the target.
//		Else,
//			check its superview.
//	Else
//		Cast the owner as the target.

LCommander*
LContextMenuAttachment::FindCommandTarget()
{
	LCommander *				tgt = nil;
	LPane *						pane = dynamic_cast<LPane*>(mOwnerHost);
	if (pane == nil) {
		tgt = dynamic_cast<LCommander*>(mOwnerHost);
	} else {
		while (pane != nil) {
			tgt = dynamic_cast<LCommander*>(pane);
			if (tgt != nil) {
				break;
			} else {
				pane = pane->GetSuperView();
			}
		}
	}
	
	Assert_(tgt != nil);			// It is a programming error if we don't find a target.
	
	return tgt;
}

#pragma mark -
//
//	Class Name:		LContextMenuForwarder
//
//	SuperClass:		LAttachment
//
//	LContextMenuForwarder is an attachment which forwards the contextual menu message it
//	handles to the host's LCommander in the form of a command message.
//
//	The rationale for this attachment is that, while many visual objects in PowerPlant are
//	standard objects, their commanders are often custom, subclassed objects which are built
//	for their specific purpose. This attachment forwards the responsibility for building
//	and processing the contextual message for this object to that commander, just as other
//	menu commands would be.
//
//	The parameter passed to the commander is a structure containing the global point where
//	the contextual menu should be displayed and a pointer to the attachment's owner pane
//	(so that the commander can distinguish between contextual menus from different sources).
//
//	The CTYP layout for LContextMenuForwarder is as follows:
//	- Execute Message
//		Doesn't matter. The class will adjust its message value appropriately.
//
//	- Execute Host
//		Doesn't matter. The attachment will "do the right thing".
//
//	- Host Owns Me
//		This can be set as you'd like, but typically should be set to true.
//
//	Who			Date		Description
// --------------------------------------------------------------------------------------
//	RLA		28-Sep-2006		Original Code

// ---------------------------------------------------------------------------
//	¥ LContextMenuForwarder				Stream Constructor			[public]
// ---------------------------------------------------------------------------

LContextMenuForwarder::LContextMenuForwarder(
	LStream *					inStream)	:	LAttachment(inStream)
{
	mMessage = msg_ContextClick;
	mTarget = nil;
}


// --------------------------------------------------------------------------
//	¥ LContextMenuForwarder			Paramterized Constructor		[public]
// --------------------------------------------------------------------------

LContextMenuForwarder::LContextMenuForwarder (
	LCommander *				inCommandTarget,
	Boolean						inExecuteHost )	:	LAttachment(msg_ContextClick,
																inExecuteHost)
{
	mTarget = inCommandTarget;
}


// ---------------------------------------------------------------------------
//	¥ ExecuteSelf										[protected, virtual]
// ---------------------------------------------------------------------------

void
LContextMenuForwarder::ExecuteSelf (
	MessageT					inMessage,
	void *						ioParam )
{
	bool						processed = false;
	
	if (inMessage == msg_ContextClick) {
		LCommander *			cmdr = mTarget;
		if (cmdr == nil) {
			cmdr = FindCommandTarget();
		}
		if (cmdr != nil) {
			SCMForward			param;
			param.globalPt = *(Point*) ioParam;
			param.ownerHost = mOwnerHost;
			processed = cmdr->ProcessCommand(inMessage, (void*) &param);
		}
	}
	SetExecuteHost(!processed);
}


// ---------------------------------------------------------------------------
//	¥ FindCommandTarget									[protected, virtual]
// ---------------------------------------------------------------------------
//	Attempt to locate the CommandTarget (the Commander/Target to which the
//	menu command will ultimately be dispatched).
//	If the attachment's owner is a visual object,
//		If it is also a commander,
//			It is the target.
//		Else,
//			check its superview.
//	Else
//		Cast the owner as the target.

LCommander*
LContextMenuForwarder::FindCommandTarget()
{
	LCommander *				tgt = nil;
	LPane *						pane = dynamic_cast<LPane*>(mOwnerHost);
	if (pane == nil) {
		tgt = dynamic_cast<LCommander*>(mOwnerHost);
	} else {
		while (pane != nil) {
			tgt = dynamic_cast<LCommander*>(pane);
			if (tgt != nil) {
				break;
			} else {
				pane = pane->GetSuperView();
			}
		}
	}
	
	Assert_(tgt != nil);			// It is a programming error if we don't find a target.
	
	return tgt;
}

PP_End_Namespace_PowerPlant
