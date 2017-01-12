// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UATSUI.cp					PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UATSUI.h>
#include <UDrawingState.h>
#include <UEnvironment.h>


PP_Begin_Namespace_PowerPlant

// ===========================================================================
//	LATSUIStyle
// ===========================================================================
//	Wrapper class for an ATSUStyle entity

// ---------------------------------------------------------------------------
//	¥ LATSUIStyle							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LATSUIStyle::LATSUIStyle()
{
	ThrowIfOSStatus_( ::ATSUCreateStyle(&mStyle) );
}


// ---------------------------------------------------------------------------
//	¥ LATSUIStyle							Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LATSUIStyle::LATSUIStyle(
	const LATSUIStyle&	inOriginal)
{
	ThrowIfOSStatus_( ::ATSUCreateAndCopyStyle( inOriginal.mStyle,
												&mStyle ) );
}


// ---------------------------------------------------------------------------
//	¥ ~LATSUIStyle							Destructor				  [public]
// ---------------------------------------------------------------------------

LATSUIStyle::~LATSUIStyle()
{
	::ATSUDisposeStyle(mStyle);
}


// ---------------------------------------------------------------------------
//	¥ SetOneAttribute												  [public]
// ---------------------------------------------------------------------------

void
LATSUIStyle::SetOneAttribute(
	ATSUAttributeTag		inTag,
	ByteCount				inDataSize,
	ATSUAttributeValuePtr	inValuePtr)
{
	ThrowIfOSStatus_( ::ATSUSetAttributes( mStyle, 1,
										   &inTag,
										   &inDataSize,
										   &inValuePtr) );
}


// ---------------------------------------------------------------------------
//	¥ SetMultipleAttributes											  [public]
// ---------------------------------------------------------------------------

void
LATSUIStyle::SetMultipleAttributes(
	ItemCount					inNumAttrs,
	const ATSUAttributeTag		inTags[],
	const ByteCount				inValueSizes[],
	const ATSUAttributeValuePtr	inValues[])
{
	ThrowIfOSStatus_( ::ATSUSetAttributes( mStyle,
										   inNumAttrs,
										   inTags,
										   inValueSizes,
										   inValues ) );
}


// ---------------------------------------------------------------------------
//	¥ ClearOneAttribute												  [public]
// ---------------------------------------------------------------------------

void
LATSUIStyle::ClearOneAttribute(
	ATSUAttributeTag		inTag)
{
	ThrowIfOSStatus_( ::ATSUClearAttributes(mStyle, 1, &inTag) );
}


// ---------------------------------------------------------------------------
//	¥ ClearAllAttributes											  [public]
// ---------------------------------------------------------------------------

void
LATSUIStyle::ClearAllAttributes()
{
	ThrowIfOSStatus_( ::ATSUClearStyle(mStyle) );
}


// ---------------------------------------------------------------------------
//	¥ IsEmpty														  [public]
// ---------------------------------------------------------------------------

bool
LATSUIStyle::IsEmpty() const
{
	Boolean	isClear;
	
	ThrowIfOSStatus_( ::ATSUStyleIsEmpty(mStyle, &isClear) );
	
	return isClear;
}


// ---------------------------------------------------------------------------
//	¥ SetThemeFont													  [public]
// ---------------------------------------------------------------------------
//	Set attributes from a Theme Font ID

