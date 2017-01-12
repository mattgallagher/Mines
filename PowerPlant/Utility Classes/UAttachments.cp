// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UAttachments.cp				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	An assortment of Attachment classes

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UAttachments.h>
#include <UDrawingState.h>
#include <PP_KeyCodes.h>
#include <LStream.h>
#include <LView.h>

#include <Sound.h>

PP_Begin_Namespace_PowerPlant


// ===========================================================================
// ¥ LCommanderPane
// ===========================================================================
//	A subclass of LCommander and LPane
//
//	An object of this class would do nothing by itself, but is a powerful
//	concept when combined with Attachments which control all drawing,
//	printing, clicking, and command handling.

LCommanderPane::LCommanderPane(
	SPaneInfo&		inPaneInfo,
	LCommander*		inSuper)

	: LPane(inPaneInfo),
	  LCommander(inSuper)
{
}


LCommanderPane::LCommanderPane(
	LStream*	inStream)

	: LPane(inStream)
{
}


LCommanderPane::~LCommanderPane()
{
}

#pragma mark -

// ===========================================================================
// ¥ LBeepAttachment
// ===========================================================================
//	Beeps when executed
//		Suitable for use with any message

LBeepAttachment::LBeepAttachment(
	MessageT	inMessage,
	Boolean		inExecuteHost)

	: LAttachment(inMessage, inExecuteHost)
{
}


LBeepAttachment::LBeepAttachment(
	LStream*	inStream)

	: LAttachment(inStream)
{
}


void
LBeepAttachment::ExecuteSelf(
	MessageT	/* inMessage */,
	void*		/* ioParam */)
{
	::SysBeep(1);
}

#pragma mark -

// ===========================================================================
// ¥ LEraseAttachment
// ===========================================================================
//	Erases the Frame of a Pane
//		For use only with msg_DrawOrPrint

LEraseAttachment::LEraseAttachment(
	Boolean	inExecuteHost)

	: LAttachment(msg_DrawOrPrint, inExecuteHost)
{
}


LEraseAttachment::LEraseAttachment(
	LStream*	inStream)

	: LAttachment(inStream)
{
	mMessage = msg_DrawOrPrint;
}


void
LEraseAttachment::ExecuteSelf(
	MessageT	/* inMessage */,
	void*		ioParam)
{
	::EraseRect(static_cast<Rect*>(ioParam));
}

#pragma mark -

// ===========================================================================
// ¥ LColorEraseAttachment
// ===========================================================================
//	Erases the Frame of a Pane
//		For use only with msg_DrawOrPrint

LColorEraseAttachment::LColorEraseAttachment(
	const RGBColor*		inBackColor,
	Boolean				inExecuteHost)

	: LAttachment(msg_DrawOrPrint, inExecuteHost)
{
	if (inBackColor == nil) {		// Default to white background
		mBackColor = Color_White;
	} else {
		mBackColor = *inBackColor;
	}

	mForeColor = Color_Black;
}


//	NOTE: PenState parameter is unnecessary. The current PenState does not
//	affect EraseRect. The parameter exists for historical reasons; removing
//	it would break existing code.
//
//	ForeColor has affect only if the BackPat (or BackPixPat) is not white.

LColorEraseAttachment::LColorEraseAttachment(
	const PenState*		/* inPenState */,
	const RGBColor*		inForeColor,
	const RGBColor*		inBackColor,
	Boolean				inExecuteHost)

	: LAttachment(msg_DrawOrPrint, inExecuteHost)
{
	if (inForeColor == nil) {		// Default to black foreground
		mForeColor = Color_Black;
	} else {
		mForeColor = *inForeColor;
	}

	if (inBackColor == nil) {		// Default to white background
		mBackColor = Color_White;
	} else {
		mBackColor = *inBackColor;
	}
}


LColorEraseAttachment::LColorEraseAttachment(
	LStream*	inStream)

	: LAttachment(inStream)
{
	mMessage = msg_DrawOrPrint;		// We only handle this message

	*inStream >> mForeColor.red;
	*inStream >> mForeColor.green;
	*inStream >> mForeColor.blue;

	*inStream >> mBackColor.red;
	*inStream >> mBackColor.green;
	*inStream >> mBackColor.blue;
}


