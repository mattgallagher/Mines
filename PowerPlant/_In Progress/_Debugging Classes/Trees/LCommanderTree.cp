// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCommanderTree.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Greg Bolsinga, John C. Daub
//
//	A Pane that will display a visual reprsentation of the Commander chain.
//	Allows one to see each Commander in the chain, its super- and sub-
//	Commanders, its state (on, latent, off), the current chain, and the
//	target. If the Commander is also a Pane, the object's PaneIDT is
//	displayed as well for easy identification. RTTI is utilized to obtain
//	other information, such as the Commander's name (again for ease of
//	reference and identification).
//
//	This class is a very handy development/debugging tool to help
//	determine Commander chain validity and diagnose Commander chain
//	problems (a common enough occurance).
//
//	The Tree is an LPeriodical Repeater with a user-settable threshold
//	for how often the Tree should be refreshed (see LTree).
//
//	Note that the Window containing the LCommanderTree will appear in
//	the CommanderTree display as it itself is a Commander. This possibly
//	could affect the behavior of your Application (as the CommanderTree
//	Window could become target, etc.) but a well-behaved Application
//	should not be affected... and of course, any problems should be
//	made visible to you by use of the LCommanderTree. :)
//
//	Note as well that if your Commander chain is very messed up, it's
//	possible that LCommanderTree might show very invalid data and/or
//	cause a crash, as walking the Commander chain and accessing invalid
//	data could certainly cause this. There isn't much that can be done
//	to avoid this other than allowing the bug to be brought to light
//	during development cycle so it can be fixed before your customers
//	see it.
//
//	The Commander information is displayed using user-specified cosmetics:
//
//		-	All data will initially be displayed in the base TextTraitsRecord
//			that you help to specify (see the CTYP for LCommanderTree).
//			In the PPob, you can specify your own 'Txtr' resource, or
//			simply specify 0 to allow default settings to be used.
//			A setup of 9 point Monaco, black, srcOr, plain, left justify
//			is a good baseline setup.
//
//		-	The target is displayed in the base Txtr plus the "Target
//			Color" that you specify. Red is a good color as it stands out.
//
//		-	Commanders in the current Command chain are drawn in the
//			base Txtr plus the Style that you provide. Bold is a good
//			choice.
//
//		-	Latent Commanders are drawn in the base Txtr plus the
//			"Latent Sub Style" that you specify. Italics is a good choice.
//
//	No great effort is made to ensure the style information does not
//	overlap. This responsibility rests upon you, the user.
//
//	The original LCommanderTree (shipped on CodeWarrior Professional 2) was
//	written by Greg Bolsinga. John C. Daub rewrote the class (into what you
//	see here).
//
//		THE ORIGINAL LCommanderTree AND THIS LCommanderTree ARE TOTALLY
//		DIFFERENT AND INCOMPATABLE WITH EACH OTHER. YOU'VE BEEN WARNED.
//
//	About his original work, Greg Bolsinga says: LCommanderTree is a
//	quick and dirty hack. :) It is intended to be lightweight and to
//	the point in the information that it provides. It might not be very
//	pretty, but it is effective. Making it pretty would involve more work
//	and overhead than the class is really worth. Hope you find it useful!


#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LCommanderTree.h>
#include <LStream.h>
#include <TArrayIterator.h>
#include <UDrawingUtils.h>
#include <UEnvironment.h>
#include <LTextEditView.h>
#include <PP_KeyCodes.h>
#include <PP_DebugMacros.h>
#include <LString.h>
#include <UTBAccessors.h>

#ifdef __GNUC__
    #include <typeinfo>
#else
    #if __option(RTTI)
        #include <typeinfo>
    #else
        #error "RTTI option disabled -- Must be enabled for LCommanderTree to function"
    #endif
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LCommanderTree									[public]
// ---------------------------------------------------------------------------
//	Default constructor