void
LATSUIStyle::SetThemeFont(
	ThemeFontID		inThemeFontID,
	ScriptCode		inScript)
{
	OSStatus	status;
	
	Str255		fontName;			// Get info about the Theme font
	SInt16		fontSize;
	Style		fontStyle;
	
	status = ::GetThemeFont( inThemeFontID, inScript,
							 fontName, &fontSize, &fontStyle );
	ThrowIfOSStatus_(status);

									// Get the QuickDraw font family ID
									//   and use that to find the ATSU
									//   font ID
	SInt16		fontFamily;
	::GetFNum(fontName, &fontFamily);
	
	ATSUFontID	atsuFont;
	
	status = ::ATSUFONDtoFontID(fontFamily, fontStyle, &atsuFont);
	ThrowIfOSStatus_(status);
										
									// We will set the ATSU style attributes
									// from the theme font information
	
	ATSUAttributeTag	tags[] = { kATSUFontTag,
								   kATSUSizeTag,
								   kATSUQDBoldfaceTag,
								   kATSUQDItalicTag,
								   kATSUQDUnderlineTag,
								   kATSUQDCondensedTag,
								   kATSUQDExtendedTag };
								   
	ByteCount			sizes[] = { sizeof(ATSUFontID),
									sizeof(Fixed),
									sizeof(Boolean),
									sizeof(Boolean),
									sizeof(Boolean),
									sizeof(Boolean),
									sizeof(Boolean) };
									
	ATSUAttributeValuePtr	values[7];
	
	Fixed		atsuSize	= IntToFixed(fontSize);
	Boolean		bTrue		= true;
	Boolean		bFalse		= false;
	
	values[0] = &atsuFont;
	values[1] = &atsuSize;
	values[2] = ((fontStyle & bold) != 0)		? &bTrue : &bFalse;
	values[3] = ((fontStyle & italic) != 0)		? &bTrue : &bFalse;
	values[4] = ((fontStyle & underline) != 0)	? &bTrue : &bFalse;
	values[5] = ((fontStyle & condense) != 0)	? &bTrue : &bFalse;
	values[6] = ((fontStyle & extend) != 0)		? &bTrue : &bFalse;
	
	status = ::ATSUSetAttributes(mStyle, 7, tags, sizes, values);
	ThrowIfOSStatus_(status);
}


// ---------------------------------------------------------------------------
//	¥ SetBoldface													  [public]
// ---------------------------------------------------------------------------

void
LATSUIStyle::SetBoldface(
	Boolean	inBoldface)
{
	SetOneAttribute(kATSUQDBoldfaceTag, sizeof(Boolean), &inBoldface);
}


// ---------------------------------------------------------------------------
//	¥ SetItalic														  [public]
// ---------------------------------------------------------------------------

void
LATSUIStyle::SetItalic(
	Boolean	inItalic)
{
	SetOneAttribute(kATSUQDItalicTag, sizeof(Boolean), &inItalic);
}


// ---------------------------------------------------------------------------
//	¥ SetUnderline													  [public]
// ---------------------------------------------------------------------------

void
LATSUIStyle::SetUnderline(
	Boolean	inUnderline)
{
	SetOneAttribute(kATSUQDUnderlineTag, sizeof(Boolean), &inUnderline);
}


// ---------------------------------------------------------------------------
//	¥ SetCondensed													  [public]
// ---------------------------------------------------------------------------

void
LATSUIStyle::SetCondensed(
	Boolean	inCondensed)
{
	SetOneAttribute(kATSUQDCondensedTag, sizeof(Boolean), &inCondensed);
}


// ---------------------------------------------------------------------------
//	¥ SetExtended													  [public]
// ---------------------------------------------------------------------------

void
LATSUIStyle::SetExtended(
	Boolean	inExtended)
{
	SetOneAttribute(kATSUQDExtendedTag, sizeof(Boolean), &inExtended);
}


// ---------------------------------------------------------------------------
//	¥ SetATSUFont													  [public]
// ---------------------------------------------------------------------------

void
LATSUIStyle::SetATSUFont(
	ATSUFontID	inFontID)
{
	SetOneAttribute(kATSUFontTag, sizeof(ATSUFontID), &inFontID);
}


// ---------------------------------------------------------------------------
//	¥ SetFontSize													  [public]
// ---------------------------------------------------------------------------

void
LATSUIStyle::SetFontSize(
	Fixed	inFontSize)
{
	SetOneAttribute(kATSUSizeTag, sizeof(Fixed), &inFontSize);
}


// ---------------------------------------------------------------------------
//	¥ SetColor														  [public]
// ---------------------------------------------------------------------------

void
LATSUIStyle::SetColor(
	const RGBColor&	inColor)
{
	SetOneAttribute(kATSUColorTag, sizeof(RGBColor), (void*) &inColor);
}

#pragma mark -
// ===========================================================================
//	LATSUITextLayout
// ===========================================================================
//	Wrapper class for an ATSUTextLayout entity

// ---------------------------------------------------------------------------
//	¥ LATSUITextLayout						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LATSUITextLayout::LATSUITextLayout()
{
	ThrowIfOSStatus_( ::ATSUCreateTextLayout(&mTextLayout) );
}


