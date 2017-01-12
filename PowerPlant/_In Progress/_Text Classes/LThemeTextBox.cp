// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LThemeTextBox.cp			PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LThemeTextBox.h>
#include <LStream.h>
#include <LString.h>
#include <UEnvironment.h>
#include <UTextTraits.h>

#include <CFString.h>
#include <MacTextEditor.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LThemeTextBox							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LThemeTextBox::LThemeTextBox()
{
	mThemeFontID = kThemeSystemFont;
	mTextTraitsID = 0;
	mDimWhenInactive = true;
	mDimWhenDisabled = true;
}


// ---------------------------------------------------------------------------
//	¥ LThemeTextBox							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LThemeTextBox::LThemeTextBox(
	const SPaneInfo&	inPaneInfo,
	ConstStringPtr		inString,
	ThemeFontID			inThemeFontID)
	
	: LPane(inPaneInfo),
	  mString(inString)
{
	mThemeFontID = inThemeFontID;
	mTextTraitsID = 0;
	mDimWhenInactive = true;
	mDimWhenDisabled = true;
}


// ---------------------------------------------------------------------------
//	¥ LThemeTextBox							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LThemeTextBox::LThemeTextBox(
	LStream*	inStream)
	
	: LPane(inStream)
{
	LStr255		str;
	Boolean		boolValue;

	*inStream >> (StringPtr) str;
	*inStream >> mTextTraitsID;
	*inStream >> mThemeFontID;
	
	*inStream >> boolValue;
	mDimWhenInactive = boolValue;
	
	*inStream >> boolValue;
	mDimWhenDisabled = boolValue;
	
	mString.Append(ConstStringPtr(str));
}


// ---------------------------------------------------------------------------
//	¥ ~LThemeTextBox						Destructor				  [public]
// ---------------------------------------------------------------------------

LThemeTextBox::~LThemeTextBox()
{
}


// ---------------------------------------------------------------------------
//	¥ ActivateSelf												   [protected]
// ---------------------------------------------------------------------------

void
LThemeTextBox::ActivateSelf()
{
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf											   [protected]
// ---------------------------------------------------------------------------

void
LThemeTextBox::DeactivateSelf()
{
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ EnableSelf												   [protected]
// ---------------------------------------------------------------------------

void
LThemeTextBox::EnableSelf()
{
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ DisableSelf												   [protected]
// ---------------------------------------------------------------------------

void
LThemeTextBox::DisableSelf()
{
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf													   [protected]
// ---------------------------------------------------------------------------

void
LThemeTextBox::DrawSelf()
{
	Rect	frame;
	CalcLocalFrameRect(frame);
	
	ApplyForeAndBackColors();

	if (CFM_AddressIsResolved_(::DrawThemeTextBox)) {
	
		if (mThemeFontID == kThemeCurrentPortFont) {
			UTextTraits::SetPortTextTraits(mTextTraitsID);
		}
		
		bool	dimmed = ( (mDimWhenInactive and not IsActive()) or
						   (mDimWhenDisabled and not IsEnabled()) );
	
		ThemeDrawState	drawState = kThemeStateActive;
		
		if (dimmed) {
			drawState = kThemeStateInactive;
		}
		
		SInt16	bitDepth;
		bool	hasColor;
		GetDeviceInfo(bitDepth, hasColor);
		
		RGBColor	textColor;
		
		GetEmbeddedTextColor(bitDepth, hasColor, not dimmed, textColor);
		
		::RGBForeColor(&textColor);

		::DrawThemeTextBox( mString,
							mThemeFontID,
							drawState,
							true,
							&frame,
							teJustLeft,
							nil );
							
	}
}


PP_End_Namespace_PowerPlant
