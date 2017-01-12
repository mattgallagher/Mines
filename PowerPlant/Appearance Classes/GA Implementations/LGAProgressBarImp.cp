// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAProgressBarImp.cp		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original author: John C. Daub
//
//	This class provides the implementation for a progress bar (determinant
//	or indeterminant) that conforms to the AGA specification.
//
//	In an effort for speed, the indeterminant progress bar's pattern is
//	rendered into a static LGWorld. Actual drawing and animation of the
//	indeterminant progress bar then blits the pattern from the LGWorld
//	into a series of "mini rects" that are just shifted to the right a
//	few pixels each time. This conforms to the AGA spec as well as providing
//	fast, smooth animation.
//
//	To actually animate the indeterminant progress bar, it is implemented
//	as an LPeriodical idler. The Appearance Manager's indeterminant progress
//	bar is animated via calls to ::IdleControls(), so this attempts to
//	replicate the same. If you will not have any sort of direct event
//	loop mechanism in your needs (e.g. event loop, calls UseIdleTime on
//	idleEvents) then you will need to call LPeriodical::DevoteTimeToIdlers
//	somewhere in your loop to allow this to animate.
//
//	The determinant progress bar's cosmetics are reflected by the value
//	of the bar (mMinValue, mMaxValue, mValue). A call to SetValue() will
//	set the bar at whatever amount you wish. If you wish to work on a
//	"percentage done" type implementation, just make the min value 0 and
//	the max value 100. Then calls to SetValue() can just pass the "percentage".


#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LGAProgressBarImp.h>
#include <LStream.h>
#include <UDrawingState.h>
#include <UGWorld.h>

#include <UGAColorRamp.h>
#include <UGraphicUtils.h>

PP_Begin_Namespace_PowerPlant


enum {
	progress_IndeterNumSteps	= 4			// 4 "frames" before we repeat
};

enum {
	progress_Rows				= 10,
	progress_Columns			= 16,
	progress_Height				= 14
};



// color constants

const SInt8		B	=	colorRamp_Black;		//	Index for black
const SInt8		W	=	colorRamp_White;		//	Index for white
const SInt8		A	=	colorRamp_GrayA1;		//	Index for gray A1

// indeterminant color matrix

static SInt8 sIndeterRender[progress_Rows][progress_Columns] =
{
	{	10,	10,	A,	A,	A,	A,	A,	A,	A,	A,	10,	10,	10,	10,	10,	10	},
	{	8,	8,	8,	10,	10,	10,	10,	10,	10,	10,	10,	8,	8,	8,	8,	8	},
	{	5,	5,	5,	5,	8,	8,	8,	8,	8,	8,	8,	8,	5,	5,	5,	5	},
	{	2,	2,	2,	2,	2,	5,	5,	5,	5,	5,	5,	5,	5,	2,	2,	2	},
	{	W,	W,	W,	W,	W,	W,	3,	3,	3,	3,	3,	3,	3,	3,	W,	W	},
	{	2,	2,	2,	2,	2,	2,	2,	5,	5,	5,	5,	5,	5,	5,	5,	2	},
	{	4,	4,	4,	4,	4,	4,	4,	4,	8,	8,	8,	8,	8,	8,	8,	8	},
	{	10,	6,	6,	6,	6,	6,	6,	6,	6,	10,	10,	10,	10,	10,	10,	10	},
	{	A,	A,	8,	8,	8,	8,	8,	8,	8,	8,	A,	A,	A,	A,	A,	A	},
	{	12,	12,	12,	10,	10,	10,	10,	10,	10,	10,	10,	12,	12,	12,	12,	12	}
};