LCommanderTree::LCommanderTree()
{
	mCurTargetColor	= Color_Red;
	mCurChainStyle	= bold;
	mLatentSubStyle	= italic;
}


// ---------------------------------------------------------------------------
//	¥ LCommanderTree									[public]
// ---------------------------------------------------------------------------
//	Copy constructor

LCommanderTree::LCommanderTree(
	const LCommanderTree&	inOriginal)

	: LTree(inOriginal)
{
	mCurTargetColor	= inOriginal.mCurTargetColor;
	mCurChainStyle	= inOriginal.mCurChainStyle;
	mLatentSubStyle	= inOriginal.mLatentSubStyle;
}


// ---------------------------------------------------------------------------
//	¥ LCommanderTree									[public]
// ---------------------------------------------------------------------------
//	Parameterized constructor

LCommanderTree::LCommanderTree(
	const SPaneInfo&		inPaneInfo,
	const SViewInfo&		inViewInfo,
	UInt32					inThreshold,
	ResIDT					inTxtrID,
	const RGBColor&			inCurTargetColor,
	Style					inCurChainStyle,
	Style					inLatentSubStyle)

	: LTree(inPaneInfo, inViewInfo, inThreshold, inTxtrID)
{
	mCurTargetColor	= inCurTargetColor;
	mCurChainStyle	= inCurChainStyle;
	mLatentSubStyle	= inLatentSubStyle;
}


// ---------------------------------------------------------------------------
//	¥ LCommanderTree									[public]
// ---------------------------------------------------------------------------
//	LStream constructor

LCommanderTree::LCommanderTree(
	LStream*	inStream)

	: LTree(inStream)
{
	*inStream >> mCurTargetColor.red;
	*inStream >> mCurTargetColor.green;
	*inStream >> mCurTargetColor.blue;

	*inStream >> mCurChainStyle;
	*inStream >> mLatentSubStyle;
}


// ---------------------------------------------------------------------------
//	¥ ~LCommanderTree									[public, virtual]
// ---------------------------------------------------------------------------
//	Destructor