void
LColorEraseAttachment::ExecuteSelf(
	MessageT	/* inMessage */,
	void*		ioParam)
{
	StColorState	saveColorState;		// Save/Restore fore & back colors

	::RGBForeColor(&mForeColor);
	::RGBBackColor(&mBackColor);

	::EraseRect(static_cast<Rect*>(ioParam));
}

#pragma mark -

// ===========================================================================
// ¥ LBorderAttachment
// ===========================================================================
//	Draws a border within the Frame of a Pane
//		For use only with msg_DrawOrPrint

LBorderAttachment::LBorderAttachment(
	const PenState*		inPenState,
	const RGBColor*		inForeColor,
	const RGBColor*		inBackColor,
	Boolean				inExecuteHost)

	: LAttachment(msg_DrawOrPrint, inExecuteHost)
{
	if (inPenState == nil) {		// Default to PenNormal state
		mPenState.pnLoc = Point_00;
		mPenState.pnSize.h = 1;
		mPenState.pnSize.v = 1;
		mPenState.pnMode = patCopy;
		UQDGlobals::GetBlackPat(&mPenState.pnPat);

	} else {
		mPenState = *inPenState;
	}

	if (inForeColor == nil) {		// Default to black foreground
		mForeColor = Color_Black;
	} else {
		mForeColor = *inForeColor;
	}

	if (inBackColor == nil) {		// Default to white background
		mBackColor = Color_White;
	} else {
		mBackColor = *inBackColor;
	}
}


LBorderAttachment::LBorderAttachment(
	LStream*	inStream)

	: LAttachment(inStream)
{
	mMessage = msg_DrawOrPrint;		// We only handle this message

	*inStream >> mPenState.pnSize;
	*inStream >> mPenState.pnMode;

	SInt16		patternIndex;
	*inStream >> patternIndex;

#if TARGET_API_MAC_CARBON

	switch (patternIndex) {
		case	0:
			UQDGlobals::GetDarkGrayPat(&mPenState.pnPat);
			break;
		case	1:
			UQDGlobals::GetLightGrayPat(&mPenState.pnPat);
			break;
		case	2:
			UQDGlobals::GetGrayPat(&mPenState.pnPat);
			break;
		case	3:
			UQDGlobals::GetBlackPat(&mPenState.pnPat);
			break;
		case	4:
			UQDGlobals::GetWhitePat(&mPenState.pnPat);
			break;
	}

#else

	Pattern		*firstPat = &UQDGlobals::GetQDGlobals()->dkGray;
	mPenState.pnPat = *(firstPat + patternIndex);

#endif

	mPenState.pnLoc = Point_00;

	*inStream >> mForeColor.red;
	*inStream >> mForeColor.green;
	*inStream >> mForeColor.blue;

	*inStream >> mBackColor.red;
	*inStream >> mBackColor.green;
	*inStream >> mBackColor.blue;
}


void
LBorderAttachment::ExecuteSelf(
	MessageT	/* inMessage */,
	void*		ioParam)
{
	StColorPenState	savePenState;		// Will save and restore pen state

	::SetPenState(&mPenState);
	::RGBForeColor(&mForeColor);
	::RGBBackColor(&mBackColor);
	::MacFrameRect(static_cast<Rect*>(ioParam));
}

#pragma mark -

// ===========================================================================
// ¥ LPaintAttachment
// ===========================================================================
//	Paints a rectangle within the Frame of a Pane
//		For use only with msg_DrawOrPrint
//
//	The rectangle is painted using the specified PenState settings and
//	foreground and background colors. The painted rectangle is inset from
//	the Frame by the size of the pnSize field of the PenState. This lets
//	you use an LPaintAttachment in conjuction with a LBorderAttachment
//	to draw a filled rectangle.

