// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LATSUITextBox.cp			PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LATSUITextBox.h>

#include <LCFString.h>
#include <LStream.h>
#include <UTextTraits.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	$$$ Testing
//	Option for whether or not to cache the line breaks and heights
//
//		Depending on the text (number of chars, fonts used, styles, etc.),
//		calculating line breaks and heights might be slow

#ifndef LATSUITextBox_Cache_Line_Info
	#define	LATSUITextBox_Cache_Line_Info	1
#endif


// ---------------------------------------------------------------------------
//	¥ LATSUITextBox							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LATSUITextBox::LATSUITextBox()
{
	mTextPtr	= nil;
	mTextLength	= 0;
	
	mLineCount   = 0;
	mLineBreaks  = nil;
	mLineHeights = nil;
}


// ---------------------------------------------------------------------------
//	¥ LATSUITextBox							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LATSUITextBox::LATSUITextBox(
	const SPaneInfo&	inPaneInfo,
	ConstStringPtr		inString)
	
	: LPane(inPaneInfo)
{
	LCFString	str(inString);
	
	mTextLength	= (UniCharCount) str.GetLength();
	mTextPtr	= new UniChar[mTextLength];
	
	mLineCount   = 0;
	mLineBreaks  = nil;
	mLineHeights = nil;
	
	str.GetSubstring(CFRangeMake(0, (SInt32) mTextLength), mTextPtr);
	
	PrepareTextLayout();
}


// ---------------------------------------------------------------------------
//	¥ LATSUITextBox							Stream Constructor		  [public]
// ---------------------------------------------------------------------------
//	$$$ For now, we use the same PPob data as LCaption. So you can create
//	a LATSUITextBox in Constructor by maing a LCaption and changing the
//	the Class ID to 'utbx'.

LATSUITextBox::LATSUITextBox(
	LStream*	inStream)
	
	: LPane(inStream)
{
	Str255	str;
	ResIDT	textTraitsID;
	
	*inStream >> str;
	*inStream >> textTraitsID;
	
	LCFString	cfStr(str);
	
	mTextLength	= (UniCharCount) cfStr.GetLength();
	mTextPtr	= new UniChar[mTextLength];
	cfStr.GetSubstring(CFRangeMake(0, (SInt32) mTextLength), mTextPtr);
	
	mLineCount   = 0;
	mLineBreaks  = nil;
	mLineHeights = nil;
	
		// Set up ATSUI Style from Text Traits info
		
		// $$$ You must set the font name in the Text Traits.
		// Don't use the System or Application font options.
	
	TextTraitsRecord	traits;
	UTextTraits::LoadTextTraits(textTraitsID, traits);
	
	ATSUAttributeTag	tags[8] = { kATSUFontTag,
									kATSUSizeTag,
									kATSUQDBoldfaceTag,
									kATSUQDItalicTag,
									kATSUQDUnderlineTag,
									kATSUQDCondensedTag,
									kATSUQDExtendedTag,
									kATSUColorTag };
	
	ByteCount			sizes[8] = { sizeof(ATSUFontID),
									 sizeof(Fixed),
									 sizeof(Boolean),
									 sizeof(Boolean),
									 sizeof(Boolean),
									 sizeof(Boolean),
									 sizeof(Boolean),
									 sizeof(RGBColor) };
	
	ATSUFontID		fontID;
	Fixed			fontSize;
	Boolean			isBold;
	Boolean			isItalic;
	Boolean			isUnderline;
	Boolean			isCondensed;
	Boolean			isExtended;
	
	ATSUAttributeValuePtr	values[8] = { &fontID,
										  &fontSize,
										  &isBold,
										  &isItalic,
										  &isUnderline,
										  &isCondensed,
										  &isExtended,
										  &traits.color };
	
	fontSize	= IntToFixed(traits.size);
	isBold		= (traits.style & bold) != 0;
	isItalic	= (traits.style & italic) != 0;
	isUnderline	= (traits.style & underline) != 0;
	isCondensed	= (traits.style & condense) != 0;
	isExtended	= (traits.style & extend) != 0;
	
	::ATSUFindFontFromName( (Ptr) (traits.fontName + 1),
							traits.fontName[0],
							kFontFullName,
							kFontMacintoshPlatform,
							kFontRomanScript,
							kFontNoLanguageCode,
							&fontID );
							
	mStyle.SetMultipleAttributes(8, tags, sizes, values);
	
	Fixed	flushFactor;
	
	switch (traits.justification) {
			
		case teCenter:
			flushFactor = kATSUCenterAlignment;
			break;
			
		case teFlushRight:
			flushFactor = kATSUEndAlignment;
			break;
	
		default:
			flushFactor = kATSUStartAlignment;
			break;
	}
	
	mTextLayout.SetFlushFactor(flushFactor);
	
	PrepareTextLayout();
}


