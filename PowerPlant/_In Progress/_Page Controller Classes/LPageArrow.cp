// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPageArrow.cp				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LPageArrow.h>

#include <LView.h>
#include <LStream.h>
#include <UDrawingState.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>
#include <UQDOperators.h>

PP_Begin_Namespace_PowerPlant


const	SInt16	pageArrow_BottomOffset = 12;

enum {
	arrowRendering_NormalColor	=	0,
	arrowRendering_PushedColor	=	1,
	arrowRendering_DimmedColor	=	2,
	arrowRendering_NormalBW		=	3,
	arrowRendering_PushedBW		=	4,
	arrowRendering_DimmedBW		=	5
};

enum {
	arrowButton_ArrowWidth	=	12,
	arrowButton_ArrowHeight	=	13
};

enum {
	arrow_Choice			=	6,
	arrow_Rows				=	13,
	arrow_Columns			=	12
};

											// COLOR CONSTANTS

const int 	B	=	colorRamp_Black;		//	Index for black
const int	W	=	colorRamp_White;		//	Index for white
const int	N	=	-1;						//	Index for no color which means draw nothing


typedef SInt8 PageArrowRenderingT[arrow_Choice][arrow_Rows][arrow_Columns];

#pragma mark Left Arrow Pixmaps

const PageArrowRenderingT leftArrow_Rendering =		// Left Arrow Pixmap Array
{
	// Normal - color	arrowRendering_NormalColor
	{
		{ N, N, N, N, N, N, N, N, B, B, N, N },		//	0
		{ N, N, N, N, N, N, N, B, 5, 5, B, N },		//	1
		{ N, N, N, N, N, N, B, 5, W, 8, B, N },		//	2
		{ N, N, N, N, N, B, 5, W, 3, 8, B, N },		//	3
		{ N, N, N, N, B, 5, W, 1, 3, 8, B, N },		//	4
		{ N, N, N, B, 5, W, 1, 1, 3, 8, B, N },		//	5
		{ N, N, B, 8, 3, 1, 1, 1, 3, 8, B, N },		//	6
		{ N, N, N, B, 8, 3, 1, 1, 3, 8, B, N },		//	7
		{ N, N, N, N, B, 8, 3, 1, 3, 8, B, N },		//	8
		{ N, N, N, N, N, B, 8, 3, 3, 8, B, N },		//	9
		{ N, N, N, N, N, N, B, 8, 3, 8, B, N },		//	10
		{ N, N, N, N, N, N, N, B, 8, 8, B, N },		//	11
		{ N, N, N, N, N, N, N, N, B, B, N, N }		//	12
	},
	// Pushed - color		arrowRendering_PushedColor
	{
		{ N, N, N, N, N, N, N, N, B, B, N, N },		//	0
		{ N, N, N, N, N, N, N, B, 8, 8, B, N },		//	1
		{ N, N, N, N, N, N, B, 8, 2,10, B, N },		//	2
		{ N, N, N, N, N, B, 8, 2, 7,10, B, N },		//	3
		{ N, N, N, N, B, 8, 2, 5, 7,10, B, N },		//	4
		{ N, N, N, B, 8, 2, 5, 5, 7,10, B, N },		//	5
		{ N, N, B,10, 7, 5, 5, 5, 7,10, B, N },		//	6
		{ N, N, N, B,10, 7, 5, 5, 7,10, B, N },		//	7
		{ N, N, N, N, B,10, 7, 5, 7,10, B, N },		//	8
		{ N, N, N, N, N, B,10, 7, 7,10, B, N },		//	9
		{ N, N, N, N, N, N, B,10, 7,10, B, N },		//	10
		{ N, N, N, N, N, N, N, B,10,10, B, N },		//	11
		{ N, N, N, N, N, N, N, N, B, B, N, N }		//	12
	},
	// Dimmed - color			arrowRendering_DimmedColor
	{
		{ N, N, N, N, N, N, N, N, 7, 7, N, N },		//	0
		{ N, N, N, N, N, N, N, 7, 3, 3, 7, N },		//	1
		{ N, N, N, N, N, N, 7, 3, 3, 3, 7, N },		//	2
		{ N, N, N, N, N, 7, 3, 3, 3, 3, 7, N },		//	3
		{ N, N, N, N, 7, 3, 3, 3, 3, 3, 7, N },		//	4
		{ N, N, N, 7, 3, 3, 3, 3, 3, 3, 7, N },		//	5
		{ N, N, 7, 3, 3, 3, 3, 3, 3, 3, 7, N },		//	6
		{ N, N, N, 7, 3, 3, 3, 3, 3, 3, 7, N },		//	7
		{ N, N, N, N, 7, 3, 3, 3, 3, 3, 7, N },		//	8
		{ N, N, N, N, N, 7, 3, 3, 3, 3, 7, N },		//	9
		{ N, N, N, N, N, N, 7, 3, 3, 3, 7, N },		//	10
		{ N, N, N, N, N, N, N, 7, 3, 3, 7, N },		//	11
		{ N, N, N, N, N, N, N, N, 7, 7, N, N }		//	12
	},
	// Normal - B&W			arrowRendering_NormalBW
	{
		{ N, N, N, N, N, N, N, N, B, B, N, N },		//	0
		{ N, N, N, N, N, N, N, B, W, W, B, N },		//	1
		{ N, N, N, N, N, N, B, W, W, W, B, N },		//	2
		{ N, N, N, N, N, B, W, W, W, W, B, N },		//	3
		{ N, N, N, N, B, W, W, W, W, W, B, N },		//	4
		{ N, N, N, B, W, W, W, W, W, W, B, N },		//	5
		{ N, N, B, W, W, W, W, W, W, W, B, N },		//	6
		{ N, N, N, B, W, W, W, W, W, W, B, N },		//	7
		{ N, N, N, N, B, W, W, W, W, W, B, N },		//	8
		{ N, N, N, N, N, B, W, W, W, W, B, N },		//	9
		{ N, N, N, N, N, N, B, W, W, W, B, N },		//	10
		{ N, N, N, N, N, N, N, B, W, W, B, N },		//	11
		{ N, N, N, N, N, N, N, N, B, B, N, N }		//	12
	},
	//	Pushed - B&W			arrowRendering_PushedBW
	{
		{ N, N, N, N, N, N, N, N, B, B, N, N },		//	0
		{ N, N, N, N, N, N, N, B, B, B, B, N },		//	1
		{ N, N, N, N, N, N, B, B, B, B, B, N },		//	2
		{ N, N, N, N, N, B, B, B, B, B, B, N },		//	3
		{ N, N, N, N, B, B, B, B, B, B, B, N },		//	4
		{ N, N, N, B, B, B, B, B, B, B, B, N },		//	5
		{ N, N, B, B, B, B, B, B, B, B, B, N },		//	6
		{ N, N, N, B, B, B, B, B, B, B, B, N },		//	7
		{ N, N, N, N, B, B, B, B, B, B, B, N },		//	8
		{ N, N, N, N, N, B, B, B, B, B, B, N },		//	9
		{ N, N, N, N, N, N, B, B, B, B, B, N },		//	10
		{ N, N, N, N, N, N, N, B, B, B, B, N },		//	11
		{ N, N, N, N, N, N, N, N, B, B, N, N }		//	12
	},
	//	Dimmed - B&W			arrowRendering_DimmeddBW
	{
		{ N, N, N, N, N, N, N, N, N, B, N, N },		//	0
		{ N, N, N, N, N, N, N, N, B, N, B, N },		//	1
		{ N, N, N, N, N, N, N, B, N, B, N, N },		//	2
		{ N, N, N, N, N, N, B, N, B, N, B, N },		//	3
		{ N, N, N, N, N, B, N, B, N, B, N, N },		//	4
		{ N, N, N, N, B, N, B, N, B, N, B, N },		//	5
		{ N, N, N, B, N, B, N, B, N, B, N, N },		//	6
		{ N, N, N, N, B, N, B, N, B, N, B, N },		//	7
		{ N, N, N, N, N, B, N, B, N, B, N, N },		//	8
		{ N, N, N, N, N, N, B, N, B, N, B, N },		//	9
		{ N, N, N, N, N, N, N, B, N, B, N, N },		//	10
		{ N, N, N, N, N, N, N, N, B, N, B, N },		//	11
		{ N, N, N, N, N, N, N, N, N, B, N, N }		//	12
	}
};

