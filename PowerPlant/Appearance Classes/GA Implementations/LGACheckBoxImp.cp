// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGACheckBoxImp.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGACheckBoxImp.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>

#include <LControl.h>
#include <LStream.h>
#include <PP_Messages.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UQDOperators.h>
#include <LString.h>
#include <UTextTraits.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Constants

const SInt16	checkBoxButton_CheckBoxWidth	=	12;
const SInt16	checkBoxButton_TextOffset		=	5;


// ---------------------------------------------------------------------------
//	¥ LGACheckBoxImp							Constructor			  [public]
// ---------------------------------------------------------------------------

LGACheckBoxImp::LGACheckBoxImp(
	LControlPane*	inControlPane)

	: LGAControlImp (inControlPane)
{
	mClassicCheckMark = false;
}


// ---------------------------------------------------------------------------
//	¥ LGACheckBoxImp							Stream Constructor	  [public]
// ---------------------------------------------------------------------------

LGACheckBoxImp::LGACheckBoxImp(
	LStream*	inStream)

	: LGAControlImp(inStream)
{
	mClassicCheckMark = false;
}


// ---------------------------------------------------------------------------
//	¥ ~LGACheckBoxImp							Destructor			  [public]
// ---------------------------------------------------------------------------

LGACheckBoxImp::~LGACheckBoxImp ()
{
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream)
{
	SInt16	controlKind;
	Str255	title;
	ResIDT	textTraitID;

	*inStream >> controlKind;
	*inStream >> textTraitID;
	inStream->ReadPString(title);

	Init(inControlPane, controlKind, title, textTraitID);
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxImp::Init(
	LControlPane*	inControlPane,
	SInt16			/* inControlKind */,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			/* inRefCon */)
{
	mControlPane  = inControlPane;
	mTitle		  = inTitle;
	mTextTraitsID = inTextTraitsID;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------

StringPtr
LGACheckBoxImp::GetDescriptor(
	Str255	outDescriptor) const
{
	return LString::CopyPStr(mTitle, outDescriptor);
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxImp::SetDescriptor(
	ConstStringPtr	 inDescriptor)
{
	mTitle = inDescriptor;
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ SetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxImp::SetTextTraitsID(
	ResIDT	 inTextTraitsID)
{
	if (mTextTraitsID != inTextTraitsID) {
		mTextTraitsID = inTextTraitsID;
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ PostSetValue													  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxImp::PostSetValue()
{
	if (IsPushed()) {
		SetPushedState(false);

	} else {
		mControlPane->Draw(nil);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetSetClassicCheckMark										  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxImp::SetSetClassicCheckMark(
	Boolean		inClassicMark)
{
	if (mClassicCheckMark != inClassicMark) {
		mClassicCheckMark = inClassicMark;
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcTitleRect													  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxImp::CalcTitleRect	(
		Rect	&outRect )
{
	StTextState		theTextState;
	StColorPenState	thePenState;
													// Get the text traits ID for the control
	ResIDT	textTID = GetTextTraitsID ();

													// Get the port setup with the text traits
	UTextTraits::SetPortTextTraits ( textTID );

													// Figure out the height of the text for the
													// selected font
	FontInfo fi;
	::GetFontInfo ( &fi );
	SInt16		textHeight = (SInt16) (fi.ascent + fi.descent);

													// Get the local inset frame rectangle
	CalcLocalFrameRect ( outRect );
	SInt16		frameHeight = UGraphicUtils::RectHeight ( outRect );

													// If the text height is greater than the frame
													// height then constrain it to the frame height
	if ( frameHeight < textHeight ) {
		textHeight = frameHeight;
	}
													// Now we modify the title rect so that it is to
													// the right of the checkbox control itself
	outRect.top   += (SInt16) ((frameHeight - textHeight) / 2);
	outRect.bottom = (SInt16) (outRect.top + textHeight);
	outRect.left  += checkBoxButton_CheckBoxWidth + checkBoxButton_TextOffset + 1;

}


// ---------------------------------------------------------------------------
//	¥ CalcLocalControlRect											  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxImp::CalcLocalControlRect (
		Rect &outRect )
{
													// Figure out the frame rect for the checkbox
													//  button
	CalcLocalFrameRect ( outRect );
	SInt16		frameHeight = UGraphicUtils::RectHeight ( outRect );
													// Adjust the rect such that it is always
													// centered vertically within the checkbox
													// buttons frame rect
	outRect.left  += 1;
	outRect.right  = (SInt16) (outRect.left + checkBoxButton_CheckBoxWidth);
	outRect.top   += (SInt16) ((frameHeight - checkBoxButton_CheckBoxWidth) / 2);
	outRect.bottom = (SInt16) (outRect.top + checkBoxButton_CheckBoxWidth);

}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxImp::DrawSelf()
{
	StColorPenState::Normalize();

	if (IsEnabled() && IsActive()) {		// Draw box in proper state
		if (IsPushed()) {
			DrawCheckBoxHilited();

		} else {
			DrawCheckBoxNormal();
		}

	} else {
		DrawCheckBoxDimmed();
	}

	if (HasTitle()  &&  !IsPushed()) {		// No need to draw title when
		DrawCheckBoxTitle();				//   pushed, where only the box
	}										//   state changes
}


// ---------------------------------------------------------------------------
//	¥ DrawCheckBoxTitle												  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxImp::DrawCheckBoxTitle()
{
	ResIDT	textTID = GetTextTraitsID();
	Rect	titleRect;

	UTextTraits::SetPortTextTraits(textTID);

	CalcTitleRect(titleRect);

	RGBColor	textColor;
	::GetForeColor(&textColor);

	mControlPane->ApplyForeAndBackColors();

	if ( (mControlPane->GetBitDepth() >= 4)  &&		// Dim color of inactive
		 (!IsEnabled() || !IsActive()) ) {			//   or disabled text

			UTextDrawing::DimTextColor(textColor);
	}

	::RGBForeColor(&textColor);

	UTextDrawing::DrawTruncatedWithJust(mTitle, titleRect,
										teFlushDefault, true);
}


// ---------------------------------------------------------------------------
//	¥ DrawCheckBoxNormal											  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxImp::DrawCheckBoxNormal()
{
	RGBColor tempColor;

	Rect	localFrame;
	CalcLocalControlRect(localFrame);

	::MacFrameRect(&localFrame);			// Draw box border

										// Erase "tail" of check mark that
										// extends outside the box
	mControlPane->GetForeAndBackColors(nil, &tempColor);
	::RGBBackColor(&tempColor);
	Rect	extra;
	extra.left   = localFrame.right;
	extra.right  = (SInt16) (extra.left + 2);
	extra.top    = (SInt16) (localFrame.top + 1);
	extra.bottom = (SInt16) (extra.top + 3);
	::EraseRect(&extra);

	if ( mControlPane->GetBitDepth() < 4 ) {	// BLACK & WHITE

		::MacInsetRect ( &localFrame, 1, 1 );	// Erase interior to remove
		::EraseRect ( &localFrame );			//   any existing mark
		::MacInsetRect ( &localFrame, -1, -1 );

	} else {									// COLOR
												// PAINT FACE
		::MacInsetRect ( &localFrame, 1, 1 );
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
		::RGBForeColor ( &tempColor );
		::PaintRect ( &localFrame );

												// LIGHT EDGES
		::RGBForeColor ( &Color_White );
		UGraphicUtils::TopLeftSide ( localFrame, 0, 0, 1, 1 );

												// SHADOW EDGES
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
		::RGBForeColor ( &tempColor );
		UGraphicUtils::BottomRightSide ( localFrame, 1, 1, 0, 0 );

												// Prepare for the drawing of the check mark
		::MacInsetRect ( &localFrame, -1, -1 );

												// CHECK MARK SHADOWS
		if ( IsMixedState ()) {
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
			::RGBForeColor ( &tempColor );
			::PenSize ( 1, 1 );
			::MoveTo ( 		(SInt16) (localFrame.left + 4),
							(SInt16) (localFrame.top + 7) );
			::MacLineTo ( 	(SInt16) (localFrame.right - 3),
							(SInt16) (localFrame.top + 7) );
			::MacLineTo ( 	(SInt16) (localFrame.right - 3),
							(SInt16) (localFrame.top + 5) );

		} else if ( IsSelected ()) {
			if ( mClassicCheckMark ) {
				::PenSize ( 1, 1 );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				::RGBForeColor ( &tempColor );
				::MoveTo ( (SInt16) (localFrame.left + 4),  (SInt16) (localFrame.bottom - 4) );
				::MacLineTo ( (SInt16) (localFrame.right - 3), (SInt16) (localFrame.top + 3) );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
				::RGBForeColor ( &tempColor );
				::MoveTo ( (SInt16) (localFrame.left + 4),  (SInt16) (localFrame.bottom - 3) );
				::MacLineTo ( (SInt16) (localFrame.right - 3), (SInt16) (localFrame.top + 4) );

												// Fixup a couple of pixels
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 4),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 3),
										tempColor );
			} else {
												// Draw the light gray shadows
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.left + 3),
										(SInt16) (localFrame.top + 7),
										tempColor );
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.left + 4),
										(SInt16) (localFrame.top + 8),
										tempColor );
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.left + 6),
										(SInt16) (localFrame.top + 9),
										tempColor );
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.left + 7),
										(SInt16) (localFrame.top + 8),
										tempColor );
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.left + 8),
										(SInt16) (localFrame.top + 7),
										tempColor );
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.left + 9),
										(SInt16) (localFrame.top + 6),
										tempColor );

				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.left + 10),
										(SInt16) (localFrame.top + 5),
										tempColor );

				tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.left + 12),
										(SInt16) (localFrame.top + 3),
										tempColor );
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.left + 13),
										(SInt16) (localFrame.top + 2),
										tempColor );

												// DARK SHADOWS
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.left + 5),
										(SInt16) (localFrame.top + 9),
										tempColor );
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.left + 6),
										(SInt16) (localFrame.top + 8),
										tempColor );
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.left + 7),
										(SInt16) (localFrame.top + 7),
										tempColor );
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.left + 8),
										(SInt16) (localFrame.top + 6),
										tempColor );
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.left + 9),
										(SInt16) (localFrame.top + 5),
										tempColor );

				tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.left + 10),
										(SInt16) (localFrame.top + 4),
										tempColor );

				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.left + 12),
										(SInt16) (localFrame.top + 2),
										tempColor );
			}
		}
	}
												// CHECK MARK
												// Draw the check mark inside the checkbox this
												// is always drawn in black so we just draw it
												// outside the depth stuff
	::RGBForeColor ( &Color_Black );
	if ( IsMixedState ()) {
		::PenSize ( 1, 2 );
		::MoveTo ( 		(SInt16) (localFrame.left + 3),
						(SInt16) (localFrame.top + 5) );
		::MacLineTo ( 	(SInt16) (localFrame.right - 4),
						(SInt16) (localFrame.top + 5) );

	} else if ( IsSelected ()) {

		::PenSize ( 1, 1 );

		if ( mClassicCheckMark ) {
			::PenSize ( 1, 2 );
			::MoveTo ( (SInt16) (localFrame.left + 3),  (SInt16) (localFrame.top + 2) );
			::MacLineTo ( (SInt16) (localFrame.right - 4), (SInt16) (localFrame.bottom - 5) );
			::MoveTo ( (SInt16) (localFrame.left + 3),  (SInt16) (localFrame.bottom - 5) );
			::MacLineTo ( (SInt16) (localFrame.right - 4), (SInt16) (localFrame.top + 2) );

		} else {

			::MoveTo ( (SInt16) (localFrame.left + 2),  (SInt16) (localFrame.top + 5) );
			::MacLineTo ( (SInt16) (localFrame.left + 2 + 1), (SInt16) (localFrame.top + 5) );
			::MoveTo ( (SInt16) (localFrame.left + 3),  (SInt16) (localFrame.top + 6) );
			::MacLineTo ( (SInt16) (localFrame.left + 3 + 1), (SInt16) (localFrame.top + 6) );
			::MoveTo ( (SInt16) (localFrame.left + 4),  (SInt16) (localFrame.top + 7) );
			::MacLineTo ( (SInt16) (localFrame.left + 4 + 2), (SInt16) (localFrame.top + 7) );
			::MoveTo ( (SInt16) (localFrame.left + 6),  (SInt16) (localFrame.top + 6) );
			::MacLineTo ( (SInt16) (localFrame.left + 6 + 1), (SInt16) (localFrame.top + 6) );
			::MoveTo ( (SInt16) (localFrame.left + 7),  (SInt16) (localFrame.top + 5) );
			::MacLineTo ( (SInt16) (localFrame.left + 7 + 1), (SInt16) (localFrame.top + 5) );
			::MoveTo ( (SInt16) (localFrame.left + 8),  (SInt16) (localFrame.top + 4) );
			::MacLineTo ( (SInt16) (localFrame.left + 8 + 1), (SInt16) (localFrame.top + 4) );
			::MoveTo ( (SInt16) (localFrame.left + 9),  (SInt16) (localFrame.top + 3) );
			::MacLineTo ( (SInt16) (localFrame.left + 9 + 1), (SInt16) (localFrame.top + 3) );
			::MoveTo ( (SInt16) (localFrame.left + 10),  (SInt16) (localFrame.top + 2) );
			::MacLineTo ( (SInt16) (localFrame.left + 10 + 1), (SInt16) (localFrame.top + 2) );
			::MoveTo ( (SInt16) (localFrame.left + 11),  (SInt16) (localFrame.top + 1) );
			::MacLineTo ( (SInt16) (localFrame.left + 11 + 1), (SInt16) (localFrame.top + 1) );

			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 5),
									(SInt16) (localFrame.top + 8),
									Color_Black );
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawCheckBoxHilited											  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxImp::DrawCheckBoxHilited	()
{
													// Get the frame for the control
	Rect	localFrame, loopRect;
	CalcLocalControlRect ( localFrame );
	loopRect = localFrame;
	loopRect.right += 2;

	RGBColor tempColor;
													// BLACK & WHITE
	if ( mControlPane->GetBitDepth() < 4 ) {
		if ( IsPushed ()) {
												// BORDER
			::PenSize ( 2, 2 );
			::MacFrameRect ( &localFrame );

												// Now we need to erase the interior of the box
												// to get rid of any previous state
			::MacInsetRect ( &localFrame, 2, 2 );
			::EraseRect ( &localFrame );
			::MacInsetRect ( &localFrame, -2, -2 );
		}

	} else {									// COLOR

		if ( IsPushed ()) {
												// Start by rendering the border and background
												// BORDER
			::MacFrameRect ( &localFrame );

												// PAINT FACE
			::MacInsetRect ( &localFrame, 1, 1 );
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
			::RGBForeColor ( &tempColor );
			::PaintRect ( &localFrame );

												// SHADOW EDGES
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
			::RGBForeColor ( &tempColor );
			UGraphicUtils::TopLeftSide ( localFrame, 0, 0, 1, 1 );

												// LIGHT EDGES
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray6);
			::RGBForeColor ( &tempColor );
			UGraphicUtils::BottomRightSide ( localFrame, 1, 1, 0, 0 );

												// Prepare for the drawing of the check mark
			::MacInsetRect ( &localFrame, -1, -1 );

												// CHECK MARK SHADOWS
			if ( IsMixedState ()) {
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
				::RGBForeColor ( &tempColor );
				::PenSize ( 1, 1 );
				::MoveTo ( 		(SInt16) (localFrame.left + 4),
								(SInt16) (localFrame.top + 7) );
				::MacLineTo ( 	(SInt16) (localFrame.right - 3),
								(SInt16) (localFrame.top + 7) );
				::MacLineTo ( 	(SInt16) (localFrame.right - 3),
								(SInt16) (localFrame.top + 5) );

			} else if ( IsSelected ()) {

				if ( mClassicCheckMark ) {
					::PenSize ( 1, 1 );
					tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
					::RGBForeColor ( &tempColor );
					::MoveTo ( (SInt16) (localFrame.left + 4),  (SInt16) (localFrame.bottom - 4) );
					::MacLineTo ( (SInt16) (localFrame.right - 3), (SInt16) (localFrame.top + 3) );
					tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
					::RGBForeColor ( &tempColor );
					::MoveTo ( (SInt16) (localFrame.left + 4),  (SInt16) (localFrame.bottom - 3) );
					::MacLineTo ( (SInt16) (localFrame.right - 3), (SInt16) (localFrame.top + 4) );

												// Fixup a couple of pixels
					tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.right - 3),
											(SInt16) (localFrame.bottom - 4),
											tempColor );
					tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.right - 3),
											(SInt16) (localFrame.bottom - 3),
											tempColor );

				} else {
												// Draw the light gray shadows
					tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.left + 3),
											(SInt16) (localFrame.top + 7),
											tempColor );
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.left + 4),
											(SInt16) (localFrame.top + 8),
											tempColor );
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.left + 6),
											(SInt16) (localFrame.top + 9),
											tempColor );
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.left + 7),
											(SInt16) (localFrame.top + 8),
											tempColor );
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.left + 8),
											(SInt16) (localFrame.top + 7),
											tempColor );
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.left + 9),
											(SInt16) (localFrame.top + 6),
											tempColor );

					tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.left + 10),
											(SInt16) (localFrame.top + 5),
											tempColor );

					tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.left + 12),
											(SInt16) (localFrame.top + 3),
											tempColor );
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.left + 13),
											(SInt16) (localFrame.top + 2),
											tempColor );

												// DARK SHADOWS
					tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.left + 5),
											(SInt16) (localFrame.top + 9),
											tempColor );
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.left + 6),
											(SInt16) (localFrame.top + 8),
											tempColor );
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.left + 7),
											(SInt16) (localFrame.top + 7),
											tempColor );
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.left + 8),
											(SInt16) (localFrame.top + 6),
											tempColor );
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.left + 9),
											(SInt16) (localFrame.top + 5),
											tempColor );

					tempColor = UGAColorRamp::GetColor(colorRamp_Gray12);
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.left + 10),
											(SInt16) (localFrame.top + 4),
											tempColor );

					tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
					UGraphicUtils::PaintColorPixel (
											(SInt16) (localFrame.left + 12),
											(SInt16) (localFrame.top + 2),
											tempColor );
				}
			}
		}
	}
												// CHECK MARK
												// Draw the check mark inside the checkbox this
												// is always drawn in black so we just draw it
												// outside the depth stuff
	if ( IsMixedState ()) {
		::RGBForeColor ( &Color_Black );
		::PenSize ( 1, 2 );
		::MoveTo ( 		(SInt16) (localFrame.left + 3),
						(SInt16) (localFrame.top + 5) );
		::MacLineTo ( 	(SInt16) (localFrame.right - 4),
						(SInt16) (localFrame.top + 5) );

	} else if ( IsPushed () && IsSelected ()) {

		tempColor = Color_Black;
		::RGBForeColor ( &tempColor );
		::PenSize ( 1, 1 );

		if ( mClassicCheckMark ) {
			::PenSize ( 1, 2 );
			::MoveTo ( (SInt16) (localFrame.left + 3),  (SInt16) (localFrame.top + 2) );
			::MacLineTo ( (SInt16) (localFrame.right - 4), (SInt16) (localFrame.bottom - 5) );
			::MoveTo ( (SInt16) (localFrame.left + 3),  (SInt16) (localFrame.bottom - 5) );
			::MacLineTo ( (SInt16) (localFrame.right - 4), (SInt16) (localFrame.top + 2) );

		} else {

			::MoveTo ( (SInt16) (localFrame.left + 2),  (SInt16) (localFrame.top + 5) );
			::MacLineTo ( (SInt16) (localFrame.left + 2 + 1), (SInt16) (localFrame.top + 5) );
			::MoveTo ( (SInt16) (localFrame.left + 3),  (SInt16) (localFrame.top + 6) );
			::MacLineTo ( (SInt16) (localFrame.left + 3 + 1), (SInt16) (localFrame.top + 6) );
			::MoveTo ( (SInt16) (localFrame.left + 4),  (SInt16) (localFrame.top + 7) );
			::MacLineTo ( (SInt16) (localFrame.left + 4 + 2), (SInt16) (localFrame.top + 7) );
			::MoveTo ( (SInt16) (localFrame.left + 6),  (SInt16) (localFrame.top + 6) );
			::MacLineTo ( (SInt16) (localFrame.left + 6 + 1), (SInt16) (localFrame.top + 6) );
			::MoveTo ( (SInt16) (localFrame.left + 7),  (SInt16) (localFrame.top + 5) );
			::MacLineTo ( (SInt16) (localFrame.left + 7 + 1), (SInt16) (localFrame.top + 5) );
			::MoveTo ( (SInt16) (localFrame.left + 8),  (SInt16) (localFrame.top + 4) );
			::MacLineTo ( (SInt16) (localFrame.left + 8 + 1), (SInt16) (localFrame.top + 4) );
			::MoveTo ( (SInt16) (localFrame.left + 9),  (SInt16) (localFrame.top + 3) );
			::MacLineTo ( (SInt16) (localFrame.left + 9 + 1), (SInt16) (localFrame.top + 3) );
			::MoveTo ( (SInt16) (localFrame.left + 10),  (SInt16) (localFrame.top + 2) );
			::MacLineTo ( (SInt16) (localFrame.left + 10 + 1), (SInt16) (localFrame.top + 2) );
			::MoveTo ( (SInt16) (localFrame.left + 11),  (SInt16) (localFrame.top + 1) );
			::MacLineTo ( (SInt16) (localFrame.left + 11 + 1), (SInt16) (localFrame.top + 1) );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 5),
									(SInt16) (localFrame.top + 8),
									tempColor );
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawCheckBoxDimmed											  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxImp::DrawCheckBoxDimmed	()
{
													// Get the frame for the control
	Rect	localFrame, loopRect;
	CalcLocalControlRect ( localFrame );
	loopRect = localFrame;
	loopRect.right += 2;

	RGBColor	tempColor;
	SInt16		depth = mControlPane->GetBitDepth();
													// BLACK & WHITE
	if (depth < 4) {
		StColorPenState::SetGrayPattern();
		::MacFrameRect ( &localFrame );

	} else {
												// Start by rendering the border and background
												// BORDER
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
		::RGBForeColor ( &tempColor );
		::MacFrameRect ( &localFrame );

											// PAINT FACE
		::MacInsetRect ( &localFrame, 1, 1 );
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
		::RGBForeColor ( &tempColor );
		::PaintRect ( &localFrame );

											// Prepare for the drawing of the check mark
		::MacInsetRect ( &localFrame, -1, -1 );
	}

												// CHECK MARK
												// Draw the check mark inside the checkbox this
												// is always drawn in black so we just draw it
												// outside the depth stuff
	if ( IsMixedState ()) {
		tempColor = Color_Black;
		if (depth >= 4) {
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
		}
		::RGBForeColor ( &tempColor );
		::PenSize ( 1, 2 );
		::MoveTo ( 		(SInt16) (localFrame.left + 3),
						(SInt16) (localFrame.top + 5) );
		::MacLineTo ( 	(SInt16) (localFrame.right - 4),
						(SInt16) (localFrame.top + 5) );

	} else if ( IsSelected ()) {
		tempColor = Color_Black;
		if (depth >= 4) {
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
		}

		::RGBForeColor ( &tempColor );

		if ( mClassicCheckMark ) {
			::PenSize ( 1, 2 );
			::MoveTo ( (SInt16) (localFrame.left + 3),  (SInt16) (localFrame.top + 2) );
			::MacLineTo ( (SInt16) (localFrame.right - 4), (SInt16) (localFrame.bottom - 5) );
			::MoveTo ( (SInt16) (localFrame.left + 3),  (SInt16) (localFrame.bottom - 5) );
			::MacLineTo ( (SInt16) (localFrame.right - 4), (SInt16) (localFrame.top + 2) );

		} else {

			::PenSize ( 1, 1 );
			::MoveTo ( (SInt16) (localFrame.left + 2),  (SInt16) (localFrame.top + 5) );
			::MacLineTo ( (SInt16) (localFrame.left + 2 + 1), (SInt16) (localFrame.top + 5) );
			::MoveTo ( (SInt16) (localFrame.left + 3),  (SInt16) (localFrame.top + 6) );
			::MacLineTo ( (SInt16) (localFrame.left + 3 + 1), (SInt16) (localFrame.top + 6) );
			::MoveTo ( (SInt16) (localFrame.left + 4),  (SInt16) (localFrame.top + 7) );
			::MacLineTo ( (SInt16) (localFrame.left + 4 + 2), (SInt16) (localFrame.top + 7) );
			::MoveTo ( (SInt16) (localFrame.left + 6),  (SInt16) (localFrame.top + 6) );
			::MacLineTo ( (SInt16) (localFrame.left + 6 + 1), (SInt16) (localFrame.top + 6) );
			::MoveTo ( (SInt16) (localFrame.left + 7),  (SInt16) (localFrame.top + 5) );
			::MacLineTo ( (SInt16) (localFrame.left + 7 + 1), (SInt16) (localFrame.top + 5) );
			::MoveTo ( (SInt16) (localFrame.left + 8),  (SInt16) (localFrame.top + 4) );
			::MacLineTo ( (SInt16) (localFrame.left + 8 + 1), (SInt16) (localFrame.top + 4) );
			::MoveTo ( (SInt16) (localFrame.left + 9),  (SInt16) (localFrame.top + 3) );
			::MacLineTo ( (SInt16) (localFrame.left + 9 + 1), (SInt16) (localFrame.top + 3) );
			::MoveTo ( (SInt16) (localFrame.left + 10),  (SInt16) (localFrame.top + 2) );
			::MacLineTo ( (SInt16) (localFrame.left + 10 + 1), (SInt16) (localFrame.top + 2) );
			::MoveTo ( (SInt16) (localFrame.left + 11),  (SInt16) (localFrame.top + 1) );
			::MacLineTo ( (SInt16) (localFrame.left + 11 + 1), (SInt16) (localFrame.top + 1) );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 5),
									(SInt16) (localFrame.top + 8),
									tempColor );
		}
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CalcBestControlRect											  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxImp::CalcBestControlRect (
		SInt16	&outWidth,
		SInt16	&outHeight,
		SInt16	&outTextBaseline ) const
{

	StTextState			theTextState;
													// Get the text for the caption
	Str255	text;
	GetDescriptor ( text );
													// Get the port setup too the text traits values
	UTextTraits::SetPortTextTraits ( GetTextTraitsID ());

													// Get the font info
	FontInfo	fi;
	::GetFontInfo ( &fi );

													// Figure out best width
	SInt16	bestWidth = (SInt16) (checkBoxButton_CheckBoxWidth + checkBoxButton_TextOffset +
				::StringWidth ( text ) + 1);

													// Figure out best height
	SInt16	bestHeight = (SInt16) (fi.ascent + fi.descent);
	if ( bestHeight < (checkBoxButton_CheckBoxWidth + 2) ) {
		bestHeight = (checkBoxButton_CheckBoxWidth + 2);
	}

													// Adjust best rect
	outWidth = bestWidth;
	outHeight = bestHeight;

													// Set baseline
	outTextBaseline = (SInt16) (fi.ascent - (fi.ascent == 12 ? 2 : 1));
}


PP_End_Namespace_PowerPlant