// ---------------------------------------------------------------------------
//	¥ ~LATSUITextBox						Destructor				  [public]
// ---------------------------------------------------------------------------

LATSUITextBox::~LATSUITextBox()
{
	delete[] mTextPtr;
	delete[] mLineBreaks;
	delete[] mLineHeights;
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf											   [protected]
// ---------------------------------------------------------------------------

void
LATSUITextBox::FinishCreateSelf()
{
									// Use CoreGraphics if present
	CGContextRef	context = GetCGContext();

	if (context != nil) {
		mTextLayout.SetCGContext(context);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------

void
LATSUITextBox::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
	LCFString	str(inDescriptor);
	
	SetCFDescriptor(str);
}

#if PP_Uses_CFDescriptor

// ---------------------------------------------------------------------------
//	¥ SetCFDescriptor												  [public]
// ---------------------------------------------------------------------------

void
LATSUITextBox::SetCFDescriptor(
	CFStringRef	inString)
{
	UniCharCount	textLength	= (UniCharCount) ::CFStringGetLength(inString);
	UniChar*		textPtr		= new UniChar[textLength];
	::CFStringGetCharacters(inString, CFRangeMake(0, (CFIndex) textLength), textPtr);
	
	delete[] mTextPtr;
	delete[] mLineBreaks;
	delete[] mLineHeights;
	
	mTextLength = textLength;
	mTextPtr	= textPtr;
	
	mLineCount   = 0;
	mLineBreaks  = nil;
	mLineHeights = nil;
	
	PrepareTextLayout();
	Refresh();
}

#endif

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf													   [protected]
// ---------------------------------------------------------------------------

void
LATSUITextBox::DrawSelf()
{
	Rect	frame;
	CalcLocalFrameRect(frame);

	ApplyForeAndBackColors();
	
	::PenNormal();
	::FrameRect(&frame);
	
	#if LATSUITextBox_Cache_Line_Info
	
	mTextLayout.DrawLinesAt( mLineCount, mLineBreaks, mLineHeights,
							 IntToFixed(frame.left), IntToFixed(frame.top) );
							 
	#else
	
	mTextLayout.DetermineLineBreaks(IntToFixed(frame.right - frame.left));
	mTextLayout.DrawInBox(frame);
	
	#endif
}


// ---------------------------------------------------------------------------
//	¥ PrepareTextLayout											   [protected]
// ---------------------------------------------------------------------------

void
LATSUITextBox::PrepareTextLayout()
{
	mTextLayout.SetTextPointer(mTextPtr, 0, mTextLength, mTextLength);
	
	mTextLayout.SetRunStyle(mStyle, 0, mTextLength);
	
	Rect	frame;
	CalcLocalFrameRect(frame);
	
	#if LATSUITextBox_Cache_Line_Info

	mTextLayout.MeasureLines( IntToFixed(frame.right - frame.left),
							  mLineCount, mLineBreaks, mLineHeights );
	
	#endif
}


PP_End_Namespace_PowerPlant
