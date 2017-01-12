// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAChasingArrowsImp.cp		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	This class provides the implementation for chasing arrows that
//	conforms to the Appearance specification. There is no AGA specification
//	for this widget.
//
//	The Appearance spec allows animation to occur by OR-ing in the
//	kControlWantsIdle bit into it's feature flags. However, there is no such
//	mechansim (yet) in LControlPane/LControlImp. If you need to force an
//	idle, then call LPeriodical::DevoteTimeToIdlers().

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGAChasingArrowsImp.h>
#include <LStream.h>
#include <LControlView.h>
#include <UDrawingState.h>
#include <UGWorld.h>

#include <UGAColorRamp.h>
#include <UGraphicUtils.h>

PP_Begin_Namespace_PowerPlant


enum {
	chaseArrows_NumFrames	= 8			// 8 "frames" before we repeat
};

enum {
	chaseArrows_Rows				= 16,	// chasing arrows are in a 16x16 area
	chaseArrows_Columns				= 16,
	chaseArrows_Width				= chaseArrows_Columns,
	chaseArrows_Height				= chaseArrows_Rows
};


// color constants

const SInt8 b	=	colorRamp_Black;	// Index for black
const SInt8 N	=	-1;					// No color


// frame matrix

SInt8	sArrowMatrix[chaseArrows_NumFrames][chaseArrows_Rows][chaseArrows_Columns] =
{
	// frame 1
	{
		{ N, N, N, N, N, N, N, b, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, b, b, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, b, b, b, b, N, N, N, N, N, N },
		{ N, N, N, N, b, b, N, b, b, N, N, N, N, N, N, N },
		{ N, N, N, b, N, N, N, b, N, N, N, N, N, N, N, N },
		{ N, N, N, b, N, N, N, N, N, N, N, N, b, N, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, N, b, N, N, N, N, N, N, N, N, b, N, N, N },
		{ N, N, N, N, N, N, N, N, b, N, N, N, b, N, N, N },
		{ N, N, N, N, N, N, N, b, b, N, b, b, N, N, N, N },
		{ N, N, N, N, N, N, b, b, b, b, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, b, b, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, b, N, N, N, N, N, N, N }
	},

	// frame 2
	{
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, b, N, N, N, N, N, N },
		{ N, N, N, N, N, N, b, b, b, b, b, N, N, N, N, N },
		{ N, N, N, N, b, b, N, N, b, b, b, b, N, N, N, N },
		{ N, N, N, b, N, N, N, N, b, N, N, N, N, N, N, N },
		{ N, N, N, b, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, b, N, N, N },
		{ N, N, N, N, N, N, N, b, N, N, N, N, b, N, N, N },
		{ N, N, N, N, b, b, b, b, N, N, b, b, N, N, N, N },
		{ N, N, N, N, N, b, b, b, b, b, N, N, N, N, N, N },
		{ N, N, N, N, N, N, b, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N }
	},

	// frame 3
	{
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, b, b, b, b, N, N, b, N, N, N },
		{ N, N, N, N, b, b, N, N, N, N, b, b, b, N, N, N },
		{ N, N, N, b, N, N, N, N, N, N, b, b, b, N, N, N },
		{ N, N, N, b, N, N, N, N, N, N, N, N, b, N, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, N, b, N, N, N, N, N, N, N, N, b, N, N, N },
		{ N, N, N, b, b, b, N, N, N, N, N, N, b, N, N, N },
		{ N, N, N, b, b, b, N, N, N, N, b, b, N, N, N, N },
		{ N, N, N, b, N, N, b, b, b, b, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N }
	},

	// frame 4
	{
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, b, b, b, b, N, N, N, N, N, N },
		{ N, N, N, N, b, b, N, N, N, N, b, b, N, N, N, N },
		{ N, N, N, b, N, N, N, N, N, N, N, N, b, N, b, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, b, b, b, N },
		{ N, N, N, N, N, N, N, N, N, N, N, b, b, b, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, b, b, b, N, N, N, N, N, N, N, N, N, N, N },
		{ N, b, b, b, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, b, N, b, N, N, N, N, N, N, N, N, b, N, N, N },
		{ N, N, N, N, b, b, N, N, N, N, b, b, N, N, N, N },
		{ N, N, N, N, N, N, b, b, b, b, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N }
	},

	// frame 5
	{
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, b, b, b, b, N, N, N, N, N, N },
		{ N, N, N, N, N, b, N, N, N, N, b, b, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, b, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, b, N, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, b, b, b, N, N, N, N, N, N, N, b, b, b, b, b },
		{ b, b, b, b, b, N, N, N, N, N, N, N, b, b, b, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, N, b, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, b, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, b, b, N, N, N, N, b, N, N, N, N, N },
		{ N, N, N, N, N, N, b, b, b, b, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N }
	},

	// frame 6
	{
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, b, b, b, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, b, b, N, N, N, N },
		{ N, N, N, b, N, N, N, N, N, N, N, N, b, N, N, N },
		{ N, N, b, b, N, N, N, N, N, N, N, N, b, N, N, N },
		{ N, b, b, b, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, b, b, b, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, b, b, b, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, b, b, b, N },
		{ N, N, N, b, N, N, N, N, N, N, N, N, b, b, N, N },
		{ N, N, N, b, N, N, N, N, N, N, N, N, b, N, N, N },
		{ N, N, N, N, b, b, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, b, b, b, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N }
	},

	// frame 7
	{
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, b, N, N, N, N, N, N },
		{ N, N, b, b, b, b, N, N, N, N, b, b, N, N, N, N },
		{ N, N, N, b, b, N, N, N, N, N, N, N, b, N, N, N },
		{ N, N, N, b, b, N, N, N, N, N, N, N, b, N, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, N, b, N, N, N, N, N, N, N, b, b, N, N, N },
		{ N, N, N, b, N, N, N, N, N, N, N, b, b, N, N, N },
		{ N, N, N, N, b, b, N, N, N, N, b, b, b, b, N, N },
		{ N, N, N, N, N, N, b, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N }
	},

	// frame 8
	{
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, b, b, N, N, N, N, N, N, N, N, N, N },
		{ N, N, N, N, N, b, b, b, N, N, N, N, N, N, N, N },
		{ N, N, N, N, b, b, b, N, N, N, N, b, N, N, N, N },
		{ N, N, N, b, N, N, b, N, N, N, N, N, b, N, N, N },
		{ N, N, N, b, N, N, N, N, N, N, N, N, b, N, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, b, N, N, N, N, N, N, N, N, N, N, b, N, N },
		{ N, N, N, b, N, N, N, N, N, N, N, N, b, N, N, N },
		{ N, N, N, b, N, N, N, N, N, b, N, N, b, N, N, N },
		{ N, N, N, N, b, N, N, N, N, b, b, b, N, N, N, N },
		{ N, N, N, N, N, N, N, N, b, b, b, N, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, b, b, N, N, N, N },
		{ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N }
	}
};