#pragma mark Right Arrow Pixmaps

const PageArrowRenderingT rightArrow_Rendering =	// Right Arrow Pixmap Array
{
	// Normal - color		arrowRendering_NormalColor
	{
		{ N, N, N, B, B, N, N, N, N, N, N, N },		//	0
		{ N, N, B, 5, 5, B, N, N, N, N, N, N },		//	1
		{ N, N, B, 5, W, 5, B, N, N, N, N, N },		//	2
		{ N, N, B, 5, W, W, 5, B, N, N, N, N },		//	3
		{ N, N, B, 5, W, 1, W, 5, B, N, N, N },		//	4
		{ N, N, B, 5, W, 1, 1, 1, 5, B, N, N },		//	5
		{ N, N, B, 5, W, 1, 1, 1, 5, 8, B, N },		//	6
		{ N, N, B, 5, W, 1, 1, 5, 8, B, N, N },		//	7
		{ N, N, B, 5, 1, 1, 5, 8, B, N, N, N },		//	8
		{ N, N, B, 5, 1, 5, 8, B, N, N, N, N },		//	9
		{ N, N, B, 5, 5, 8, B, N, N, N, N, N },		//	10
		{ N, N, B, 8, 8, B, N, N, N, N, N, N },		//	11
		{ N, N, N, B, B, N, N, N, N, N, N, N }		//	12
	},
	// Pushed - color		arrowRendering_PushedColor
	{
		{ N, N, N, B, B, N, N, N, N, N, N, N },		//	0
		{ N, N, B, 8, 8, B, N, N, N, N, N, N },		//	1
		{ N, N, B, 8, 3, 8, B, N, N, N, N, N },		//	2
		{ N, N, B, 8, 3, 3, 8, B, N, N, N, N },		//	3
		{ N, N, B, 8, 3, 5, 3, 8, B, N, N, N },		//	4
		{ N, N, B, 8, 3, 5, 5, 5, 8, B, N, N },		//	5
		{ N, N, B, 8, 3, 5, 5, 5, 8,10, B, N },		//	6
		{ N, N, B, 8, 3, 5, 5, 8,10, B, N, N },		//	7
		{ N, N, B, 8, 5, 5, 8,10, B, N, N, N },		//	8
		{ N, N, B, 8, 5, 8,10, B, N, N, N, N },		//	9
		{ N, N, B, 8, 8,10, B, N, N, N, N, N },		//	10
		{ N, N, B,10,10, B, N, N, N, N, N, N },		//	11
		{ N, N, N, B, B, N, N, N, N, N, N, N }		//	12
	},
	// Dimmed - color		arrowRendering_DimmedColor
	{
		{ N, N, N, 7, 7, N, N, N, N, N, N, N },		//	0
		{ N, N, 7, 3, 3, 7, N, N, N, N, N, N },		//	1
		{ N, N, 7, 3, 3, 3, 7, N, N, N, N, N },		//	2
		{ N, N, 7, 3, 3, 3, 3, 7, N, N, N, N },		//	3
		{ N, N, 7, 3, 3, 3, 3, 3, 7, N, N, N },		//	4
		{ N, N, 7, 3, 3, 3, 3, 3, 3, 7, N, N },		//	5
		{ N, N, 7, 3, 3, 3, 3, 3, 3, 3, 7, N },		//	6
		{ N, N, 7, 3, 3, 3, 3, 3, 3, 7, N, N },		//	7
		{ N, N, 7, 3, 3, 3, 3, 3, 7, N, N, N },		//	8
		{ N, N, 7, 3, 3, 3, 3, 7, N, N, N, N },		//	9
		{ N, N, 7, 3, 3, 3, 7, N, N, N, N, N },		//	10
		{ N, N, 7, 3, 3, 7, N, N, N, N, N, N },		//	11
		{ N, N, N, 7, 7, N, N, N, N, N, N, N }		//	12
	},
	// Normal - B&W			arrowRendering_NormalBW
	{
		{ N, N, N, B, B, N, N, N, N, N, N, N },		//	0
		{ N, N, B, W, W, B, N, N, N, N, N, N },		//	1
		{ N, N, B, W, W, W, B, N, N, N, N, N },		//	2
		{ N, N, B, W, W, W, W, B, N, N, N, N },		//	3
		{ N, N, B, W, W, W, W, W, B, N, N, N },		//	4
		{ N, N, B, W, W, W, W, W, W, B, N, N },		//	5
		{ N, N, B, W, W, W, W, W, W, W, B, N },		//	6
		{ N, N, B, W, W, W, W, W, W, B, N, N },		//	7
		{ N, N, B, W, W, W, W, W, B, N, N, N },		//	8
		{ N, N, B, W, W, W, W, B, N, N, N, N },		//	9
		{ N, N, B, W, W, W, B, N, N, N, N, N },		//	10
		{ N, N, B, W, W, B, N, N, N, N, N, N },		//	11
		{ N, N, N, B, B, N, N, N, N, N, N, N }		//	12
	},
	//	Pushed - B&W		arrowRendering_PushedBW
	{
		{ N, N, N, B, B, N, N, N, N, N, N, N },		//	0
		{ N, N, B, B, B, B, N, N, N, N, N, N },		//	1
		{ N, N, B, B, B, B, B, N, N, N, N, N },		//	2
		{ N, N, B, B, B, B, B, B, N, N, N, N },		//	3
		{ N, N, B, B, B, B, B, B, B, N, N, N },		//	4
		{ N, N, B, B, B, B, B, B, B, B, N, N },		//	5
		{ N, N, B, B, B, B, B, B, B, B, B, N },		//	6
		{ N, N, B, B, B, B, B, B, B, B, N, N },		//	7
		{ N, N, B, B, B, B, B, B, B, N, N, N },		//	8
		{ N, N, B, B, B, B, B, B, N, N, N, N },		//	9
		{ N, N, B, B, B, B, B, N, N, N, N, N },		//	10
		{ N, N, B, B, B, B, N, N, N, N, N, N },		//	11
		{ N, N, N, B, B, N, N, N, N, N, N, N }		//	12
	},
	//	Dimmed - B&W		arrowRendering_DimmedBW
	{
		{ N, N, N, B, N, N, N, N, N, N, N, N },		//	0
		{ N, N, B, N, B, N, N, N, N, N, N, N },		//	1
		{ N, N, N, B, N, B, N, N, N, N, N, N },		//	2
		{ N, N, B, N, B, N, B, N, N, N, N, N },		//	3
		{ N, N, N, B, N, B, N, B, N, N, N, N },		//	4
		{ N, N, B, N, B, N, B, N, B, N, N, N },		//	5
		{ N, N, N, B, N, B, N, B, N, B, N, N },		//	6
		{ N, N, B, N, B, N, B, N, B, N, N, N },		//	7
		{ N, N, N, B, N, B, N, B, N, N, N, N },		//	8
		{ N, N, B, N, B, N, B, N, N, N, N, N },		//	9
		{ N, N, N, B, N, B, N, N, N, N, N, N },		//	10
		{ N, N, B, N, B, N, N, N, N, N, N, N },		//	11
		{ N, N, N, B, N, N, N, N, N, N, N, N }		//	12
	}
};

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LPageArrow							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LPageArrow::LPageArrow()
{
	mPushed    = false;
	mFacesLeft = false;
	mBackColor = pageButton_DefaultBackColor;
	mFaceColor = pageButton_DefaultFaceColor;
}