// ---------------------------------------------------------------------------
//	¥ LATSUITextLayout						Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LATSUITextLayout::LATSUITextLayout(
	const LATSUITextLayout&	inOriginal)
{
	ThrowIfOSStatus_( ::ATSUCreateAndCopyTextLayout(
									inOriginal.mTextLayout,
									&mTextLayout ) );
}


// ---------------------------------------------------------------------------
//	¥ LATSUITextLayout						Parameterized Constructor [public]
// ---------------------------------------------------------------------------
//	Create from a Text pointer and a single style

LATSUITextLayout::LATSUITextLayout(
	ConstUniCharArrayPtr	inTextPtr,
	UniCharArrayOffset		inTextOffset,
	UniCharCount			inTextLength,
	UniCharCount			inTextTotalLength,
	const LATSUIStyle&		inStyle)
{
	ATSUStyle	theStyle = inStyle.Get();

	ThrowIfOSStatus_( ::ATSUCreateTextLayoutWithTextPtr(
								inTextPtr,
								inTextOffset,
								inTextLength,
								inTextTotalLength,
								1,
								(UniCharCount*) &inTextLength,
								&theStyle,
								&mTextLayout ) );
}


// ---------------------------------------------------------------------------
//	¥ ~LATSUITextLayout						Destructor				  [public]
// ---------------------------------------------------------------------------

LATSUITextLayout::~LATSUITextLayout()
{
	::ATSUDisposeTextLayout(mTextLayout);
}


// ---------------------------------------------------------------------------
//	¥ SetTextPointer
// ---------------------------------------------------------------------------

void
LATSUITextLayout::SetTextPointer(
	ConstUniCharArrayPtr	inTextPtr,
	UniCharArrayOffset		inTextOffset,
	UniCharCount			inTextLength,
	UniCharCount			inTextTotalLength)
{
	ThrowIfOSStatus_( ::ATSUSetTextPointerLocation( mTextLayout,
													inTextPtr,
													inTextOffset,
													inTextLength,
													inTextTotalLength) );
}


// ---------------------------------------------------------------------------
//	¥ SetOneLayoutControl											  [public]
// ---------------------------------------------------------------------------

void
LATSUITextLayout::SetOneLayoutControl(
	ATSUAttributeTag		inTag,
	ByteCount				inDataSize,
	ATSUAttributeValuePtr	inValuePtr)
{
	ThrowIfOSStatus_( ::ATSUSetLayoutControls( mTextLayout, 1,
												&inTag,
												&inDataSize,
												&inValuePtr) );
}


// ---------------------------------------------------------------------------
//	¥ SetMultipleLayoutControls										  [public]
// ---------------------------------------------------------------------------

void
LATSUITextLayout::SetMultipleLayoutControls(
	ItemCount					inCount,
	const ATSUAttributeTag		inTags[],
	const ByteCount				inValueSizes[],
	const ATSUAttributeValuePtr	inValues[])
{
	ThrowIfOSStatus_( ::ATSUSetLayoutControls( mTextLayout, inCount,
												inTags,
												inValueSizes,
												inValues) );
}


// ---------------------------------------------------------------------------
//	¥ ClearOneLayoutControl											  [public]
// ---------------------------------------------------------------------------

void
LATSUITextLayout::ClearOneLayoutControl(
	ATSUAttributeTag		inTag)
{
	ThrowIfOSStatus_( ::ATSUClearLayoutControls(mTextLayout, 1, &inTag) );
}


// ---------------------------------------------------------------------------
//	¥ ClearAllLayoutControls										  [public]
// ---------------------------------------------------------------------------

void
LATSUITextLayout::ClearAllLayoutControls()
{
	ThrowIfOSStatus_( ::ATSUClearLayoutControls(mTextLayout, kATSUClearAll, nil) );
}


// ---------------------------------------------------------------------------
//	¥ SetLineWidth													  [public]
// ---------------------------------------------------------------------------

void
LATSUITextLayout::SetLineWidth(
	ATSUTextMeasurement	inLineWidth)
{
	SetOneLayoutControl(kATSULineWidthTag, sizeof(ATSUTextMeasurement), &inLineWidth);
}


// ---------------------------------------------------------------------------
//	¥ SetRotation													  [public]
// ---------------------------------------------------------------------------