// static GWorlds
LGWorld*	LGAChasingArrowsImp::sArrowGWorlds[chaseArrows_NumFrames];


// ---------------------------------------------------------------------------
//	¥ LGAChasingArrowsImp					Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGAChasingArrowsImp::LGAChasingArrowsImp(
	LStream*	inStream)

	: LGAControlImp(inStream)
{
	mCurrentFrame = 0;
}


// ---------------------------------------------------------------------------
//	¥ LGAChasingArrowsImp					Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LGAChasingArrowsImp::LGAChasingArrowsImp(
	LControlPane*	inControlPane)

	: LGAControlImp(inControlPane)
{
	mCurrentFrame = 0;
}


// ---------------------------------------------------------------------------
//	¥ ~LGAChasingArrowsImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LGAChasingArrowsImp::~LGAChasingArrowsImp()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Init									Stream Initializer		  [public]
// ---------------------------------------------------------------------------

void
LGAChasingArrowsImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream)
{
	SInt16	controlKind;			// We don't use the Stream data, but we
	ResIDT	textTraitID;			//   read it so that the Stream marker moves
	Str255	title;

	*inStream >> controlKind;
	*inStream >> textTraitID;
	inStream->ReadPString(title);

	Init(inControlPane, kControlChasingArrowsProc);
}


// ---------------------------------------------------------------------------
//	¥ Init									Parameterized Initializer [public]
// ---------------------------------------------------------------------------

void
LGAChasingArrowsImp::Init(
	LControlPane*	inControlPane,
	SInt16			/* inControlKind */,
	ConstStringPtr	/* inTitle */,
	ResIDT			/* inTextTraitsID */,
	SInt32			/* inRefCon */)
{
	mControlPane = inControlPane;

	AllocateGWorlds();

	mCurrentFrame = 1;
}