// ---------------------------------------------------------------------------
//	¥ LPageArrow							Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LPageArrow::LPageArrow(
	const LPageArrow&	inOriginal)

	: LControl(inOriginal)
{
	mPushed    = inOriginal.mPushed;
	mFacesLeft = inOriginal.mFacesLeft;
	mBackColor = inOriginal.mBackColor;
	mFaceColor = inOriginal.mFaceColor;
}


// ---------------------------------------------------------------------------
//	¥ LPageArrow							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LPageArrow::LPageArrow(
	const SPaneInfo&	inPaneInfo,
	const SControlInfo&	inControlInfo,
	bool				inFacesLeft,
	const RGBColor&		inBackColor,
	const RGBColor&		inFaceColor)

	: LControl(inPaneInfo, inControlInfo.valueMessage,
						   inControlInfo.value,
						   inControlInfo.minValue,
						   inControlInfo.maxValue)
{
	mPushed    = false;
	mFacesLeft = inFacesLeft;
	mBackColor = inBackColor;
	mFaceColor = inFaceColor;
}


// ---------------------------------------------------------------------------
//	¥ ~LPageArrow							Destructor				  [public]
// ---------------------------------------------------------------------------

LPageArrow::~LPageArrow()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetPushedState												  [public]
// ---------------------------------------------------------------------------