static SInt8 sIndeterDisabledRender[progress_Rows][progress_Columns] =
{
	{	2,	2,	5,	5,	5,	5,	5,	5,	5,	5,	2,	2,	2,	2,	2,	2	},
	{	2,	2,	2,	5,	5,	5,	5,	5,	5,	5,	5,	2,	2,	2,	2,	2	},
	{	2,	2,	2,	2,	5,	5,	5,	5,	5,	5,	5,	5,	2,	2,	2,	2	},
	{	2,	2,	2,	2,	2,	5,	5,	5,	5,	5,	5,	5,	5,	2,	2,	2	},
	{	2,	2,	2,	2,	2,	2,	5,	5,	5,	5,	5,	5,	5,	5,	2,	2	},
	{	2,	2,	2,	2,	2,	2,	2,	5,	5,	5,	5,	5,	5,	5,	5,	2	},
	{	2,	2,	2,	2,	2,	2,	2,	2,	5,	5,	5,	5,	5,	5,	5,	5	},
	{	5,	2,	2,	2,	2,	2,	2,	2,	2,	5,	5,	5,	5,	5,	5,	5	},
	{	5,	5,	2,	2,	2,	2,	2,	2,	2,	2,	5,	5,	5,	5,	5,	5	},
	{	5,	5,	5,	2,	2,	2,	2,	2,	2,	2,	2,	5,	5,	5,	5,	5	}
};


static SInt8 sIndeterRenderBW[progress_Rows][progress_Columns] =
{
	{	W,	W,	B,	B,	B,	B,	B,	B,	B,	B,	W,	W,	W,	W,	W,	W	},
	{	W,	W,	W,	B,	B,	B,	B,	B,	B,	B,	B,	W,	W,	W,	W,	W	},
	{	W,	W,	W,	W,	B,	B,	B,	B,	B,	B,	B,	B,	W,	W,	W,	W	},
	{	W,	W,	W,	W,	W,	B,	B,	B,	B,	B,	B,	B,	B,	W,	W,	W	},
	{	W,	W,	W,	W,	W,	W,	B,	B,	B,	B,	B,	B,	B,	B,	W,	W	},
	{	W,	W,	W,	W,	W,	W,	W,	B,	B,	B,	B,	B,	B,	B,	B,	W	},
	{	W,	W,	W,	W,	W,	W,	W,	W,	B,	B,	B,	B,	B,	B,	B,	B	},
	{	B,	W,	W,	W,	W,	W,	W,	W,	W,	B,	B,	B,	B,	B,	B,	B	},
	{	B,	B,	W,	W,	W,	W,	W,	W,	W,	W,	B,	B,	B,	B,	B,	B	},
	{	B,	B,	B,	W,	W,	W,	W,	W,	W,	W,	W,	B,	B,	B,	B,	B	}
};


static SInt8 sIndeterDisabledRenderBW[progress_Rows][progress_Columns] =
{
	{	W,	W,	B,	W,	B,	W,	B,	W,	B,	W,	W,	W,	W,	W,	W,	W	},
	{	W,	W,	W,	B,	W,	B,	W,	B,	W,	B,	W,	W,	W,	W,	W,	W	},
	{	W,	W,	W,	W,	B,	W,	B,	W,	B,	W,	B,	W,	W,	W,	W,	W	},
	{	W,	W,	W,	W,	W,	B,	W,	B,	W,	B,	W,	B,	W,	W,	W,	W	},
	{	W,	W,	W,	W,	W,	W,	B,	W,	B,	W,	B,	W,	B,	W,	W,	W	},
	{	W,	W,	W,	W,	W,	W,	W,	B,	W,	B,	W,	B,	W,	B,	W,	W	},
	{	W,	W,	W,	W,	W,	W,	W,	W,	B,	W,	B,	W,	B,	W,	B,	W	},
	{	W,	W,	W,	W,	W,	W,	W,	W,	W,	B,	W,	B,	W,	B,	W,	B	},
	{	B,	W,	W,	W,	W,	W,	W,	W,	W,	W,	B,	W,	B,	W,	B,	W	},
	{	W,	B,	W,	W,	W,	W,	W,	W,	W,	W,	W,	B,	W,	B,	W,	B	}
};


LGWorld*	LGAProgressBarImp::sProgBarGWorld = nil;
LGWorld*	LGAProgressBarImp::sDisabledProgBarGWorld = nil;
LGWorld*	LGAProgressBarImp::sProgBarGWorldBW = nil;
LGWorld*	LGAProgressBarImp::sDisabledProgBarGWorldBW = nil;



// ---------------------------------------------------------------------------
//	¥ LGAProgressBarImp									  [public]
// ---------------------------------------------------------------------------
//	Stream Constructor

