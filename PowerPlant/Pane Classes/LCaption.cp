// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCaption.cp					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Pane with a block of text

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LCaption.h>
#include <LStream.h>
#include <UDrawingUtils.h>
#include <UTextTraits.h>

#include <TextUtils.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LCaption								Default Constructor		  [public]
// ---------------------------------------------------------------------------

LCaption::LCaption()
{
	mTxtrID  = 0;
}


// ---------------------------------------------------------------------------
//	¥ LCaption								Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LCaption::LCaption(
	const LCaption&		inOriginal)

	: LPane(inOriginal)
{
	mText   = inOriginal.mText;
	mTxtrID = inOriginal.mTxtrID;
}


// ---------------------------------------------------------------------------
//	¥ LCaption								Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LCaption::LCaption(
	const SPaneInfo&	inPaneInfo,
	ConstStringPtr		inString,
	ResIDT				inTextTraitsID)

	: LPane(inPaneInfo),
	  mText(inString)
{
	mTxtrID = inTextTraitsID;
}


// ---------------------------------------------------------------------------
//	¥ LCaption								Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LCaption::LCaption(
	LStream*	inStream)

	: LPane(inStream)
{
	*inStream >> (StringPtr) mText;
	*inStream >> mTxtrID;
}


// ---------------------------------------------------------------------------
//	¥ ~LCaption								Destructor				  [public]
// ---------------------------------------------------------------------------

LCaption::~LCaption()
{
}


// ---------------------------------------------------------------------------
//	¥ GetDescriptor
// ---------------------------------------------------------------------------
//	Return contents of a Caption as a string

StringPtr
LCaption::GetDescriptor(
	Str255	outDescriptor) const
{
	return LString::CopyPStr(mText, outDescriptor);
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor
// ---------------------------------------------------------------------------
//	Set contents of a Caption from a string

void
LCaption::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
	mText = inDescriptor;
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ GetValue
// ---------------------------------------------------------------------------
//	Return the integer value represented by the text of Caption

SInt32
LCaption::GetValue() const
{
	return mText;
}


// ---------------------------------------------------------------------------
//	¥ SetValue
// ---------------------------------------------------------------------------
//	Set a Caption to the text representation of an integer value

void
LCaption::SetValue(
	SInt32	inValue)
{
	mText = inValue;
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf
// ---------------------------------------------------------------------------
//	Draw the Caption

void
LCaption::DrawSelf()
{
	Rect	frame;
	CalcLocalFrameRect(frame);

	SInt16	just = UTextTraits::SetPortTextTraits(mTxtrID);

	RGBColor	textColor;
	::GetForeColor(&textColor);

	ApplyForeAndBackColors();
	::RGBForeColor(&textColor);

	UTextDrawing::DrawWithJustification(mText.TextPtr(), mText.LongLength(),
						frame, just, true);
}


PP_End_Namespace_PowerPlant