void
LATSUITextLayout::SetRotation(
	Fixed	inRotation)
{
	SetOneLayoutControl(kATSULineRotationTag, sizeof(Fixed), &inRotation);
}


// ---------------------------------------------------------------------------
//	¥ SetFlushFactor												  [public]
// ---------------------------------------------------------------------------

void
LATSUITextLayout::SetFlushFactor(
	Fixed	inFlushFactor)
{
	SetOneLayoutControl(kATSULineFlushFactorTag, sizeof(Fixed), &inFlushFactor);
}


// ---------------------------------------------------------------------------
//	¥ SetTruncation													  [public]
// ---------------------------------------------------------------------------

void
LATSUITextLayout::SetTruncation(
	ATSULineTruncation	inTruncation)
{
	SetOneLayoutControl(kATSULineTruncationTag, sizeof(ATSULineTruncation), &inTruncation);
}


// ---------------------------------------------------------------------------
//	¥ SetCGContext													  [public]
// ---------------------------------------------------------------------------

void
LATSUITextLayout::SetCGContext(
	CGContextRef	inCGContext)
{
	#if TARGET_API_MAC_CARBON
	
		SetOneLayoutControl( kATSUCGContextTag,
							 sizeof(CGContextRef),
							 &inCGContext );
	
	#else							// CGContext not supported on Classic
		#pragma unused(inCGContext)
	#endif
}


// ---------------------------------------------------------------------------
//	¥ SetRunStyle													  [public]
// ---------------------------------------------------------------------------

void
LATSUITextLayout::SetRunStyle(
	const LATSUIStyle&	inStyle,
	UniCharArrayOffset	inRunStart,
	UniCharCount		inRunLength)
{
	ThrowIfOSStatus_( ::ATSUSetRunStyle(
							mTextLayout, inStyle, inRunStart, inRunLength) );
}


// ---------------------------------------------------------------------------
//	¥ DetermineLineBreaks											  [public]
// ---------------------------------------------------------------------------
//	Set soft line breaks for the text using the specified line length

void
LATSUITextLayout::DetermineLineBreaks(
	ATSUTextMeasurement		inLineWidth)
{
	SetLineWidth(inLineWidth);

	UniCharArrayOffset	lineStart;
	UniCharArrayOffset	lineBreak = 0;
	
	do {
		lineStart = lineBreak;
	
		::ATSUBreakLine( mTextLayout,
						 lineStart,
						 inLineWidth,
						 true,
						 &lineBreak );
	} while (lineBreak > lineStart);
}


// ---------------------------------------------------------------------------
//	¥ MeasureLines
// ---------------------------------------------------------------------------
//	Pass back the number of lines, line break offsets, and line heights for
//	the text in a layout.
//
//	Function allocates outLineBreaks and outLineHeights as arrays using
//	new[]. Caller is resposible for deleting these arrays (remember to
//	use delete[] instead of delete).
//
//	You may pass the line breaks and heights to the DrawLinesAt() function
//	to draw the text.

void
LATSUITextLayout::MeasureLines(
	ATSUTextMeasurement		inLineWidth,
	ItemCount&				outLineCount,
	UniCharArrayOffset*&	outLineBreaks,
	ATSUTextMeasurement*&	outLineHeights)
{
									// Set soft line breaks
	DetermineLineBreaks(inLineWidth);
	
	OSStatus	status;
							
	ItemCount	breakCount;			// Get number of soft line breaks

	status = ::ATSUGetSoftLineBreaks( mTextLayout, kATSUFromTextBeginning,
												   kATSUToTextEnd,
												   0,
												   nil,
												   &breakCount );
												   
									// Allocate and fill array of soft line
									//   break offsets
	outLineBreaks = new UniCharArrayOffset[breakCount];
	
	status = ::ATSUGetSoftLineBreaks( mTextLayout, kATSUFromTextBeginning,
												   kATSUToTextEnd,
												   breakCount,
												   outLineBreaks,
												   &breakCount );
	
	outLineCount = breakCount + 1;	// Lines of text is one more than the
									//   number of line breaks
									
									// Allocate array to store line heights
	outLineHeights = new ATSUTextMeasurement[outLineCount];
	
									// Measure each line of text, storing
									//   the height in our array
	UniCharArrayOffset	lineStart  = 0;
	UniCharCount		lineLength = 0;
	
	for (ItemCount i = 0; i <= breakCount; i++) {
	
		if (i == breakCount) {
			lineLength = kATSUToTextEnd;
		} else {
			lineLength = outLineBreaks[i] - lineStart;
		}

		ATSUTextMeasurement		before;
		ATSUTextMeasurement		after;
		ATSUTextMeasurement		ascent;
		ATSUTextMeasurement		descent;
		
		::ATSUMeasureText( mTextLayout, lineStart,
										lineLength,
										&before,
										&after,
										&ascent,
										&descent );
						   
						   			// Line height is the ascent plus descent
		outLineHeights[i] = ascent + descent;
				
		lineStart = outLineBreaks[i];
	}
}



