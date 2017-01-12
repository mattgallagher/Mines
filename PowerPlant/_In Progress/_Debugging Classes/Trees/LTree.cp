// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTree.cp					PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	An abstract base class for rendering "trees". Actually this was written
//	for the purpose of consolidating common code bwtween LCommanderTree and
//	LPaneTree.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTree.h>
#include <LStream.h>
#include <UMemoryMgr.h>
#include <LString.h>
#include <LTextEditView.h>
#include <PP_DebugMacros.h>
#include <PP_DebugConstants.h>
#include <UDrawingState.h>
#include <UAttachments.h>

#include <Fonts.h>

PP_Begin_Namespace_PowerPlant

const RGBColor	Color_Red		= {0xFFFF, 0x0000, 0x0000};
const RGBColor	Color_DarkGreen	= {0x0000, 0x7777, 0x0000};


// ---------------------------------------------------------------------------
//	¥ LTree										[public]
// ---------------------------------------------------------------------------
//	Default constructor

LTree::LTree()
{
	mLastTime			= ::TickCount();
	mLastTop			= 0;
	mThreshold			= kDefault_Threshold;
	mIsRepeating		= false;
	mTextPane			= nil;
	mTreeTextH			= nil;
	mTreeTextStyleH		= nil;
	mDisplayExtraInfo	= false;

	InitTree(0);
}


// ---------------------------------------------------------------------------
//	¥ LTree										[public]
// ---------------------------------------------------------------------------
//	Copy constructor

