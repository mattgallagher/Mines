// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInPlaceHostView.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LInPlaceHostView.h>
#include <LInPlaceEditField.h>

#include <LEditField.h>
#include <LStream.h>
#include <UDrawingUtils.h>
#include <UTextTraits.h>

#include <LFastArrayIterator.h>
#include <UNewTextDrawing.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LInPlaceHostView						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LInPlaceHostView::LInPlaceHostView(
	LStream* inStream)

	: LView(inStream)
{
	InitHostView();

						// Read stream data (matches edit field stream data).

	inStream->ReadPString(mText);

	*inStream >> mTextTraitsID;
	*inStream >> mMaxChars;
	*inStream >> mEditFieldAttributes;

	SInt8 keyFilterID;
	*inStream >> keyFilterID;

	mKeyFilter = UKeyFilters::SelectTEKeyFilter(keyFilterID);
}


// ---------------------------------------------------------------------------
//	¥ LInPlaceHostView						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LInPlaceHostView::LInPlaceHostView()
{
	InitHostView();
}


// ---------------------------------------------------------------------------
//	¥ ~LInPlaceHostView						Destructor				  [public]
// ---------------------------------------------------------------------------

LInPlaceHostView::~LInPlaceHostView()
{
	delete mEditField;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetDescriptor
// ---------------------------------------------------------------------------
//	Return the text that would be edited (if no IPE active)
//	or the current text (if active).

StringPtr
LInPlaceHostView::GetDescriptor(
	Str255 outDescriptor) const
{
	if (mEditField != nil) {
		return mEditField->GetDescriptor(outDescriptor);

	} else {
		return LString::CopyPStr(mText, outDescriptor);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor
// ---------------------------------------------------------------------------
//	Change the text value of this edit field.

void
LInPlaceHostView::SetDescriptor(
	ConstStringPtr inDescriptor)
{

	// Stash text for future drawing.

	mText = inDescriptor;
	Refresh();

	// Relay new text to in-place editor (if one exists).

	if (mEditField != nil)
		mEditField->SetDescriptor(inDescriptor);

	// Cache frame rect of new text for hit testing.

	UpdateCachedFrame();
}


// ---------------------------------------------------------------------------
//	¥ GetValue
// ---------------------------------------------------------------------------

SInt32
LInPlaceHostView::GetValue() const
{
    SInt32   theNumber = 0;
    Str255  str;

    GetDescriptor(str);
    if (str[0] > 0) {
        ::StringToNum(str, &theNumber);
    }

    return theNumber;
}


// ---------------------------------------------------------------------------
//	¥ SetValue
// ---------------------------------------------------------------------------

void
LInPlaceHostView::SetValue(
	SInt32	inValue)
{
    Str255  numberAsString;
    ::NumToString(inValue, numberAsString);
    SetDescriptor(numberAsString);
}


// ---------------------------------------------------------------------------
//	¥ SetValueMessage
// ---------------------------------------------------------------------------
//	Set the message which is broadcast when the in-place editor
//	detects a change in the value.

void
LInPlaceHostView::SetValueMessage(
	MessageT inValueMessage)
{
	mValueMessage = inValueMessage;
	if (mEditField != nil)
		mEditField->SetValueMessage(inValueMessage);
}


// ---------------------------------------------------------------------------
//	¥ IsHitBy
// ---------------------------------------------------------------------------
//	Return true if the in-place editor would be hit by this port location.

Boolean
LInPlaceHostView::IsHitBy(
	SInt32 inHorizPort,
	SInt32 inVertPort)
{
	// If we have an in-place editor, just use it.

	if (mEditField != nil) {
		return mEditField->IsHitBy(inHorizPort, inVertPort);
	}

	// Convert point to local coordinates.

	Point localPt;
	localPt.h = (SInt16) inHorizPort;
	localPt.v = (SInt16) inVertPort;
	PortToLocalPoint(localPt);

	// Compare with cached text frame rect.

	return (::MacPtInRect(localPt, &mTextLocalFrame) && IsEnabled());
}


// ---------------------------------------------------------------------------
//	¥ CalcPortTextRect
// ---------------------------------------------------------------------------
//	Return the location of the static text in port coordinates.

void
LInPlaceHostView::CalcPortTextRect(
	Rect& outFrame)
{
	outFrame = mTextLocalFrame;
	LocalToPortPoint(topLeft(outFrame));
	LocalToPortPoint(botRight(outFrame));
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf												[protected]
// ---------------------------------------------------------------------------
//	Draw the text value of this field unless we have actually
//	instantiated an edit field. Then the edit field draws the text instead.

void
LInPlaceHostView::DrawSelf()
{

	// If we have an edit field, it will draw the text.

	if (mEditField != nil)
		return;

	// Otherwise, we draw the text in exactly the same location as it would
	// appear when edited.

	Rect frame;
	CalcLocalFrameRect(frame);
	if (mEditFieldAttributes & editAttr_Box)
		::MacInsetRect(&frame, 3, 2);
	else
		::MacInsetRect(&frame, 1, 0);

	SInt16 just = UTextTraits::SetPortTextTraits(mTextTraitsID);

	RGBColor textColor;
	::GetForeColor(&textColor);

	ApplyForeAndBackColors();
	::RGBForeColor(&textColor);

	UTextDrawing::DrawWithJustification((Ptr) &mText[1], mText[0], frame, just);

}


// ---------------------------------------------------------------------------
//	¥ RemoveSubPane											[protected]
// ---------------------------------------------------------------------------
//	If the edit field is the pane being removed, cancel references to it.

void
LInPlaceHostView::RemoveSubPane(
	LPane* inSub)
{
	if (inSub == mEditField) {
		LStr255 value;
		mEditField->GetDescriptor(value);
		SetDescriptor(value);
		mEditField = nil;
	}

	LView::RemoveSubPane(inSub);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ClickSelf												[protected]
// ---------------------------------------------------------------------------
//	Start in-place editing by defualt. You may want to override this to
//	add a delay before editing (like the Finder).

void
LInPlaceHostView::ClickSelf(
	const SMouseDownEvent& /* inMouseDown */)
{
	SpawnInPlaceEditor();
}


// ---------------------------------------------------------------------------
//	¥ SpawnInPlaceEditor									[protected]
// ---------------------------------------------------------------------------
//	Create an in-place edit field. Normally this method will not be overriden.

void
LInPlaceHostView::SpawnInPlaceEditor()
{

	// Fill in default pane info.

	Rect editRect;
	CalcLocalFrameRect(editRect);
	SPaneInfo paneInfo = { 0,							// paneID
					(SInt16) (editRect.right - editRect.left),	// width
					(SInt16) (editRect.bottom - editRect.top),	// height
					true,								// visible
					true,								// enabled
					{ true, true, true, true },			// bindings
					editRect.left,						// left
					editRect.top,						// top
					0,									// userCon
					this };								// superView

	// Create edit field.

	mEditField = new LInPlaceEditField(
					paneInfo,							// inPaneInfo
					mText,								// inString
					mTextTraitsID,						// inTextTraitsID
					mMaxChars,							// inMaxChars
					mEditFieldAttributes,				// inAttributes
					mKeyFilter,							// inKeyFilter
					this);								// inSuper

	ThrowIfNil_(mEditField);
	mEditField->FinishCreate();
	mEditField->AdjustSize();
	SwitchTarget(mEditField);

	// Set up listener relationships for edit field.

	mEditField->SetValueMessage(mValueMessage);
	LFastArrayIterator iter(mListeners);
	LListener* listener;
	while (iter.Next(&listener)) {
		mEditField->AddListener(listener);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ UpdateCachedFrame										[protected]
// ---------------------------------------------------------------------------
//	Update the mLocalTextFrame member so that it reflects the location
//	of the text to be displayed.

void
LInPlaceHostView::UpdateCachedFrame()
{

	// Cache frame rect of new text for hit testing.

	Rect frame;
	CalcLocalFrameRect(frame);
	if (mEditFieldAttributes & editAttr_Box)
		::MacInsetRect(&frame, 3, 2);
	else
		::MacInsetRect(&frame, 1, 0);
	SInt16 just = UTextTraits::SetPortTextTraits(mTextTraitsID);
	UNewTextDrawing::MeasureWithJustification((Ptr) &mText[1], mText[0], frame, just, mTextLocalFrame, true);

}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf										[protected]
// ---------------------------------------------------------------------------
//	Overridden to ensure that the cached text frame is correctly initialized.

void
LInPlaceHostView::FinishCreateSelf()
{
	LView::FinishCreateSelf();
	UpdateCachedFrame();
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ InitHostView											[private]
// ---------------------------------------------------------------------------
//	Common initializer for LInPlaceHostView constructors.

void
LInPlaceHostView::InitHostView()
{
	mEditField = nil;
	mEditFieldAttributes = editAttr_Box;
	mKeyFilter = &UKeyFilters::PrintingCharField;
	mMaxChars = 255;
	mTextTraitsID = 0;
	mValueMessage = mPaneID;
}


PP_End_Namespace_PowerPlant