// ---------------------------------------------------------------------------
//	¥ DrawLinesAt													  [public]
// ---------------------------------------------------------------------------
//	Draw multiple lines of text using saved line breaks and heights

void
LATSUITextLayout::DrawLinesAt(
	ItemCount				inLineCount,
	UniCharArrayOffset*		inLineBreaks,
	ATSUTextMeasurement*	inLineHeights,
	ATSUTextMeasurement		inLeftCoord,
	ATSUTextMeasurement		inTopCoord) const
{
	ItemCount	breakCount = inLineCount - 1;
	
	UniCharArrayOffset	lineStart	= 0;
	UniCharCount		lineLength	= 0;
	ATSUTextMeasurement	vert		= inTopCoord;
	
	LATSUCoordinates	coords(mTextLayout, UQDGlobals::GetCurrentPort());

	for (ItemCount i = 0; i <= breakCount; i++) {
	
		if (i == breakCount) {
			lineLength = kATSUToTextEnd;
		} else {
			lineLength = inLineBreaks[i] - lineStart;
		}
						   
		vert += inLineHeights[i];
		
		ATSUTextMeasurement	horizCoord;
		ATSUTextMeasurement	vertCoord;
		coords.QDToATSU(inLeftCoord, vert, horizCoord, vertCoord);
		
		::ATSUDrawText( mTextLayout, lineStart,
									 lineLength,
									 horizCoord,
									 vertCoord);
				
		lineStart = inLineBreaks[i];
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawOneLineAt													  [public]
// ---------------------------------------------------------------------------
//	Draw a single line of text

void
LATSUITextLayout::DrawOneLineAt(
	UniCharArrayOffset		inLineOffset,
	UniCharCount			inLineLength,
	ATSUTextMeasurement		inHoriz,
	ATSUTextMeasurement		inVert) const
{
	LATSUCoordinates	coords(mTextLayout, UQDGlobals::GetCurrentPort());
	ATSUTextMeasurement	horizCoord;
	ATSUTextMeasurement	vertCoord;
	coords.QDToATSU(inHoriz, inVert, horizCoord, vertCoord);

	::ATSUDrawText(mTextLayout, inLineOffset, inLineLength, horizCoord, vertCoord);
}


// ---------------------------------------------------------------------------
//	¥ DrawInBox														  [public]
// ---------------------------------------------------------------------------
//	Draw text wrapped to a QuickDraw rectangle
//
//	This function is similar to TETextBox and DrawThemeTextBox. However,
//	it does not clip the text to the box. Caller should set the appropriate
//	clipping region beforehand.
//
//	Function calculates line breaks and line heights on every call. This
//	could be slow, depending on the amount of text and the text formatting.
//	If that is a concern, you can use MeasureLines() to cache the line
//	breaks and heights and then call DrawLinesAt() to draw the text.

void
LATSUITextLayout::DrawInBox(
	const Rect&		inBox)
{
	OSStatus	status;
	
	DetermineLineBreaks(IntToFixed(inBox.right - inBox.left));
	
	ItemCount	breakCount;

	status = ::ATSUGetSoftLineBreaks( mTextLayout, kATSUFromTextBeginning,
												   kATSUToTextEnd,
												   0,
												   nil,
												   &breakCount );

	UniCharArrayOffset*	lineBreaks = new UniCharArrayOffset[breakCount];
	
	status = ::ATSUGetSoftLineBreaks( mTextLayout, kATSUFromTextBeginning,
												   kATSUToTextEnd,
												   breakCount,
												   lineBreaks,
												   &breakCount );
	
	UniCharArrayOffset	lineStart = 0;
	UniCharCount		lineLength = 0;
	ATSUTextMeasurement	horiz	= IntToFixed(inBox.left);
	ATSUTextMeasurement	vert	= IntToFixed(inBox.top);
	
	LATSUCoordinates	coords(mTextLayout, UQDGlobals::GetCurrentPort());
	
	for (ItemCount i = 0; i <= breakCount; i++) {
	
		if (i == breakCount) {
			lineLength = kATSUToTextEnd;
		} else {
			lineLength = lineBreaks[i] - lineStart;
		}

		ATSUTextMeasurement		before;
		ATSUTextMeasurement		after;
		ATSUTextMeasurement		ascent;
		ATSUTextMeasurement		descent;
		
		::ATSUMeasureText( mTextLayout, lineStart,
										lineLength,
										&before,
										&after,
										&ascent,
										&descent );
						   
		vert += ascent + descent;
		
		ATSUTextMeasurement		horizCoord;
		ATSUTextMeasurement		vertCoord;
		coords.QDToATSU(horiz, vert, horizCoord, vertCoord);
		
			
		status = ::ATSUDrawText( mTextLayout, lineStart,
											  lineLength,
											  horizCoord,
											  vertCoord);
				
		lineStart = lineBreaks[i];
	}
	
	delete[] lineBreaks;
}

#pragma mark -
// ===========================================================================
//	LATSUCoordinates
// ===========================================================================
//	Helper class for converting from QuickDraw coordinates to Core Graphics
//	coordinates as used by ATSU
//
//	QuickDraw coords are 16-bit integers with the vertical coordinate
//	increasing down. The default origin is at the top left of the port,
//	but PowerPlant changes the origin to give each View its own local
//	coordinate space.
//
//	Core Graphics coords are floats with the vertical coordinate increasing
//	up (opposite of QuickDraw). The default origin is at the bottom left
//	of the port, and presently we assume it remains there.
//
//	ATSU coords are fixed numbers. If the text layout has CGContext, it
//	uses the CG origin in the lower left. Otherwise, it uses the QuickDraw
//	origin.

// ---------------------------------------------------------------------------
//	¥ LATSUCoordinates						Constructor				  [public]
// ---------------------------------------------------------------------------

LATSUCoordinates::LATSUCoordinates(
	ATSUTextLayout	inLayout,
	GrafPtr			inPort)
{
	mHasCGContext = false;
	mHorizOffset  = 0;
	mVertOffset   = 0;

	#if TARGET_API_MAC_CARBON
	
		CGContextRef	context = nil;
		ByteCount		valueSize;
		
		::ATSUGetLayoutControl( inLayout, kATSUCGContextTag,
										  sizeof(CGContextRef),
										  &context,
										  &valueSize );
											 
		if (context != nil) {
			mHasCGContext = true;
			
			Rect	portBounds;
			::GetPortBounds(inPort, &portBounds);
			
			mHorizOffset = IntToFixed(portBounds.left);
			mVertOffset  = IntToFixed(portBounds.bottom);
		}
		
	#else
		#pragma unused(inLayout, inPort)
	#endif
}


// ---------------------------------------------------------------------------
//	¥ ~LATSUCoordinates						Destructor				  [public]
// ---------------------------------------------------------------------------

LATSUCoordinates::~LATSUCoordinates()
{
}


// ---------------------------------------------------------------------------
//	¥ QDToATSU														  [public]
// ---------------------------------------------------------------------------
//	Convert from QuickDraw space (vertical increasing down) to the local
//	coordinates of an ATSU layout

void
LATSUCoordinates::QDToATSU(
	ATSUTextMeasurement		inQDHoriz,
	ATSUTextMeasurement		inQDVert,
	ATSUTextMeasurement&	outATSUHoriz,
	ATSUTextMeasurement&	outATSUVert)
{
	if (mHasCGContext) {				// With a CGContext, we must offset
										//   the coordinates to account for
										//   the different origins and
										//   vertical direction
		outATSUHoriz = inQDHoriz - mHorizOffset;
		outATSUVert  = mVertOffset  - inQDVert;
		
	} else {							// No CGContext. No transform.
		outATSUHoriz = inQDHoriz;
		outATSUVert  = inQDVert;
	}
}


PP_End_Namespace_PowerPlant