LTree::LTree(
	const LTree&	inOriginal)

	: LView(inOriginal)
{
	mLastTime			= ::TickCount();
	mLastTop			= 0;
	mThreshold			= inOriginal.mThreshold;
	mIsRepeating		= inOriginal.mIsRepeating;
	mBaseTraits			= inOriginal.mBaseTraits;
	mDisplayExtraInfo	= inOriginal.mDisplayExtraInfo;

	mTextPane			= nil; // This will be picked up in FinishCreate()

	mTreeTextH	= inOriginal.mTreeTextH;
	OSErr err	= ::HandToHand(&mTreeTextH);
	ThrowIfOSErr_(err);

	mTreeTextStyleH	= inOriginal.mTreeTextStyleH;
	err	= ::HandToHand(reinterpret_cast<Handle*>(&mTreeTextStyleH));
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ LTree										[public]
// ---------------------------------------------------------------------------
//	Parameterized constructor

LTree::LTree(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	UInt32				inThreshold,
	ResIDT				inTxtrID,
	bool				inDisplayExtraInfo)

	: LView(inPaneInfo, inViewInfo)
{
	mLastTime			= ::TickCount();
	mThreshold			= inThreshold;
	mLastTop			= 0;
	mIsRepeating		= false;
	mTextPane			= nil;
	mTreeTextH			= nil;
	mTreeTextStyleH		= nil;
	mDisplayExtraInfo	= inDisplayExtraInfo;

	InitTree(inTxtrID);
}


// ---------------------------------------------------------------------------
//	¥ LTree										[public]
// ---------------------------------------------------------------------------
//	LStream constructor

LTree::LTree(
	LStream*	inStream)

	: LView(inStream)
{
	*inStream >> mThreshold;
	ResIDT txtrID;
	*inStream >> txtrID;

	mLastTime			= ::TickCount();
	mLastTop			= 0;
	mIsRepeating		= false;
	mTextPane			= nil;
	mTreeTextH			= nil;
	mTreeTextStyleH		= nil;
	mDisplayExtraInfo	= false;

	InitTree(txtrID);
}


// ---------------------------------------------------------------------------
//	¥ ~LTree									[public, virtual]
// ---------------------------------------------------------------------------
//	Destructor

LTree::~LTree()
{
	if (mTreeTextH != nil) {
		::DisposeHandle(mTreeTextH);
	}

	if (mTreeTextStyleH != nil) {
		::DisposeHandle(reinterpret_cast<Handle>(mTreeTextStyleH));
	}
}


// ---------------------------------------------------------------------------
//	¥ InitTree									[private]
// ---------------------------------------------------------------------------
//	Private initalizer. Pass zero for the 'Txtr' ResIDT to utilize defaults
//	(hard-coded, helps to aleviate the need for a 'Txtr' resource)

void
LTree::InitTree(
	ResIDT		inTxtrID)
{
	if (inTxtrID != 0) {

		// We'll maintain our own local copy of the information

			// Get the resource
		StResource	theTxtr(ResType_TextTraits, inTxtrID);

			// Detach it since we'll be modifying it slightly
		theTxtr.Detach();

			// Fix it
		StHandleLocker lock(theTxtr);
		UTextTraits::LoadTextTraits(*(reinterpret_cast<TextTraitsH>(theTxtr.Get())));

			// And make our copy
		::BlockMoveData(*theTxtr.Get(), &mBaseTraits, ::GetHandleSize(theTxtr));

			// Tweak a few settings
		mBaseTraits.justification = teFlushLeft;	// It'll look silly any other way

	} else {

			// Defaults
		mBaseTraits.size			= 9;
		mBaseTraits.style			= normal;
		mBaseTraits.justification	= teFlushLeft;
		mBaseTraits.mode			= srcOr;
		mBaseTraits.color.red		= 0;
		mBaseTraits.color.green		= 0;
		mBaseTraits.color.blue		= 0;
		LString::CopyPStr(StringLiteral_("Monaco"), mBaseTraits.fontName);
		::GetFNum(mBaseTraits.fontName, &mBaseTraits.fontNumber);
	}
}


// ---------------------------------------------------------------------------
// ¥ FinishCreateSelf							[protected, virtual]
// ---------------------------------------------------------------------------

void
LTree::FinishCreateSelf()
{
		// Create the text pane that will actually display the tree text.
		// we use a generic LView* to hold this object to allow you to subclass
		// and use your own (i.e. we use LTextEditView by default, but you could
		// subclass and use CWASTEEdit instead).

	SPaneInfo	paneInfo = {	Tree_TextPane,
								mFrameSize.width,
								mFrameSize.height,
								true,
								true,
								{true, true, true, true},
								0,
								0,
								0,
								this };
	SViewInfo	viewInfo = {	mImageSize,
								{0,0},
								{1,1},
								false };

	UInt16		textAttr =	textAttr_MultiStyle;

	mTextPane = new LTextEditView(paneInfo, viewInfo, textAttr, 0);
	mTextPane->FinishCreate();

	mTextPane->AddAttachment(new LColorEraseAttachment(&Color_White, true));

	if (mTreeTextH == nil) {
		mTreeTextH = ::NewHandle(0);
		ThrowIfMemFail_(mTreeTextH);
	}

	if (mTreeTextStyleH == nil) {
		mTreeTextStyleH = reinterpret_cast<StScrpHandle>(::NewHandleClear(sizeof(StScrpRec)));
		ThrowIfMemFail_(mTreeTextStyleH);
	}

	StartRepeating();
}


// ---------------------------------------------------------------------------
//	¥ Update										[public, virtual]
// ---------------------------------------------------------------------------
//	The Tree object itself does not have any cosmetics (note the lack of
//	a Draw/DrawSelf override) -- the cosmetics come from the mTextPane
//	subpane. If you call the Tree's Refresh/Draw, nothing will happen, or
//	at least it will not rebuild the Tree's text (adding subclasses of
//	these methods to rebuild the text causes a great deal of flashing and
//	other cosmetic annoyances).
//
//	If you wish to actually force the Tree to update, you should call
//	this method.

void
LTree::Update()
{
	BuildTree(mDisplayExtraInfo);
	InsertTree();
}


// ---------------------------------------------------------------------------
//	¥ SpendTime										[public, virtual]
// ---------------------------------------------------------------------------
//	Our repeater that forces the window to be redrawn

void
LTree::SpendTime(
	const EventRecord&	/*inMacEvent*/)
{
		// A threshold of zero prevents this
	if ((mThreshold != 0) && ((::TickCount() - mLastTime) > mThreshold)) {
		Update();
		mLastTime = ::TickCount();
	}
}


// ---------------------------------------------------------------------------
//	¥ HideSelf									[protected, virtual]
// ---------------------------------------------------------------------------

void
LTree::HideSelf()
{
	StopRepeating();
}


// ---------------------------------------------------------------------------
//	¥ ShowSelf									[protected, virtual]
// ---------------------------------------------------------------------------

void
LTree::ShowSelf()
{
		// Start repeating and force a draw of the tree contents
		// immediately.
	if (mThreshold != 0) {
		StartRepeating();
	}
	Update();
}


// ---------------------------------------------------------------------------
//	¥ StartRepeating							[public, virtual]
// ---------------------------------------------------------------------------

void
LTree::StartRepeating()
{
	LPeriodical::StartRepeating();

	BroadcastMessage(msg_StartRepeating, this);
	mIsRepeating = true;
}


// ---------------------------------------------------------------------------
//	¥ StopRepeating								[public, virtual]
// ---------------------------------------------------------------------------

void
LTree::StopRepeating()
{
	LPeriodical::StopRepeating();

	BroadcastMessage(msg_StopRepeating, this);

	mIsRepeating = false;
}


// ---------------------------------------------------------------------------
//	¥ InsertTree								[protected, virtual]
// ---------------------------------------------------------------------------
// Inserts our text into the text pane object.

void
LTree::InsertTree()
{
	LTextEditView*	theTextView = DebugCast_(mTextPane, LView, LTextEditView);

	theTextView->SetTextHandle(mTreeTextH, mTreeTextStyleH);
}


// ---------------------------------------------------------------------------
//	¥ BuildTree									[protected, virtual]
// ---------------------------------------------------------------------------

void
LTree::BuildTree(
	bool	/*inGenerateExtraInfo*/)
{
		// Do not assign the value of inGenerateExtraInfo to
		// mDisplayExtraInfo, as this gives the ability to temporarily
		// override the value of mDisplayExtraInfo (e.g. DumpTree())

	SignalStringLiteral_("You must subclass LTree to be of any use");
}


// ---------------------------------------------------------------------------
//	¥ÊDumpTree									[public, virtual]
// ---------------------------------------------------------------------------
//	Dumps the Tree's contents via LDebugStream to the specified
//	EFlushLocation. Due to the level of information generated, it's
//	recommended to flush to a file (or perhaps a console window).

void
LTree::DumpTree(
	EFlushLocation		inFlushLocation,
	ConstStringPtr		inFileName)
{
		// Create the stream
	LDebugStream	theStream(inFlushLocation, false, true);
	theStream.SetFilename(inFileName);

		// Write the text to file
	theStream << mTreeTextH;

	theStream.Flush();
}


// ---------------------------------------------------------------------------
//	¥ SubImageChanged							[public, virtual]
// ---------------------------------------------------------------------------
//	Notification that the Image of some SubView changed size, location,
//	or scroll units.

void
LTree::SubImageChanged(
	LView*		inSubView)
{
		// Because we're "odd" and fake the text pane inside of our
		// pane, we have to keep ourselves in sync.

	SDimension32 subSize;
	inSubView->GetImageSize(subSize);
	ResizeImageTo(subSize.width, subSize.height, Refresh_Yes);

		// And generally speaking, we'll be enclosed within a scroller
		// object. We'd probably pass "this" up the chain, but since again
		// we're "faking" stuff (not to mention we set the "internal"
		// text pane to be the scrolling view in LTreeWindow), we have
		// to let the potential scroller know it changed.
	if (mSuperView != nil) {
		mSuperView->SubImageChanged(inSubView);
	}
}


// ---------------------------------------------------------------------------
//	¥ ScrollImageBy								[public, virtual]
// ---------------------------------------------------------------------------
//	Overridden to keep the tree and text subpane in sync when scrolling

void
LTree::ScrollImageBy(
	SInt32		inLeftDelta,
	SInt32		inTopDelta,
	Boolean		inRefresh)
{
		// Delay drawing until the end to try to minimize flicker and
		// clean up some ugly syncing/drawing while scrolling

	LView::ScrollImageBy(inLeftDelta, inTopDelta, Refresh_No);

	LTextEditView*	theTextView = DebugCast_(mTextPane, LView, LTextEditView);
	theTextView->ScrollImageBy(inLeftDelta, inTopDelta, Refresh_No);

	if (inRefresh) {
		Draw(nil);
	}
}


PP_End_Namespace_PowerPlant