LGAProgressBarImp::LGAProgressBarImp(
	LStream*	inStream)

	: LGAControlImp(inStream)
{
	mIsIndeterminant = false;
	mCurrentFrame	 = 0;
	mFillWidth		 = 0;
	mLeftBarEdge	 = 0;
	mRightBarEdge	 = 0;
	mRightFillEdge	 = 0;
}


// ---------------------------------------------------------------------------
//	¥ LGAProgressBarImp									 [public]
// ---------------------------------------------------------------------------
//	Parameterized Constructor

LGAProgressBarImp::LGAProgressBarImp(
	LControlPane	*inControlPane)

	: LGAControlImp(inControlPane)
{
	mIsIndeterminant = false;
	mCurrentFrame	 = 0;
	mFillWidth		 = 0;
	mLeftBarEdge	 = 0;
	mRightBarEdge	 = 0;
	mRightFillEdge	 = 0;
}


// ---------------------------------------------------------------------------
//	¥ ~LGAProgressBarImp								[public, virtual]
// ---------------------------------------------------------------------------
//	Destructor

LGAProgressBarImp::~LGAProgressBarImp()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Init												[public, virtual]
// ---------------------------------------------------------------------------
//	Stream Init

void
LGAProgressBarImp::Init(
	LControlPane	*inControlPane,
	LStream			*inStream)
{
	SInt16	controlKind;
	Str255	title;
	ResIDT	textTraitID;

	*inStream >> controlKind;
	*inStream >> textTraitID;
	inStream->ReadPString ( title );

	Init( inControlPane, controlKind );


}


// ---------------------------------------------------------------------------
//	¥ Init												[public, virtual]
// ---------------------------------------------------------------------------
// Parameterized Initializer

void
LGAProgressBarImp::Init(
	LControlPane*	inControlPane,
	SInt16			/* inControlKind */,
	ConstStringPtr	/* inTitle */,
	ResIDT			/* inTextTraitsID */,
	SInt32			/* inRefCon */)
{
	mControlPane = inControlPane;

	AllocateGWorlds();
}


// ---------------------------------------------------------------------------
//	¥ AllocateGWorlds									[public, virtual]
// ---------------------------------------------------------------------------
//	Allocates our GWorlds

void
LGAProgressBarImp::AllocateGWorlds()
{
	// allocate the GWorld. Even if it's a determinant progress bar,
	// we must still create this (as it will not be created otherwise)

	SInt8		colorindex;
	RGBColor	color;

	Rect gRect;
	::MacSetRect( &gRect, 0, 0, progress_Columns, progress_Rows );

	if ( sProgBarGWorld == nil ) {


		try {
			sProgBarGWorld = new LGWorld( gRect );

		}

		catch (...) {
			// probably out of memory. If so, try temporary memory
			sProgBarGWorld = new LGWorld( gRect, 0, useTempMem );
		}

		// draw into it

		sProgBarGWorld->BeginDrawing();

		for (  SInt16 i = 0; i < progress_Rows; ++i ) {
			for (  SInt16 j = 0; j < progress_Columns; ++j ) {
				colorindex = sIndeterRender[i][j];
				color = UGAColorRamp::GetColor(colorindex);
				UGraphicUtils::PaintColorPixel (
									(SInt16) (gRect.left + j),
									(SInt16) (gRect.top + i),
									color );
			}
		}

		sProgBarGWorld->EndDrawing();
	}

	if ( sDisabledProgBarGWorld == nil ) {

		try {
			sDisabledProgBarGWorld = new LGWorld( gRect );
		}

		catch (...) {
			// probably out of memory. If so, try temporary memory
			sDisabledProgBarGWorld = new LGWorld( gRect, 0, useTempMem );
		}

		// draw into it

		sDisabledProgBarGWorld->BeginDrawing();

		for (  SInt16 i = 0; i < progress_Rows; ++i ) {
			for (  SInt16 j = 0; j < progress_Columns; ++j ) {
				colorindex = sIndeterDisabledRender[i][j];
				color = UGAColorRamp::GetColor(colorindex);
				UGraphicUtils::PaintColorPixel (
									(SInt16) (gRect.left + j),
									(SInt16) (gRect.top + i),
									color );
			}
		}

		sDisabledProgBarGWorld->EndDrawing();
	}

	if ( sProgBarGWorldBW == nil ) {

		try {
			sProgBarGWorldBW = new LGWorld( gRect );
		}

		catch (...) {
			// probably out of memory. If so, try temporary memory
			sProgBarGWorldBW = new LGWorld( gRect, 0, useTempMem );
		}

		// draw into it

		sProgBarGWorldBW->BeginDrawing();

		for (  SInt16 i = 0; i < progress_Rows; ++i ) {
			for (  SInt16 j = 0; j < progress_Columns; ++j ) {
				colorindex = sIndeterRenderBW[i][j];
				color = UGAColorRamp::GetColor(colorindex);
				UGraphicUtils::PaintColorPixel (
									(SInt16) (gRect.left + j),
									(SInt16) (gRect.top + i),
									color );
			}
		}

		sProgBarGWorldBW->EndDrawing();
	}

	if ( sDisabledProgBarGWorldBW == nil ) {

		try {
			sDisabledProgBarGWorldBW = new LGWorld( gRect );
		}

		catch (...) {
			// probably out of memory. If so, try temporary memory
			sDisabledProgBarGWorldBW = new LGWorld( gRect, 0, useTempMem );
		}

		// draw into it

		sDisabledProgBarGWorldBW->BeginDrawing();

		for (  SInt16 i = 0; i < progress_Rows; ++i ) {
			for (  SInt16 j = 0; j < progress_Columns; ++j ) {
				colorindex = sIndeterDisabledRenderBW[i][j];
				color = UGAColorRamp::GetColor(colorindex);
				UGraphicUtils::PaintColorPixel (
									(SInt16) (gRect.left + j),
									(SInt16) (gRect.top + i),
									color );
			}
		}

		sDisabledProgBarGWorldBW->EndDrawing();
	}
}

