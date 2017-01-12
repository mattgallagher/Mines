// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGroupBox.cp				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	View class for a gray box with a text title

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGroupBox.h>
#include <UDrawingState.h>
#include <URegions.h>
#include <UTextTraits.h>

PP_Begin_Namespace_PowerPlant


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LGroupBox								Default Constructor		  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LGroupBox::LGroupBox()
{
	mFrameColor.red = mFrameColor.blue = mFrameColor.green = 0x8000;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LGroupBox								Copy Constructor		  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LGroupBox::LGroupBox(
	const LGroupBox&		inOriginal)

	: LCaption(inOriginal)
{
	mFrameColor.red = mFrameColor.blue = mFrameColor.green = 0x8000;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LGroupBox								Parameterized Constructor [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LGroupBox::LGroupBox(
	const SPaneInfo&	inPaneInfo,
	Str255				inString,
	ResIDT				inTextTraitsID)

	: LCaption(inPaneInfo, inString, inTextTraitsID)
{
	mFrameColor.red = mFrameColor.blue = mFrameColor.green = 0x8000;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LGroupBox								Stream Constructor		  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LGroupBox::LGroupBox(
	LStream*	inStream)

	: LCaption(inStream)
{
	mFrameColor.red = mFrameColor.blue = mFrameColor.green = 0x8000;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ ~LGroupBox							Destructor				  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LGroupBox::~LGroupBox()
{
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ DrawSelf													   [protected]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Draw GroupBox
//
//  We draw in two stages. First we draw the frame rectangle,
//  minus the area where the title text is drawn; then we draw the
//  title text. In order to draw without any flashing, we find the
//  frame of the text box and clip it out of the drawing region.
//
void
LGroupBox::DrawSelf()
{
	Rect textFrame;
	CalcTextBoxFrame(textFrame);

	StRegion	newClip;
	::GetClip(newClip);

	StRegion	textRgn(textFrame);
	newClip -= textRgn;

	{
		StClipRgnState clip(newClip);

		Rect borderFrame;
		CalcLocalFrameRect(borderFrame);
		if (textFrame.right > textFrame.left)
			borderFrame.top += (textFrame.bottom - textFrame.top) >> 1;

		DrawBorder(borderFrame);
	}

	::PenNormal();
	DrawText(textFrame);
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ DrawText													   [protected]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Draw the text given the placement chosen by CalcTextBoxFrame

void
LGroupBox::DrawText(
	const Rect&		inRect)
{
		// Retrieve info about font again.

	FontInfo fInfo;
	::GetFontInfo(&fInfo);

		// Set background color

	RGBColor	textColor;			// Text has its own foreground color
	::GetForeColor(&textColor);

	ApplyForeAndBackColors();
	::EraseRect(&inRect);

	::RGBForeColor(&textColor);

	::MoveTo((SInt16) (inRect.left + 3),
			 (SInt16) (inRect.top + fInfo.ascent + (fInfo.leading >> 1)));
	::DrawString(mText);

}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ DrawBorder												   [protected]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Draw border around the GroupBox

void
LGroupBox::DrawBorder(
	const Rect&		inRect)
{
	::PenNormal();
	StColorPenState savePenState;		// Will save and restore pen state

	if (GetBitDepth() > 2) {
		::RGBForeColor(&mFrameColor);
	} else {
		Pattern		grayPat;
		::PenPat(UQDGlobals::GetGrayPat(&grayPat));
	}

	::MacFrameRect(&inRect);
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ CalcTextBoxFrame											   [protected]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Decides where the text will be drawn, but does not draw the text

void
LGroupBox::CalcTextBoxFrame(
	Rect&	outRect)
{
	// Find the edges of the pane.

	CalcLocalFrameRect(outRect);


	// If text is empty, return an empty rectangle.

	if (mText.Length() == 0) {
		outRect.right = outRect.left;
		outRect.bottom = outRect.top;
		return;
	}


	// We have some text, find its size and place it.
	// This version is pretty naive; it assumes the text will
	// fit comfortably on a single line.

	SInt16 just			= UTextTraits::SetPortTextTraits(mTxtrID);
	SInt16 strSize		= (SInt16) (::StringWidth(mText) + 6);		// 3 pixels slop on either side
	SInt16 frameWidth	= (SInt16) (outRect.right - outRect.left);

	FontInfo fInfo;						// vertical placement is always at top
	::GetFontInfo(&fInfo);
	outRect.bottom = (SInt16) (outRect.top + fInfo.ascent + fInfo.descent + fInfo.leading);

	switch (just) {						// horizontal placement depends on Txtr

		case teCenter:
			outRect.left += (frameWidth - strSize) >> 1;
			outRect.right = (SInt16) (outRect.left + strSize);
			break;

		case teFlushRight:
			outRect.right -= 6;
			outRect.left = (SInt16) (outRect.right - strSize);
			break;

		case teFlushDefault:
		case teFlushLeft:
		default:
			outRect.left += 6;
			outRect.right = (SInt16) (outRect.left + strSize);
			break;
	}
}


PP_End_Namespace_PowerPlant
