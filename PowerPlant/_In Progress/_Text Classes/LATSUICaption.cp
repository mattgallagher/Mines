// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LATSUICaption.cp			PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LATSUICaption.h>

#include <LCFString.h>
#include <LStream.h>
#include <UEnvironment.h>
#include <UTextTraits.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LATSUICaption							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LATSUICaption::LATSUICaption()
{
	mTextPtr			= nil;
	mTextLength			= 0;
	mBeforeLocation		= 0;
	mBaselineLocation	= 0;
}


// ---------------------------------------------------------------------------
//	¥ LATSUICaption							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LATSUICaption::LATSUICaption(
	const SPaneInfo&	inPaneInfo,
	ConstStringPtr		inString)
	
	: LPane(inPaneInfo)
{
	LCFString	str(inString);
	
	mTextLength			= (UniCharCount) str.GetLength();
	mTextPtr			= new UniChar[mTextLength];
	mBeforeLocation		= 0;
	mBaselineLocation	= 0;
	
	str.GetSubstring(CFRangeMake(0, (CFIndex) mTextLength), mTextPtr);
	
	PrepareTextLayout();
}


// ---------------------------------------------------------------------------
//	¥ LATSUICaption							Stream Constructor		  [public]
// ---------------------------------------------------------------------------
//	$$$ For now, we use the same PPob data as LCaption. So you can create
//	a LATSUICaption in Constructor by maing a LCaption and changing the
//	the Class ID to 'ucap'.

LATSUICaption::LATSUICaption(
	LStream*	inStream)
	
	: LPane(inStream)
{
	Str255	str;
	ResIDT	textTraitsID;
	
	*inStream >> str;
	*inStream >> textTraitsID;
	
	LCFString	cfStr(str);
	
	mTextLength			= (UniCharCount) cfStr.GetLength();
	mTextPtr			= new UniChar[mTextLength];
	cfStr.GetSubstring(CFRangeMake(0, (CFIndex) mTextLength), mTextPtr);
	
	mBeforeLocation		= 0;
	mBaselineLocation	= 0;
	
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
//	¥ ~LATSUICaption						Destructor				  [public]
// ---------------------------------------------------------------------------

LATSUICaption::~LATSUICaption()
{
	delete[] mTextPtr;
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf											   [protected]
// ---------------------------------------------------------------------------

void
LATSUICaption::FinishCreateSelf()
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
LATSUICaption::SetDescriptor(
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
LATSUICaption::SetCFDescriptor(
	CFStringRef	inString)
{
	UniCharCount	textLength	= (UniCharCount) ::CFStringGetLength(inString);
	UniChar*		textPtr		= new UniChar[textLength];
	::CFStringGetCharacters(inString, CFRangeMake(0, (CFIndex) textLength), textPtr);
	
	delete[] mTextPtr;
	
	mTextLength = textLength;
	mTextPtr	= textPtr;
	
	PrepareTextLayout();
	Refresh();
}

#endif


// ---------------------------------------------------------------------------
//	¥ DrawSelf													   [protected]
// ---------------------------------------------------------------------------

void
LATSUICaption::DrawSelf()
{
	Rect	frame;
	CalcLocalFrameRect(frame);

	ApplyForeAndBackColors();
	
	mTextLayout.DrawOneLineAt( kATSUFromTextBeginning,
							   kATSUToTextEnd,
							   IntToFixed(frame.left) - mBeforeLocation,
							   IntToFixed(frame.top) + mBaselineLocation );
}


// ---------------------------------------------------------------------------
//	¥ PrepareTextLayout											   [protected]
// ---------------------------------------------------------------------------

void
LATSUICaption::PrepareTextLayout()
{
	mTextLayout.SetTextPointer(mTextPtr, 0, mTextLength, mTextLength);
	
	mTextLayout.SetRunStyle(mStyle, 0, mTextLength);

	ATSUTextMeasurement		after;
	ATSUTextMeasurement		ascent;
	ATSUTextMeasurement		descent;
	
	::ATSUMeasureText( mTextLayout,
					   kATSUFromTextBeginning,
					   kATSUToTextEnd,
					   &mBeforeLocation,
					   &after,
					   &ascent,
					   &descent );
					   
	if (mBaselineLocation <= 0) {	// Use text ascent as baseline location
		mBaselineLocation = ascent;
	}
}


PP_End_Namespace_PowerPlant