void
LPageArrow::SetPushedState(
	Boolean	inPushedState)
{
	if (mPushed != inPushedState) {
		mPushed = inPushedState;
		Draw(nil);
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcLocalArrowRect											  [public]
// ---------------------------------------------------------------------------

void
LPageArrow::CalcLocalArrowRect(
	Rect&		outRect) const
{

	CalcLocalFrameRect(outRect);
	SInt16		frameHeight = UGraphicUtils::RectHeight(outRect);

							// Adjust the rect such that it is always centered
							// vertically within the arrows frame rect
	outRect.left  += 4;
	outRect.right  = (SInt16) (outRect.left + arrowButton_ArrowWidth);
	outRect.top   += (SInt16) ((frameHeight - (arrowButton_ArrowHeight + pageArrow_BottomOffset)) / 2);
	outRect.bottom = (SInt16) (outRect.top + arrowButton_ArrowHeight);

}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ EnableSelf													  [public]
// ---------------------------------------------------------------------------

void
LPageArrow::EnableSelf()
{
	Draw(nil);
}


// ---------------------------------------------------------------------------
//	¥ DisableSelf													  [public]
// ---------------------------------------------------------------------------

void
LPageArrow::DisableSelf()
{
	Draw(nil);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ActivateSelf													  [public]
// ---------------------------------------------------------------------------

void
LPageArrow::ActivateSelf()
{
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf												  [public]
// ---------------------------------------------------------------------------

void
LPageArrow::DeactivateSelf()
{
	Refresh();
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LPageArrow::DrawSelf()
{
	StColorPenState::Normalize();

	if (IsPushed() && IsEnabled()) {
		DrawPageArrowPushed();

	} else if ( !IsEnabled() || !IsActive() ) {
		DrawPageArrowDimmed();

	} else {
		DrawPageArrowNormal();
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawPageArrowNormal										   [protected]
// ---------------------------------------------------------------------------

void
LPageArrow::DrawPageArrowNormal()
{
	Rect	arrowRect;
	CalcLocalArrowRect(arrowRect);

	SInt16	bitDepth;
	bool	hasColor;
	
	GetDeviceInfo(bitDepth, hasColor);

	if (bitDepth < 4) {
		DrawPageArrowNormalBW();
		DrawArrowChoice(arrowRendering_NormalBW, arrowRect);

	} else {
		DrawPageArrowNormalColor();
		DrawArrowChoice ( arrowRendering_NormalColor, arrowRect);
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawPageArrowPushed										   [protected]
// ---------------------------------------------------------------------------

void
LPageArrow::DrawPageArrowPushed()
{
	Rect	arrowRect;
	CalcLocalArrowRect(arrowRect);

	SInt16	bitDepth;
	bool	hasColor;
	
	GetDeviceInfo(bitDepth, hasColor);

	if (bitDepth < 4) {
		DrawArrowChoice(arrowRendering_PushedBW, arrowRect);

	} else {
		DrawArrowChoice(arrowRendering_PushedColor, arrowRect);
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawPageArrowDimmed										   [protected]
// ---------------------------------------------------------------------------

void
LPageArrow::DrawPageArrowDimmed()
{
	Rect	arrowRect;
	CalcLocalArrowRect(arrowRect);

	SInt16	bitDepth;
	bool	hasColor;
	
	GetDeviceInfo(bitDepth, hasColor);

	if (bitDepth < 4) {
		DrawPageArrowNormalBW();
		DrawArrowChoice(arrowRendering_DimmedBW, arrowRect);

	} else {
		LView*	super = GetSuperView ();
		if (!super->IsEnabled() || !IsActive()) {
			DrawPageArrowDimmedColor();

		} else {
			DrawPageArrowNormalColor();
		}

		DrawArrowChoice(arrowRendering_DimmedColor, arrowRect);
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawPageArrowNormalBW										   [protected]
// ---------------------------------------------------------------------------

void
LPageArrow::DrawPageArrowNormalBW()
{
	StColorPenState::Normalize();

	Rect localFrame;
	CalcLocalFrameRect ( localFrame );
												// Erase the area occupied by the button
	::RGBBackColor ( &Color_White );
	::EraseRect ( &localFrame );
												// Frame the button in black
	::RGBForeColor ( &Color_Black );
	localFrame.bottom -= pageArrow_BottomOffset;
	::MacFrameRect ( &localFrame );
	localFrame.bottom += pageArrow_BottomOffset;

												// Now draw the shadow along the bottom of the
												// button
	::MoveTo (		localFrame.left,	(SInt16) (localFrame.bottom - 12) );
	::MacLineTo (	localFrame.right, 	(SInt16) (localFrame.bottom - 12) );
	::MoveTo (		localFrame.left,	(SInt16) (localFrame.bottom - 11) );
	::MacLineTo (	localFrame.right,	(SInt16) (localFrame.bottom - 11) );
}


// ---------------------------------------------------------------------------
//	¥ DrawPageArrowNormalColor									   [protected]
// ---------------------------------------------------------------------------

void
LPageArrow::DrawPageArrowNormalColor()
{
	StColorPenState::Normalize();

	RGBColor	tempColor;

	Rect localFrame;
	CalcLocalFrameRect ( localFrame );

												// FACE
												// First make sure the face of the control is drawn
	::RGBForeColor ( &mFaceColor );
	localFrame.bottom -= pageArrow_BottomOffset;
	::MacInsetRect ( &localFrame, 1, 1 );
	::PaintRect ( &localFrame );
	::MacInsetRect ( &localFrame, -1, -1 );
	localFrame.bottom += pageArrow_BottomOffset;

												// BACKGROUND

	if (mBackColor == Color_White) {			// Interpret white to mean transparent
		mSuperView->ApplyForeAndBackColors();

	} else {
		Pattern	whitePat;
		::BackPat( UQDGlobals::GetWhitePat(&whitePat) );
		::RGBBackColor(&mBackColor);
	}

	SInt16	oldTop = localFrame.top;
	localFrame.top += (SInt16) (UGraphicUtils::RectHeight ( localFrame ) - pageArrow_BottomOffset);
	::EraseRect ( &localFrame );
	localFrame.top = oldTop;
												// FRAME
												// Frame the control in black
	tempColor = UGAColorRamp::GetBlackColor();
	::RGBForeColor ( &tempColor );
	UGraphicUtils::TopLeftSide ( localFrame, 0, 0, pageArrow_BottomOffset, 0 );
	UGraphicUtils::BottomRightSide ( localFrame, 1, 1, pageArrow_BottomOffset, 0 );

												// SHADOWS
												// Render the left top edge
	tempColor = UGraphicUtils::Lighten ( mFaceColor );
	::RGBForeColor ( &tempColor );
	UGraphicUtils::TopLeftSide ( 	localFrame,
													1,
													1,
													pageArrow_BottomOffset + 2,
													2 );

  		 										// Render the bottom right edge
	tempColor = UGraphicUtils::Darken ( mFaceColor );
	::RGBForeColor ( &tempColor );
	UGraphicUtils::BottomRightSide ( localFrame, 2, 2, 13, 1 );

												// DROP SHADOW
												// Now draw the shadow along the bottom of the
												// button
	UGAColorRamp::GetColor(colorRamp_Gray8, tempColor);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 12) );
	::MacLineTo ( localFrame.right, (SInt16) (localFrame.bottom - 12) );

	UGAColorRamp::GetColor(colorRamp_Gray6, tempColor);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 11) );
	::MacLineTo ( localFrame.right, (SInt16) (localFrame.bottom - 11) );

	UGAColorRamp::GetColor(colorRamp_Gray4, tempColor);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 10) );
	::MacLineTo ( localFrame.right, (SInt16) (localFrame.bottom - 10) );

}


// ---------------------------------------------------------------------------
//	¥ DrawPageArrowDimmedColor									   [protected]
// ---------------------------------------------------------------------------

void
LPageArrow::DrawPageArrowDimmedColor()
{
	StColorPenState::Normalize();

	RGBColor	tempColor;

	Rect localFrame;
	CalcLocalFrameRect ( localFrame );

												// First make sure the face of the control is drawn
	tempColor = UGraphicUtils::Lighten ( mFaceColor );
	::RGBForeColor ( &tempColor );
	localFrame.bottom -= pageArrow_BottomOffset;
	::MacInsetRect ( &localFrame, 1, 1 );
	::PaintRect ( &localFrame );
	::MacInsetRect ( &localFrame, -1, -1 );
	localFrame.bottom += pageArrow_BottomOffset;

												// Now we make sure we draw the background color
												// so that the controller can stand on its own
												// without needing a view to paint the background
												// BACKGROUND

	if (mBackColor == Color_White) {			// Interpret white to mean transparent
		mSuperView->ApplyForeAndBackColors();

	} else {
		Pattern	whitePat;
		::BackPat( UQDGlobals::GetWhitePat(&whitePat) );
		::RGBBackColor(&mBackColor);
	}

	SInt16	oldTop = localFrame.top;
	localFrame.top += (SInt16) (UGraphicUtils::RectHeight ( localFrame ) - pageArrow_BottomOffset);
	::EraseRect ( &localFrame );
	localFrame.top = oldTop;
												// Draw a black line along the left, top and right
												// edges
	UGAColorRamp::GetColor(colorRamp_Gray7, tempColor);
	::RGBForeColor ( &tempColor );
	UGraphicUtils::TopLeftSide ( localFrame, 0, 0, pageArrow_BottomOffset, 0 );
	UGraphicUtils::BottomRightSide ( localFrame, 1, 1, pageArrow_BottomOffset, 0 );

												// Now draw the shadow along the bottom of the
												// button
	UGAColorRamp::GetColor(colorRamp_Gray5, tempColor);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 12) );
	::MacLineTo ( localFrame.right, (SInt16) (localFrame.bottom - 12) );

	UGAColorRamp::GetColor(colorRamp_Gray4, tempColor);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 11) );
	::MacLineTo ( localFrame.right, (SInt16) (localFrame.bottom - 11) );

	UGAColorRamp::GetColor(colorRamp_Gray3, tempColor);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 10) );
	::MacLineTo ( localFrame.right, (SInt16) (localFrame.bottom - 10) );

}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ PointIsInFrame											   [protected]
// ---------------------------------------------------------------------------

Boolean
LPageArrow::PointIsInFrame(
	SInt32		inHorizPort,
	SInt32		inVertPort) const
{
							// We are only going to handle points in the
							// actual content area of the arrow button
	Rect localFrame;
	CalcLocalFrameRect ( localFrame );
	localFrame.bottom -= pageArrow_BottomOffset;
	Point			portPt = { (SInt16) inVertPort, (SInt16) inHorizPort };
	PortToLocalPoint ( portPt );

	return ::MacPtInRect ( portPt, &localFrame );

}


// ---------------------------------------------------------------------------
//	¥ HotSpotAction												   [protected]
// ---------------------------------------------------------------------------

void
LPageArrow::HotSpotAction(
	SInt16		/* inHotSpot */,
	Boolean		inCurrInside,
	Boolean		inPrevInside)
{
							// If the mouse moved in or out of the hot spot
							// handle the hiliting of the control
	if (inCurrInside != inPrevInside) {
							// For a straight button all we have to worry
							// about is switching the hiliting of the button
		SetPushedState(inCurrInside);
	}
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult												   [protected]
// ---------------------------------------------------------------------------

void
LPageArrow::HotSpotResult(
	SInt16 /* inHotSpot */)
{
						// Do the right thing about getting the value set
						// after a button has been clicked in its hotspot,
						// NOTE: this method assumes the entire control is
						// the hotspot
	BroadcastValueMessage();

}


// ---------------------------------------------------------------------------
//	¥ DrawArrowChoice												 [private]
// ---------------------------------------------------------------------------

void
LPageArrow::DrawArrowChoice(
	SInt16			inChoice,
	const Rect&		inFrame)
{
						// Iterate over our rendering array drawing the
						// individual pixels as specified in the rendering
						// array

	const PageArrowRenderingT* arrowRendering = &rightArrow_Rendering;

	if (mFacesLeft) {
		arrowRendering = &leftArrow_Rendering;
	}

	for (int i = 0; i < arrow_Rows; i++) {

		for (int j = 0; j < arrow_Columns; j++) {

			SInt8 index = (*arrowRendering)[inChoice][i][j];
			if (index != N) {
				RGBColor color;
				UGAColorRamp::GetColor(index, color);

				UGraphicUtils::PaintColorPixel((SInt16) (inFrame.left + j),
											   (SInt16) (inFrame.top + i),
											   color);
			}
		}
	}
}


PP_End_Namespace_PowerPlant
