// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMStaticTextImp.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LAMStaticTextImp.h>
#include <LStream.h>
#include <LString.h>
#include <UDrawingState.h>
#include <UGraphicUtils.h>
#include <UQDOperators.h>
#include <UTextTraits.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LAMStaticTextImp			Stream/Default Constructor	  [public]
// ---------------------------------------------------------------------------
//	Do not access inStream. It may be nil. Read Stream data in Init()

LAMStaticTextImp::LAMStaticTextImp(
	LStream*	inStream)
	
	: LAMControlImp(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ Init									Parameterized Initializer [public]
// ---------------------------------------------------------------------------

void
LAMStaticTextImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			inRefCon)
{
	LAMControlImp::Init(inControlPane, inControlKind, Str_Empty,
						inTextTraitsID, inRefCon);
						
	SetDataTag(0, kControlStaticTextTextTag, inTitle[0], (Ptr)(inTitle + 1));
}


// ---------------------------------------------------------------------------
//	¥ ~LAMStaticTextImp				Destructor				  [public]
// ---------------------------------------------------------------------------

LAMStaticTextImp::~LAMStaticTextImp()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------
//	Pass back contents of StaticText as a string
//
//	Unlike other controls, StaticText accesses its contents via a data tag
//	and does not use its title

StringPtr
LAMStaticTextImp::GetDescriptor(
	Str255		outDescriptor) const
{
	Size	textSize;
	GetDataTag(0, kControlStaticTextTextTag, 255, outDescriptor + 1,
				&textSize);
	
	outDescriptor[0] = 255;
	if (textSize < 255) {
		outDescriptor[0] = (UInt8) textSize;
	}
	
	return outDescriptor;
}
		

// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------
//	Set contents of StaticText from a string
//
//	Unlike other controls, StaticText accesses its contents via a data tag
//	and does not use its title

void
LAMStaticTextImp::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
	SetDataTag(0, kControlStaticTextTextTag, inDescriptor[0],
					(Ptr)(inDescriptor + 1));
	Refresh();
}

#if PP_Uses_CFDescriptor

// ---------------------------------------------------------------------------
//	¥ CopyCFDescriptor												  [public]
// ---------------------------------------------------------------------------
//	Pass back contents of StaticText as a CF string
//
//	Unlike other controls, StaticText accesses its contents via a data tag
//	and does not use its title

CFStringRef
LAMStaticTextImp::CopyCFDescriptor() const
{
	CFStringRef	stringRef = nil;

		// Try to get the text as a CFString. This may fail because
		// not all systems support the necessary data tag

	OSErr	err = ::GetControlData( mMacControlH,
									kControlEntireControl,
									kControlStaticTextCFStringTag,
									sizeof(CFStringRef),
									&stringRef,
									nil );
									
	if (err != noErr) {
	
			// Can't get a CFString directly. We need to get the
			// text as regular characters and create a CFString.
	
		Size	textLen;			// Get length of text
	
		err = ::GetControlDataSize( mMacControlH,
									kControlEntireControl,
									kControlStaticTextTextTag,
									&textLen );
									
		ThrowIfOSErr_(err);
									// Copy text into an array
		UInt8*	textBuffer = new UInt8[textLen];
		
		err = ::GetControlData( mMacControlH,
								kControlEntireControl,
								kControlStaticTextTextTag,
								textLen,
								textBuffer,
								nil );
								
		if (err == noErr) {			// Make a CFString from the text
			stringRef = ::CFStringCreateWithBytes( nil,
												   textBuffer,
												   (CFIndex) textLen,
												   ::CFStringGetSystemEncoding(),
												   false );
		}
		
		delete[] textBuffer;
		
		ThrowIfOSErr_(err);
	}
	
	return stringRef;
}


// ---------------------------------------------------------------------------
//	¥ SetCFDescriptor												  [public]
// ---------------------------------------------------------------------------
//	Set contents of StaticText from a CF string
//
//	Unlike other controls, StaticText accesses its contents via a data tag
//	and does not use its title

void
LAMStaticTextImp::SetCFDescriptor(
	CFStringRef	inStringRef)
{
		// Try to set text using the CFString. This may fail because
		// not all systems support the necessary data tag
	
	OSErr err = ::SetControlData( mMacControlH,
								  kControlEntireControl,
								  kControlStaticTextCFStringTag,
								  sizeof(CFStringRef),
								  &inStringRef );
								  
	if (err != noErr) {
	
			// Failed to set text using CFString. Extract text
			// as regular (non Unicode) characters.
	
		CFIndex	strLen	  = ::CFStringGetLength(inStringRef);
		CFIndex	bufferLen = 2 * strLen;		// Allow for double-byte chars
		CFIndex	textLen;
		
		UInt8*	textBuffer = new UInt8[bufferLen];
		
		::CFStringGetBytes( inStringRef,
							::CFRangeMake(0, strLen),
							::CFStringGetSystemEncoding(),
							0, false,
							textBuffer,
							bufferLen,
							&textLen );
		
		err = ::SetControlData( mMacControlH,
								kControlEntireControl,
								kControlStaticTextTextTag,
								textLen,
								textBuffer );
								
		delete[] textBuffer;
								
		SignalIfOSErr_(err);
		
		mControlPane->Refresh();
	}
}

#endif

// ---------------------------------------------------------------------------
//	¥ GetValue														  [public]
// ---------------------------------------------------------------------------

bool
LAMStaticTextImp::GetValue(
	SInt32		&outValue) const
{
	LStr255	textStr;
	GetDescriptor(textStr);
	outValue = (SInt32) textStr;
	return true;
}
		

// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LAMStaticTextImp::SetValue(
	SInt32		inValue)
{
	LStr255	textStr = inValue;
	SetDescriptor(textStr);
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LAMStaticTextImp::DrawSelf()
{
	// [Workaround] Appearance Manager feature/bug
	//
	// Some configurations of System/AM version don't lighten the text
	// when inactive if the color is not black. As a workaround, we
	// explicitly set the font style with the proper color before drawing.
	
	TextTraitsH		traitsH = UTextTraits::LoadTextTraits(mTextTraitsID);
		
	if (traitsH != nil) {
	
		ControlFontStyleRec	styleRec;
		
		styleRec.foreColor = (**traitsH).color;
		
		if (styleRec.foreColor != Color_Black) {
		
			styleRec.flags	= kControlUseFontMask +
								kControlUseFaceMask +
								kControlUseSizeMask +
								kControlUseModeMask +
								kControlUseJustMask +
								kControlUseForeColorMask;

			styleRec.font	= (**traitsH).fontNumber;
			styleRec.size	= (**traitsH).size;
			styleRec.style	= (**traitsH).style;
			styleRec.mode	= (**traitsH).mode;
			styleRec.just	= (**traitsH).justification;
				
			if (not IsActive() or not IsEnabled()) {
				styleRec.foreColor = UGraphicUtils::Lighten(styleRec.foreColor);
			}
				
			SetFontStyle(styleRec);
		}
	}

	LAMControlImp::DrawSelf();
}


PP_End_Namespace_PowerPlant