// ---------------------------------------------------------------------------
//	¥ DisposeGWorlds						[static, public]
// ---------------------------------------------------------------------------
//	Disposes of our GWorlds. Not normally called, but available for those
//	that might need it (e.g. working in a code resource).
//
//	BE CAREFUL! No check is made within DrawSelf() to ensure the GWorlds
//	are valid, so only dispose if you are certain no further drawing
//	will occur.

void
LGAProgressBarImp::DisposeGWorlds()
{
	delete sProgBarGWorld;
	sProgBarGWorld = nil;

	delete sDisabledProgBarGWorld;
	sDisabledProgBarGWorld = nil;

	delete sProgBarGWorldBW;
	sProgBarGWorldBW = nil;

	delete sDisabledProgBarGWorldBW;
	sDisabledProgBarGWorldBW = nil;

}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ PostSetValue								[public, virtual]
// ---------------------------------------------------------------------------
//	Really only useful for the determinant progress indicator to set it's
//	progress amount

void
LGAProgressBarImp::PostSetValue()
{
	// redraw to reflect the new value
	mControlPane->Draw(nil);
}


// ---------------------------------------------------------------------------
//	¥ SetMinValue							[public, virtual]
// ---------------------------------------------------------------------------
//	LControlPane actually sets the value, we just need to refresh as a result