LPaintAttachment::LPaintAttachment(
	const PenState*		inPenState,
	const RGBColor*		inForeColor,
	const RGBColor*		inBackColor,
	Boolean				inExecuteHost)

	: LAttachment(msg_DrawOrPrint, inExecuteHost)
{
	if (inPenState == nil) {		// Default to PenNormal state
		mPenState.pnLoc = Point_00;
		mPenState.pnSize.h = 1;
		mPenState.pnSize.v = 1;
		mPenState.pnMode = patCopy;
		UQDGlobals::GetBlackPat(&mPenState.pnPat);

	} else {
		mPenState = *inPenState;
	}

	if (inForeColor == nil) {		// Default to black foreground
		mForeColor = Color_Black;
	} else {
		mForeColor = *inForeColor;
	}

	if (inBackColor == nil) {		// Default to white background
		mBackColor = Color_White;
	} else {
		mBackColor = *inBackColor;
	}
}


LPaintAttachment::LPaintAttachment(
	LStream*	inStream)

	: LAttachment(inStream)
{
	mMessage = msg_DrawOrPrint;		// We only handle this message

	*inStream >> mPenState.pnSize;
	*inStream >> mPenState.pnMode;

	SInt16		patternIndex;
	*inStream >> patternIndex;

#if TARGET_API_MAC_CARBON

	switch (patternIndex) {
		case	0:
			UQDGlobals::GetDarkGrayPat(&mPenState.pnPat);
			break;
		case	1:
			UQDGlobals::GetLightGrayPat(&mPenState.pnPat);
			break;
		case	2:
			UQDGlobals::GetGrayPat(&mPenState.pnPat);
			break;
		case	3:
			UQDGlobals::GetBlackPat(&mPenState.pnPat);
			break;
		case	4:
			UQDGlobals::GetWhitePat(&mPenState.pnPat);
			break;
	}

#else

	Pattern		*firstPat = &UQDGlobals::GetQDGlobals()->dkGray;
	mPenState.pnPat = *(firstPat + patternIndex);

#endif

	mPenState.pnLoc = Point_00;

	*inStream >> mForeColor.red;
	*inStream >> mForeColor.green;
	*inStream >> mForeColor.blue;

	*inStream >> mBackColor.red;
	*inStream >> mBackColor.green;
	*inStream >> mBackColor.blue;
}


void
LPaintAttachment::ExecuteSelf(
	MessageT	/* inMessage */,
	void*		ioParam)
{
	StColorPenState	savePenState;		// Will save and restore pen state

	::SetPenState(&mPenState);
	::RGBForeColor(&mForeColor);
	::RGBBackColor(&mBackColor);

	Rect	r = *(static_cast<Rect*>(ioParam));
	::MacInsetRect(&r, mPenState.pnSize.h, mPenState.pnSize.v);
	::PaintRect(&r);
}

#pragma mark -

// ===========================================================================
// ¥ LCmdEnablerAttachment
// ===========================================================================
//	Enables a particular menu command
//		For use only with msg_CommandStatus

LCmdEnablerAttachment::LCmdEnablerAttachment(
	CommandT	inCmdToEnable)

	: LAttachment(msg_CommandStatus)
{
	mCmdToEnable = inCmdToEnable;
}


LCmdEnablerAttachment::LCmdEnablerAttachment(
	LStream*	inStream)

	: LAttachment(inStream)
{
	mMessage = msg_CommandStatus;	// We only handle this message

	*inStream >> mCmdToEnable;
}


void
LCmdEnablerAttachment::ExecuteSelf(
	MessageT	/* inMessage */,
	void*		ioParam)
{
	mExecuteHost = true;
	if ((static_cast<SCommandStatusP>(ioParam))->command == mCmdToEnable) {
									// This is our command, enable it
		*(static_cast<SCommandStatusP>(ioParam))->enabled = true;
		mExecuteHost = false;		// We have enabled the command, so don't
									//   bother to ask the host
	}
}

#pragma mark -