LCommanderTree::~LCommanderTree()
{
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf								[protected, virtual]
// ---------------------------------------------------------------------------

void
LCommanderTree::FinishCreateSelf()
{
		// Tweak a few settings
	mBaseTraits.color.red	= 0;
	mBaseTraits.color.green	= 0;
	mBaseTraits.color.blue	= 0;

	mBaseTraits.style &= static_cast<SInt16>(~mCurChainStyle);	// Remove the 2 styles we'll be using,
	mBaseTraits.style &= static_cast<SInt16>(~mLatentSubStyle);	// 	just to keep things clean.

	LTree::FinishCreateSelf();
}


// ---------------------------------------------------------------------------
//	¥ BuildTree										[protected, virtual]
// ---------------------------------------------------------------------------
//	Build the text (and style) for the tree. Stored internally in Handles

void
LCommanderTree::BuildTree(
	bool	inGenerateExtraInfo)
{
		// Get our Handles all set up to hold the text
	if (mTreeTextH != nil) {
		::DisposeHandle(mTreeTextH);
		mTreeTextH = nil;
	}

	if (mTreeTextStyleH != nil) {
		::DisposeHandle( reinterpret_cast<Handle>(mTreeTextStyleH) );
		mTreeTextStyleH = nil;
	}

	mTreeTextH = ::NewHandle(0);
	ThrowIfMemFail_(mTreeTextH);
	ValidateHandle_(mTreeTextH);

	mTreeTextStyleH = reinterpret_cast<StScrpHandle>(::NewHandleClear(sizeof(StScrpRec)));
	ThrowIfMemFail_(mTreeTextStyleH);
	ValidateHandle_(mTreeTextStyleH);
	(*mTreeTextStyleH)->scrpNStyles = 0;

	SInt32	theLevel	= 0;
	mLastTop			= 0;

	LCommander*	theTopCommander = LCommander::GetTopCommander();
	if (theTopCommander != nil) {
		WalkCommanderChain(*theTopCommander, theLevel, inGenerateExtraInfo);
	}
}


// ---------------------------------------------------------------------------
//	¥ WalkCommanderChain							[protected, virtual]
// ---------------------------------------------------------------------------
//	Performs the Commander chain iteration to actually draw the Commander

void
LCommanderTree::WalkCommanderChain(
	const LCommander&	inCommander,
	const SInt32&		inLevel,
	bool				inGenerateExtraInfo)
{
	Assert_(mTreeTextH != nil);
	Assert_(mTreeTextStyleH != nil);

		// Get the base text for the level
	LStr255 theText;
	MakeCommanderText(inCommander, theText, inLevel);

		// Append a return character to the end of the text.
		// If the maximum number of characters exist, twiddle the
		// last character to be a return. If this isn't done, a
		// very long descriptor (e.g. from an LTextEditView) can
		// foul up the cosmetics and layout of the Tree window.
	if (theText.Length() < (theText.GetMaxLength() - 1)) {
		theText += char_Return;
	} else {
		theText[theText.Length()] = char_Return;
	}

		// Append the text to our text Handle
	SInt32 runOffset = ::GetHandleSize(mTreeTextH);
	OSErr err = ::PtrAndHand(theText.TextPtr(), mTreeTextH, theText.LongLength());
	ThrowIfOSErr_(err);

		// Construct the style run
	Verify_(mTextPane->FocusDraw());

	(void)UTextTraits::SetPortTextTraits(&mBaseTraits);

		// ScrpStTable holds 1601 elements and we don't want to
		// overflow this boundry. Create unique style runs through
		// run 1600. For the last run stick to the base style and
		// apply this to the rest of the text. Running out of style
		// runs is not a fatal condition, just a limitation of TextEdit.
		//
		// BTW, TextEdit has other limitations (TERec::teLength is a
		// short so you're limited to 32K of text; TERec::lineStarts is
		// an array with 16001 blocks so you can't have more than than many
		// line starts; TERec::viewRect/destRect are Rect's and you can
		// easily blow those short values). We don't enforce those other
		// limitations here -- BE AWARE --. Given the scope of this class
		// it'd doubtful you should hit any of these TextEdit limitation, but
		// if you do you should consider subclassing the Tree classes and
		// reimplementing them using a text engine/class that can support
		// your needs (e.g. WASTE/CWASTEEdit).

	SInt16 runCount = (*mTreeTextStyleH)->scrpNStyles;
	Assert_(runCount >= 0);

	if (runCount <= 1601) {

			// Within the boundries, but only apply changes to
			// the base style for all but the last run
		if (runCount < 1601) {

				// Increase the run count
			(*mTreeTextStyleH)->scrpNStyles += 1;

				// Change the color for the Target
			if (inCommander.IsTarget()) {
				if (UEnvironment::HasFeature(env_SupportsColor)) {
					::RGBForeColor(&mCurTargetColor);
					::RGBBackColor(&Color_White);
				}
			}

				// Change the style for the state
			switch (inCommander.GetOnDutyState()) {
				case triState_Latent:
					::TextFace(static_cast<SInt16>(::GetPortTextFace(GetMacPort()) + mLatentSubStyle));
					break;

				case triState_On:
					::TextFace(static_cast<SInt16>(::GetPortTextFace(GetMacPort()) + mCurChainStyle));
					break;
				
				default:
					break;
			}
		} else { // runCount == 1601
			SignalStringLiteral_("StScrpTable is now full. Subsequent text will use base style information");
		}

			// Add the style run to our list
		FontInfo theFontInfo;
		::GetFontInfo(&theFontInfo);

		((*mTreeTextStyleH)->scrpStyleTab[runCount]).scrpStartChar	= runOffset;
		((*mTreeTextStyleH)->scrpStyleTab[runCount]).scrpHeight		= (SInt16) (theFontInfo.descent + theFontInfo.ascent);
		((*mTreeTextStyleH)->scrpStyleTab[runCount]).scrpAscent		= theFontInfo.ascent;
		((*mTreeTextStyleH)->scrpStyleTab[runCount]).scrpFont		= ::GetPortTextFont(GetMacPort());
		((*mTreeTextStyleH)->scrpStyleTab[runCount]).scrpFace		= ::GetPortTextFace(GetMacPort());
		((*mTreeTextStyleH)->scrpStyleTab[runCount]).scrpSize		= ::GetPortTextSize(GetMacPort());
		::GetForeColor(&((*mTreeTextStyleH)->scrpStyleTab[runCount]).scrpColor);

	} else {

			// You should never reach this point
		SignalStringLiteral_("runCount greater than 1601 -- this should never happen");
	}

		// Just in case the text is going to be wider than the window, we'll
		// expand the image so all fits on one line
	SInt16 strWidth = ::StringWidth(theText);
	SDimension32 imgSize;
	GetImageSize(imgSize);
	if (imgSize.width <= strWidth) {
		mTextPane->ResizeImageBy(strWidth - imgSize.width + 10, 0, Refresh_Yes);
		LTextEditView*	theView = DebugCast_(mTextPane, LView, LTextEditView);
		theView->AlignTextEditRects();
	}

		// Get its subcommanders (if any)
	TArrayIterator<LCommander*> iterator(
								(const_cast<TArray<LCommander*>&>
								((const_cast<LCommander&>
								(inCommander)).GetSubCommanders())));

	LCommander*	theSub;
	while (iterator.Next(theSub) && (theSub != nil)) {
		WalkCommanderChain(*theSub, inLevel + 1, inGenerateExtraInfo);
	}
}



// ---------------------------------------------------------------------------
//	¥ MakeCommanderText							[protected, virtual]
// ---------------------------------------------------------------------------
//	Makes the string containing the text to be used in each "cell".

void
LCommanderTree::MakeCommanderText(
	const LCommander&	inCommander,
	LStr255&			outText,
	SInt32				inLevel)
{
		// Indent
	for (SInt32 i = 1; i <= inLevel; i++) {
		outText += StringLiteral_("  ");
	}

		// Through the magic of RTTI, obtain the name of our object
	outText += typeid(inCommander).name();
	outText += StringLiteral_(" - ");

		// See if the Commander is also an LPane. If so, print its PaneIDT.
	const LPane*	thePane = dynamic_cast<const LPane*>(&inCommander);
	if (thePane != nil) {

		outText += StringLiteral_("ID: ");
		PaneIDT theID = thePane->GetPaneID();

			// PaneIDT's can be numbers or text (FourCharCode). We'll try to
			// display that here. We test versus what should be the minimal
			// possible FourCharCode (4 spaces). If at least that, assume it
			// is text (doubtful someone would use a integral PaneIDT of that
			// size) and display it as such. (FWIW, this really isn't the
			// best way to check if a PaneIDT could be displayed as a FourCharCode,
			// but every other option I've tried has more problems :-(

		if (theID >= 0x20202020) {
				// LString doesn't have an Append(FourCharCode) method,
				// so for ease we'll just create an LStr255(FourCharCode)
				// object and append that since we have Append(LString&)
			outText += char_Apostrophe;
			outText += LStr255(static_cast<FourCharCode>(theID));
			outText += char_Apostrophe;

		} else {
				// Probably just an integral type.
			outText += theID;
		}

		outText += StringLiteral_(" - ");
	}

		// And now set the duty state
	outText += StringLiteral_("Duty: ");

	switch (inCommander.GetOnDutyState()) {
		case triState_Off:
			outText += StringLiteral_("Off");
			break;

		case triState_Latent:
			outText += StringLiteral_("Latent");
			break;

		case triState_On:
			outText += StringLiteral_("On");
			break;
	}
}

PP_End_Namespace_PowerPlant