void
LGAProgressBarImp::SetMinValue(
	SInt32		/*inMinValue*/)
{
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ SetMaxValue							[public, virtual]
// ---------------------------------------------------------------------------
//	LControlPane actually sets the value, we just need to refresh as a result

void
LGAProgressBarImp::SetMaxValue(
	SInt32		/* inMaxValue */)
{
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ SetDataTag							[public, virtual]
// ---------------------------------------------------------------------------
//	Establish the data associated with this control object.
//
//	For a progress bar, the only use is to set if the bar is determinant or
//	indeterminant.

void
LGAProgressBarImp::SetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
	if (inTag == kControlProgressBarIndeterminateTag) {

		if (mIsIndeterminant) {
			mCurrentFrame = -1;
		}

		mIsIndeterminant = *(Boolean *)inDataPtr;

		if ( mIsIndeterminant ) {
			mCurrentFrame = 1;
		}
	
	} else {
		LGAControlImp::SetDataTag(inPartCode, inTag, inDataSize, inDataPtr);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetDataTag							[public, virtual]
// ---------------------------------------------------------------------------
//	Get the data associated with this control object.
//
//	For a progress bar, the only use is to set if the bar is determinant or
//	indeterminant.

void
LGAProgressBarImp::GetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size*			outDataSize) const
{
	if (inTag == kControlProgressBarIndeterminateTag) {
		if (outDataSize != nil){
			*outDataSize = sizeof(Boolean);
		}

		*(Boolean *)inBuffer = mIsIndeterminant;
		
	} else {
		LGAControlImp::GetDataTag(inPartCode, inTag, inBufferSize,
									inBuffer, outDataSize);
	}
}


// ---------------------------------------------------------------------------
//	¥ Idle															  [public]
// ---------------------------------------------------------------------------
//	Animate the indeterminant progress bar. Does nothing for a determinant bar.

void
LGAProgressBarImp::Idle()
{
	if (mIsIndeterminant) {

		++mCurrentFrame;

		if (mCurrentFrame > progress_IndeterNumSteps) {
			mCurrentFrame = 1;
		}

		mControlPane->Draw(nil);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------
//	Do the cosmetic renderings

void
LGAProgressBarImp::DrawSelf()
{
	Rect frame;
	CalcLocalControlRect(frame);

	DrawBorder(frame);						// Border around bar

	StColorPenState::Normalize();

	StClipRgnState	clip;
	clip.ClipToIntersection(frame);

	if (mIsIndeterminant) {

		GrafPtr				currPort = UQDGlobals::GetCurrentPort();

		SInt16 frameInset = (SInt16) ((mCurrentFrame - 1) * 4);

		Rect	drawRect;
		::MacSetRect(&drawRect, (SInt16) (frame.left - progress_Columns + frameInset),
								frame.top,
								(SInt16) (frame.left + frameInset),
								(SInt16) (frame.top + progress_Rows));

		if (mControlPane->GetBitDepth() < 4) {		// Black & White

			while (drawRect.right < (frame.right + progress_Columns)) {

				if (IsActive() && IsEnabled()) {
					sProgBarGWorldBW->CopyImage(currPort, drawRect);
				} else {
					sDisabledProgBarGWorldBW->CopyImage(currPort, drawRect);
				}

				drawRect.left  = drawRect.right;
				drawRect.right = (SInt16) (drawRect.left + progress_Columns);
			}

		} else {									// Color

			while ( drawRect.right < (frame.right + progress_Columns) ) {

				if (IsActive() && IsEnabled()) {
					sProgBarGWorld->CopyImage(currPort, drawRect);
				} else {
					sDisabledProgBarGWorld->CopyImage(currPort, drawRect);
				}

				drawRect.left  = drawRect.right;
				drawRect.right = (SInt16) (drawRect.left + progress_Columns);
			}
		}

	} else { 	// Determinant

		// Based upon the value (mValue), determine a percentage
		// of how far we are to draw

		SInt32 value	= mControlPane->GetValue();
		SInt32 maxValue = mControlPane->GetMaxValue();
		SInt32 minValue = mControlPane->GetMinValue();

		double percentD = ((double) (value - minValue) /
							 (double) (maxValue - minValue));

		mFillWidth = (SInt16) (percentD * (frame.right - frame.left));

		if (mControlPane->GetBitDepth() < 4) {

			Rect	fillRect;
			::MacSetRect(&fillRect, frame.left, frame.top,
							(SInt16) (frame.left + mFillWidth), frame.bottom);

			if (!IsActive() || !IsEnabled()) {
				StColorPenState::SetGrayPattern();
			}
			::PaintRect(&fillRect);

			fillRect.left  = (SInt16) (fillRect.right + 1);
			fillRect.right = frame.right;
			::EraseRect(&fillRect);

		} else {

			DrawLeftEnd(frame);
			DrawRightEnd(frame);
			DrawFill(frame);
			DrawBackground(frame);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawBorder								[protected, virtual]
// ---------------------------------------------------------------------------
//	Render the frame

void
LGAProgressBarImp::DrawBorder(
	Rect&	ioFrame)
{
	StColorPenState::Normalize();

	RGBColor		tempColor;

	if (mControlPane->GetBitDepth() < 4) {		// Black & White

		tempColor = Color_Black;

		if (!IsActive() || !IsEnabled()) {		// Inactive border is gray
			StColorPenState::SetGrayPattern();
		}

	} else {									// Color

		if (IsActive() && IsEnabled()) {		// Draw shadows

			tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
			::RGBForeColor(&tempColor);
			UGraphicUtils::TopLeftSide( ioFrame, 0, 0, 1, 1 );

			::RGBForeColor(&Color_White);
			UGraphicUtils::BottomRightSide(ioFrame, 1, 1, 0, 0);

			tempColor = Color_Black;			// Active border color

		} else {								// Inactive border color
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
		}
	}

	::RGBForeColor(&tempColor);					// Draw border
	::MacInsetRect(&ioFrame, 1, 1);
	::MacFrameRect(&ioFrame);

	::MacInsetRect(&ioFrame, 1, 1);				// Set rect to interior
}


// ---------------------------------------------------------------------------
//	¥ DrawLeftEnd								[protected, virtual]
// ---------------------------------------------------------------------------
//	Draw the "cap" at the left end of the bar

void
LGAProgressBarImp::DrawLeftEnd(
	const Rect&		inFrame)
{
	if ( !mControlPane->IsActive()  || !mControlPane->IsEnabled() )
		return;

	RGBColor	tempColor;
	mLeftBarEdge = inFrame.left;

	tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
	::RGBForeColor(&tempColor);
	::MoveTo(mLeftBarEdge, (SInt16) (inFrame.bottom - 1));
	::MacLineTo(mLeftBarEdge, inFrame.top);

	mLeftBarEdge += 1;

	UGraphicUtils::PaintColorPixel(
						mLeftBarEdge,
						inFrame.top,
						tempColor );
	UGraphicUtils::PaintColorPixel(
						mLeftBarEdge,
						(SInt16) (inFrame.top + 8),
						tempColor );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
	UGraphicUtils::PaintColorPixel(
						mLeftBarEdge,
						(SInt16) (inFrame.top + 9),
						tempColor );

	tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
	UGraphicUtils::PaintColorPixel(
						mLeftBarEdge,
						(SInt16) (inFrame.top + 1),
						tempColor );
	UGraphicUtils::PaintColorPixel(
						mLeftBarEdge,
						(SInt16) (inFrame.top + 7),
						tempColor );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
	UGraphicUtils::PaintColorPixel(
						mLeftBarEdge,
						(SInt16) (inFrame.top + 2),
						tempColor );
	UGraphicUtils::PaintColorPixel(
						mLeftBarEdge,
						(SInt16) (inFrame.top + 6),
						tempColor );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray1);
	::RGBForeColor(&tempColor);
	::MoveTo(mLeftBarEdge, (SInt16) (inFrame.top + 3) );
	::MacLineTo(mLeftBarEdge, (SInt16) (inFrame.top + 5) );

	mLeftBarEdge += 1;
}


// ---------------------------------------------------------------------------
//	¥ DrawRightEnd								[protected, virtual]
// ---------------------------------------------------------------------------
//	Draw the right "end-cap" on the bar

void
LGAProgressBarImp::DrawRightEnd(
	const Rect&		inFrame)
{
	mRightBarEdge = (SInt16) (inFrame.left + mFillWidth);
	RGBColor		tempColor;

	if ( mControlPane->IsActive() && mControlPane->IsEnabled() ) {

		::RGBForeColor(&Color_Black);
		::MoveTo((SInt16) (inFrame.left + mFillWidth), inFrame.top);
		::MacLineTo((SInt16) (inFrame.left + mFillWidth), (SInt16) (inFrame.top + 9));

		mFillWidth -= 1;

		tempColor = UGAColorRamp::GetColor(colorRamp_Gray12);
		::RGBForeColor(&tempColor);
		::MoveTo((SInt16) (inFrame.left + mFillWidth), inFrame.top);
		::MacLineTo((SInt16) (inFrame.left + mFillWidth), (SInt16) (inFrame.top + 9) );

		tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
		UGraphicUtils::PaintColorPixel((SInt16) (inFrame.left + mFillWidth),
											inFrame.top,
											tempColor);
		mFillWidth -= 1;

		::MoveTo((SInt16) (inFrame.left + mFillWidth), inFrame.top);
		::MacLineTo((SInt16) (inFrame.left + mFillWidth), (SInt16) (inFrame.top + 9) );
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
		UGraphicUtils::PaintColorPixel(
							(SInt16) (inFrame.left + mFillWidth),
							(SInt16) (inFrame.top + 1),
							tempColor);
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray12);
		UGraphicUtils::PaintColorPixel(
							(SInt16) (inFrame.left + mFillWidth),
							(SInt16) (inFrame.top + 9),
							tempColor);
		--mFillWidth;

		UGraphicUtils::PaintColorPixel(
							(SInt16) (inFrame.left + mFillWidth),
							(SInt16) (inFrame.top + 9),
							tempColor);
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
		UGraphicUtils::PaintColorPixel(
							(SInt16) (inFrame.left + mFillWidth),
							(SInt16) (inFrame.top + 8),
							tempColor);
		UGraphicUtils::PaintColorPixel(
							(SInt16) (inFrame.left + mFillWidth),
							inFrame.top,
							tempColor);
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
		UGraphicUtils::PaintColorPixel(
							(SInt16) (inFrame.left + mFillWidth),
							(SInt16) (inFrame.top + 7),
							tempColor);
		UGraphicUtils::PaintColorPixel(
							(SInt16) (inFrame.left + mFillWidth),
							(SInt16) (inFrame.top + 1),
							tempColor);
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
		UGraphicUtils::PaintColorPixel(
							(SInt16) (inFrame.left + mFillWidth),
							(SInt16) (inFrame.top + 6),
							tempColor);
		UGraphicUtils::PaintColorPixel(
							(SInt16) (inFrame.left + mFillWidth),
							(SInt16) (inFrame.top + 2),
							tempColor);

		tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
		::RGBForeColor(&tempColor);
		::MoveTo((SInt16) (inFrame.left + mFillWidth), (SInt16) (inFrame.top + 3));
		::MacLineTo((SInt16) (inFrame.left + mFillWidth), (SInt16) (inFrame.top + 5));

		--mFillWidth;

	} else {

		tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
		::RGBForeColor(&tempColor);
		::MoveTo(mRightBarEdge, inFrame.top);
		::MacLineTo(mRightBarEdge, inFrame.bottom);
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawFill								[protected, virtual]
// ---------------------------------------------------------------------------
//	Fill in the progress bar

void
LGAProgressBarImp::DrawFill(
	const Rect&		inFrame)
{
	mRightFillEdge = (SInt16) (inFrame.left + mFillWidth);
	RGBColor		tempColor;

	if (mControlPane->IsActive() && mControlPane->IsEnabled() ) {
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray1);
		::RGBForeColor(&tempColor);
		::MoveTo(mLeftBarEdge, (SInt16) (inFrame.top + 4));
		::MacLineTo(mRightFillEdge, (SInt16) (inFrame.top + 4));

		tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
		::RGBForeColor(&tempColor);
		::MoveTo(mLeftBarEdge, (SInt16) (inFrame.top + 3));
		::MacLineTo(mRightFillEdge, (SInt16) (inFrame.top + 3));
		::MoveTo(mLeftBarEdge, (SInt16) (inFrame.top + 5));
		::MacLineTo(mRightFillEdge, (SInt16) (inFrame.top + 5));

		tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
		::RGBForeColor(&tempColor);
		::MoveTo(mLeftBarEdge, (SInt16) (inFrame.top + 2));
		::MacLineTo(mRightFillEdge, (SInt16) (inFrame.top + 2));
		::MoveTo(mLeftBarEdge, (SInt16) (inFrame.top + 6));
		::MacLineTo(mRightFillEdge, (SInt16) (inFrame.top + 6));

		tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
		::RGBForeColor(&tempColor);
		::MoveTo(mLeftBarEdge, (SInt16) (inFrame.top + 1));
		::MacLineTo(mRightFillEdge, (SInt16) (inFrame.top + 1));
		::MoveTo(mLeftBarEdge, (SInt16) (inFrame.top + 7));
		::MacLineTo(mRightFillEdge, (SInt16) (inFrame.top + 7));

		tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
		::RGBForeColor(&tempColor);
		::MoveTo(mLeftBarEdge, inFrame.top);
		::MacLineTo(mRightFillEdge, inFrame.top);
		::MoveTo(mLeftBarEdge, (SInt16) (inFrame.top + 8));
		::MacLineTo(mRightFillEdge, (SInt16) (inFrame.top + 8));

		tempColor = UGAColorRamp::GetColor(colorRamp_Gray12);
		::RGBForeColor(&tempColor);
		::MoveTo(mLeftBarEdge, (SInt16) (inFrame.top + 9));
		::MacLineTo(mRightFillEdge, (SInt16) (inFrame.top + 9) );

	} else {									// Inactive Bar

		if (mControlPane->GetBitDepth() < 4) {		// Black & White
			tempColor = Color_Black;
			StColorPenState::SetGrayPattern();

		} else {									// Color
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
		}

		::RGBForeColor(&tempColor);
		Rect fillRect = { inFrame.top, inFrame.left, inFrame.bottom, mRightFillEdge };
		::PaintRect(&fillRect);
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawBackground						[protected, virtual]
// ---------------------------------------------------------------------------
//	Fill the background (the "right/unfinished" portion of the bar)

void
LGAProgressBarImp::DrawBackground(
	const Rect&		inFrame)
{
	RGBColor		tempColor;

	if ( mControlPane->IsActive() && mControlPane->IsEnabled() ) {

		tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
		::RGBForeColor(&tempColor);
		::MoveTo((SInt16) (mRightBarEdge + 1), inFrame.bottom );
		::MacLineTo((SInt16) (mRightBarEdge + 1), inFrame.top );
		::MacLineTo((SInt16) (inFrame.right - 1), inFrame.top );

		if ( (mRightBarEdge + 2) < (inFrame.right -2) ) {
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
			::RGBForeColor(&tempColor);
			::MoveTo((SInt16) (mRightBarEdge + 2), (SInt16) (inFrame.bottom - 1));
			::MacLineTo((SInt16) (inFrame.right - 2), (SInt16) (inFrame.bottom - 1));
		}

		if ( mRightBarEdge < (inFrame.right - 1) ) {
			::MoveTo((SInt16) (inFrame.right - 1), (SInt16) (inFrame.top + 1));
			::MacLineTo((SInt16) (inFrame.right - 1), (SInt16) (inFrame.top + 9));

			tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
			UGraphicUtils::PaintColorPixel(
								(SInt16) (inFrame.right - 1),
								inFrame.top,
								tempColor);
		}
	}

	Rect	fillRect;
	SInt8	rampColor = 3;

	if (mControlPane->IsActive() && mControlPane->IsEnabled()) {

		::MacSetRect(&fillRect, (SInt16) (mRightBarEdge + 2),
								(SInt16) (inFrame.top + 1),
								(SInt16) (inFrame.right - 1),
								(SInt16) (inFrame.bottom - 1) );
		rampColor = 4;

	} else {

		::MacSetRect(&fillRect, (SInt16) (mRightBarEdge + 1),
								inFrame.top,
								inFrame.right,
								inFrame.bottom );
	}

	tempColor = UGAColorRamp::GetColor(rampColor);
	::RGBForeColor(&tempColor);
	::PaintRect( &fillRect );		// fill in the rest
}


// ---------------------------------------------------------------------------
//	¥ CalcLocalControlRect						[protected, virtual]
// ---------------------------------------------------------------------------
//	Figure out a rect (used in drawing) to always keep the control
//	centered vertically in the frame (we'll always go the width of the
//	the frame).

void
LGAProgressBarImp::CalcLocalControlRect(
	Rect &outRect )
{
	CalcLocalFrameRect(outRect);
	SInt16 height = (SInt16) (outRect.bottom - outRect.top);

	outRect.top    = (SInt16) ((outRect.top + (height/2)) - (progress_Height/2));
	outRect.bottom = (SInt16) (outRect.top + progress_Height);
}

PP_End_Namespace_PowerPlant