// ===========================================================================
// ¥ LKeyScrollAttachment
// ===========================================================================
//	Handles scrolling a View using keyboard navigation keys:
//		Home, End, PageUp, PageDown
//
//		For use only with msg_KeyPress
//
//	If you have a View that is also a Commander, you can attach a
//	LKeyScrollAttachment to it to implement keyboard navigation.
//
//	If your View is not a Commander, but you still want to implement
//	keyboard navigation, you can attach a LKeyScrollAttachment to
//	a SuperView that is a Commander (such as the Window containing
//	the View). However, if you can delete the View independent of
//	the Window, you must take care to delete the Attachment.

LKeyScrollAttachment::LKeyScrollAttachment(
	LView*	inViewToScroll)

	: LAttachment(msg_KeyPress)
{
	mViewToScroll = inViewToScroll;
}


LKeyScrollAttachment::LKeyScrollAttachment(
	LStream*	inStream)

	: LAttachment(inStream)
{
	mMessage = msg_KeyPress;	// We only handle this message
	mViewToScroll = LView::GetDefaultView();
}


void
LKeyScrollAttachment::ExecuteSelf(
	MessageT	/* inMessage */,
	void*		ioParam)
{
	mExecuteHost = false;		// We handle navigation keys
	SInt16	theKey = (SInt16) ((static_cast<EventRecord*>(ioParam))->message & charCodeMask);

	switch (theKey) {

		case char_Home:			// Scroll to top left
			mViewToScroll->ScrollImageTo(0, 0, true);
			break;

		case char_End: {		// Scroll to bottom right
			SDimension16	frameSize;
			SDimension32	imageSize;
			mViewToScroll->GetFrameSize(frameSize);
			mViewToScroll->GetImageSize(imageSize);

			SInt32	leftLocation = imageSize.width - frameSize.width;
			if (leftLocation < 0) {		// If Image is narrower than Frame,
				leftLocation = 0;		//   scroll to left edge
			}

			SInt32	topLocation = imageSize.height - frameSize.height;
			if (topLocation < 0) {		// If Image is shorter than Frame,
				topLocation = 0;		//   scroll to top edge
			}

			mViewToScroll->ScrollImageTo(leftLocation, topLocation,
												true);
			break;
		}

		case char_PageUp: {		// Scroll up by height of Frame,
								//   but not past top of Image
			SPoint32		frameLoc;
			SPoint32		imageLoc;
			mViewToScroll->GetFrameLocation(frameLoc);
			mViewToScroll->GetImageLocation(imageLoc);

			SInt32	upMax = frameLoc.v - imageLoc.v;
			if (upMax > 0) {
				SPoint32		scrollUnit;
				SDimension16	frameSize;
				mViewToScroll->GetScrollUnit(scrollUnit);
				mViewToScroll->GetFrameSize(frameSize);

				SInt32	up = (frameSize.height / scrollUnit.v) - 1;
				if (up <= 0) {
					up = 1;
				}
				up *= scrollUnit.v;
				if (up > upMax) {
					up = upMax;
				}
				mViewToScroll->ScrollImageBy(0, -up, true);
			}
			break;
		}

		case char_PageDown: {	// Scroll down by height of Frame,
								//   but not past bottom of Image
			SPoint32		frameLoc;
			SPoint32		imageLoc;
			SDimension16	frameSize;
			SDimension32	imageSize;
			mViewToScroll->GetFrameLocation(frameLoc);
			mViewToScroll->GetImageLocation(imageLoc);
			mViewToScroll->GetFrameSize(frameSize);
			mViewToScroll->GetImageSize(imageSize);

			SInt32	downMax = imageSize.height - frameSize.height -
								(frameLoc.v - imageLoc.v);
			if (downMax > 0) {
				SPoint32		scrollUnit;
				mViewToScroll->GetScrollUnit(scrollUnit);

				SInt32	down = (frameSize.height / scrollUnit.v) - 1;
				if (down <= 0) {
					down = 1;
				}
				down *= scrollUnit.v;
				if (down > downMax) {
					down = downMax;
				}
				mViewToScroll->ScrollImageBy(0, down, true);
			}
			break;
		}

		default:
			mExecuteHost = true;	// Some other key, let host respond
			break;
	}
}


PP_End_Namespace_PowerPlant