// ---------------------------------------------------------------------------
//	¥ AllocateGWorlds									[public, virtual]
// ---------------------------------------------------------------------------
//	Allocates our GWorlds for the arrows

void
LGAChasingArrowsImp::AllocateGWorlds()
{
	for ( SInt16 ii = 0; ii < chaseArrows_NumFrames; ++ii ) {

		if ( sArrowGWorlds[ii] == nil ) {

			Rect	theRect;
			::MacSetRect(&theRect, 0 ,0, chaseArrows_Columns, chaseArrows_Rows);


			try {
				sArrowGWorlds[ii] = new LGWorld( theRect );

			}

			catch (...) {
				// probably out of memory. If so, try temporary memory
				sArrowGWorlds[ii] = new LGWorld( theRect, 0, useTempMem );
			}

			// draw into it

			StColorPenState::Normalize();

			sArrowGWorlds[ii]->BeginDrawing();

			SInt8 colorIndex;
			for ( SInt16 i = 0; i < chaseArrows_Rows; ++i ) {
				for ( SInt16 j = 0; j < chaseArrows_Columns; ++j ) {
					RGBColor tempColor;
					colorIndex = sArrowMatrix[ii][i][j];
					if ( colorIndex != N ) {
						tempColor = UGAColorRamp::GetColor(colorIndex);
						UGraphicUtils::PaintColorPixel (
											(SInt16) (theRect.left + j),
											(SInt16) (theRect.top + i),
											tempColor );
					}
				}
			}

			sArrowGWorlds[ii]->EndDrawing();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DisposeGWorlds							[static, public]
// ---------------------------------------------------------------------------
//	Disposes of our GWorlds. Normally not called, but available for those
//	that might need it (e.g. working in a code resource).
//
//	BE CAREFUL! No check is made within DrawSelf() to ensure the GWorlds
//	are valid, so only dispose if you are certain no further drawing
//	will occur.

void
LGAChasingArrowsImp::DisposeGWorlds()
{
	for (SInt16 ii = 0; ii < chaseArrows_NumFrames; ++ii) {
		delete sArrowGWorlds[ii];
		sArrowGWorlds[ii] = nil;
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGAChasingArrowsImp::DrawSelf()
{
	Rect		frame;
	CalcLocalFrameRect(frame);

	StClipRgnState	clip(frame);
	mControlPane->ApplyForeAndBackColors();
	::EraseRect(&frame);

	StColorState::Normalize();
	CalcLocalControlRect(frame);
	sArrowGWorlds[mCurrentFrame - 1]->CopyImage(
								UQDGlobals::GetCurrentPort(), frame, srcOr);

	if ( (mControlPane->GetBitDepth() >= 4)  &&
		 (!IsEnabled() || !IsActive()) ) {
		RGBColor opColor   = UGAColorRamp::GetColor(7);
		RGBColor foreColor = UGAColorRamp::GetColor(2);
		UGraphicUtils::BlendGraphic(frame, opColor, foreColor);
	}
}


// ---------------------------------------------------------------------------
//	¥ Idle															  [public]
// ---------------------------------------------------------------------------
//	Animate the chasing arrows

void
LGAChasingArrowsImp::Idle()
{
	mCurrentFrame += 1;

	if (mCurrentFrame > chaseArrows_NumFrames) {
		mCurrentFrame = 1;
	}

	mControlPane->Draw(nil);
}


// ---------------------------------------------------------------------------
//	¥ CalcLocalControlRect										   [protected]
// ---------------------------------------------------------------------------
//	Figure out a rect (used in drawing) to always keep the control centered
//	in the frame

void
LGAChasingArrowsImp::CalcLocalControlRect(
	Rect&	outRect )
{
	CalcLocalFrameRect( outRect );

	SInt16 height  = (SInt16) (outRect.bottom - outRect.top);
	SInt16 width   = (SInt16) (outRect.right - outRect.left);

	::MacSetRect(&outRect,
				(SInt16) ((outRect.left + (width/2)) - (chaseArrows_Width/2)),
				(SInt16) ((outRect.top + (height/2)) - (chaseArrows_Height/2)),
				(SInt16) (outRect.left + chaseArrows_Width),
				(SInt16) (outRect.top + chaseArrows_Height));
}


PP_End_Namespace_PowerPlant
