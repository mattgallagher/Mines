// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UNewTextDrawing.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UNewTextDrawing.h>

#include <FixMath.h>
#include <Script.h>
#include <TextEdit.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ MeasureWithJustification
// ---------------------------------------------------------------------------
//	Returns the bounding box of text that would be drawn by
//	UTextDrawing::DrawWithJustification.

void
UNewTextDrawing::MeasureWithJustification(
	Ptr			inText,
	SInt32		inLength,
	const Rect&	inRect,
	SInt16		inJustification,
	Rect&		outTextRect,
	Boolean		inMeasureTrailingSpace)
{

	Fixed fixedWidth = ::Long2Fix(inRect.right - inRect.left);

	FontInfo fontInfo;
	::GetFontInfo(&fontInfo);

	SInt16 lineHeight = (SInt16) (fontInfo.ascent + fontInfo.descent + fontInfo.leading);
	SInt16 lineBase   = (SInt16) (inRect.top + fontInfo.ascent + fontInfo.leading);
	
	if (inLength <= 0) {					// Special case for no text
		outTextRect.left	= inRect.left;
		outTextRect.right	= inRect.right;
		outTextRect.top		= inRect.top;
		outTextRect.bottom	= (SInt16) (inRect.top + lineHeight);
		
	} else {
		outTextRect.top = outTextRect.bottom = inRect.top;
		outTextRect.left = inRect.right;
		outTextRect.right = inRect.left;

		Fixed wrapWidth;
		SInt32 blackSpace, lineBytes, textWidth;
		SInt32 textLeft = inLength;
		Ptr textEnd = inText + inLength;
		StyledLineBreakCode lineBreak;

		SInt16 justification = inJustification;
		if (justification == teFlushDefault) {
			justification = ::GetSysDirection();
		}

		while ((inText < textEnd) && (lineBase <= inRect.bottom)) {
			lineBytes = 1;
			wrapWidth = fixedWidth;

			SInt32 textLen = textLeft;		// Note in IM:Text 5-80 states
			if (textLen > max_Int16) {		//   that length is limited to the
				textLen = max_Int16;		//   integer range
			}

			lineBreak = ::StyledLineBreak(inText, textLen, 0, textLen, 0, &wrapWidth, &lineBytes);
			blackSpace = inMeasureTrailingSpace ? lineBytes : ::VisibleLength(inText, lineBytes);
			textWidth = ::TextWidth(inText, 0, (SInt16) blackSpace);

			SInt16 leftEdge = inRect.left;

			switch (justification) {

				case teFlushRight:
					leftEdge = (SInt16) (inRect.right - textWidth);
					break;

				case teJustCenter:
					leftEdge = (SInt16) (inRect.left + ((inRect.right - inRect.left) - textWidth) / 2);
					break;
			}

			SInt16 rightEdge = (SInt16) (leftEdge + textWidth);

			if (leftEdge < outTextRect.left)
				outTextRect.left = leftEdge;
			if (rightEdge > outTextRect.right)
				outTextRect.right = rightEdge;

			outTextRect.bottom = (SInt16) (lineBase + fontInfo.descent);

			lineBase += lineHeight;
			inText += lineBytes;
			textLeft -= lineBytes;
		}
	}
}


PP_End_Namespace_PowerPlant
