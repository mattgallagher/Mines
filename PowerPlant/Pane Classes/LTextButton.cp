// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTextButton.cp				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	A button that behaves like a radio button, but displays text
//	(like the sort… buttons in the Finder).

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTextButton.h>

#include <LStream.h>
#include <PP_Messages.h>
#include <PP_Resources.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UTBAccessors.h>

PP_Begin_Namespace_PowerPlant


// ———————————————————————————————————————————————————————————————————————————
//	• LTextButton							Default Constructor		  [public]
// ———————————————————————————————————————————————————————————————————————————

LTextButton::LTextButton()
{
	mTextTraitsID  = Txtr_SystemFont;
	mSelectedStyle = bold;
}


// ———————————————————————————————————————————————————————————————————————————
//	• LTextButton							Stream Constructor		  [public]
// ———————————————————————————————————————————————————————————————————————————

LTextButton::LTextButton(
	LStream*	inStream)

	: LControl(inStream)
{
	inStream->ReadPString(mText);

	*inStream >> mTextTraitsID;
	*inStream >> mSelectedStyle;
}


// ———————————————————————————————————————————————————————————————————————————
//	• ~LTextButton							Destructor				  [public]
// ———————————————————————————————————————————————————————————————————————————

LTextButton::~LTextButton()
{
}

#pragma mark -

// ———————————————————————————————————————————————————————————————————————————
//	• GetDescriptor													  [public]
// ———————————————————————————————————————————————————————————————————————————
//	Return the text of the TextButton

StringPtr
LTextButton::GetDescriptor(
	Str255	outDescriptor) const
{
	return LString::CopyPStr(mText, outDescriptor);
}


// ———————————————————————————————————————————————————————————————————————————
//	• SetDescriptor													  [public]
// ———————————————————————————————————————————————————————————————————————————
//	Reset title and update region

void
LTextButton::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
	mText = inDescriptor;
	Refresh();
}


// ———————————————————————————————————————————————————————————————————————————
//	• SetValue														  [public]
// ———————————————————————————————————————————————————————————————————————————
//	Reset button value, update region, and send message

void
LTextButton::SetValue(
	SInt32		inValue)
{
	SInt32 oldValue = mValue;

	LControl::SetValue(inValue);
	if (oldValue != inValue) {
		Refresh();
	}

	if (inValue == Button_On) {
		BroadcastMessage(msg_ControlClicked, this);
	}
}

#pragma mark -

// ———————————————————————————————————————————————————————————————————————————
//	• DrawSelf													   [protected]
// ———————————————————————————————————————————————————————————————————————————
//	Draw this text in with appropriate textstyle for selected status

void
LTextButton::DrawSelf()
{
	StTextState		origTextState;
	StColorPenState	origCPenState;

		// Configure the text state. If the button is selected,
		// modify the text style.

	SInt16 theJust = UTextTraits::SetPortTextTraits(mTextTraitsID);

	if (mValue != Button_Off) {
		GrafPtr currPort = UQDGlobals::GetCurrentPort();
		SInt16 currStyle = ::GetPortTextFace(currPort);
		::TextFace((SInt16) (currStyle ^ mSelectedStyle));
	}

	RGBColor	textColor;				// Must save text color
	::GetForeColor(&textColor);

	ApplyForeAndBackColors();			// Set the background color
	::RGBForeColor(&textColor);			// Restore text color

	Rect theFrame;
	CalcLocalFrameRect(theFrame);

	if (theJust == teFlushDefault) {
		theJust = ::GetSysDirection();
	}

	UTextDrawing::DrawWithJustification(mText.TextPtr(), mText.LongLength(),
										theFrame, theJust);
}

#pragma mark -

// ———————————————————————————————————————————————————————————————————————————
//	• HotSpotAction												   [protected]
// ———————————————————————————————————————————————————————————————————————————
//	Toggle between highlighted and plain states, depending on mouse location

void
LTextButton::HotSpotAction(
	SInt16		/* inHotSpot */,
	Boolean		inCurrInside,
	Boolean		inPrevInside)
{
	if (inCurrInside != inPrevInside) {
		FocusDraw();
		Rect	frame;
		CalcLocalFrameRect(frame);
		::MacInvertRect(&frame);
	}
}


// ———————————————————————————————————————————————————————————————————————————
//	• DoneTracking												   [protected]
// ———————————————————————————————————————————————————————————————————————————

void
LTextButton::DoneTracking(
	SInt16		/* inHotSpot */,
	Boolean		/* inGoodTrack */)
{
}	// Inherited function tries to unhilite. We don't need to.


// ———————————————————————————————————————————————————————————————————————————
//	• HotSpotResult												   [protected]
// ———————————————————————————————————————————————————————————————————————————
//	Update value and send message

void
LTextButton::HotSpotResult(
	SInt16	inHotSpot)
{
	if (mValue == Button_Off) {
		SetValue(Button_On);
	} else {
		HotSpotAction(inHotSpot, true, false);	// Undo hilighting
	}
}


PP_End_Namespace_PowerPlant
