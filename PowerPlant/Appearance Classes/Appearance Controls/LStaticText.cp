// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LStaticText.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LStaticText.h>
#include <LControlImp.h>
#include <LString.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LStaticText							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LStaticText::LStaticText(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlPane(inStream, inImpID)
{
}


// ---------------------------------------------------------------------------
//	¥ LStaticText							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LStaticText::LStaticText(
	const SPaneInfo&	inPaneInfo,
	ConstStringPtr		inTitle,
	ResIDT				inTextTraitsID,
	ClassIDT			inImpID)

	: LControlPane(inPaneInfo, inImpID, kControlStaticTextProc,
					inTitle, inTextTraitsID)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LStaticText							Destructor				  [public]
// ---------------------------------------------------------------------------

LStaticText::~LStaticText()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------

void
LStaticText::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
	SetText((Ptr) inDescriptor + 1, inDescriptor[0]);
}


// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------

StringPtr
LStaticText::GetDescriptor(
	Str255	outDescriptor) const
{
	Size	strLen;
	GetText((Ptr) outDescriptor + 1, 255, &strLen);

	if (strLen > 255) {
		strLen = 255;
	}
	outDescriptor[0] = (UInt8) strLen;

	return outDescriptor;
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LStaticText::SetValue(
	SInt32		inValue)
{
	LStr255		valStr = inValue;
	SetText(valStr);
}


// ---------------------------------------------------------------------------
//	¥ GetValue														  [public]
// ---------------------------------------------------------------------------

SInt32
LStaticText::GetValue() const
{
	LStr255		valStr;
	GetText(valStr);
	return (SInt32) valStr;
}


// ---------------------------------------------------------------------------
//	¥ SetText														  [public]
// ---------------------------------------------------------------------------

void
LStaticText::SetText(
	Ptr		inTextPtr,
	Size	inTextLength)
{
	mControlImp->SetDataTag(0, kControlStaticTextTextTag,
							inTextLength, inTextPtr);
}


void
LStaticText::SetText(
	const LString&	inString)
{
	mControlImp->SetDataTag(0, kControlStaticTextTextTag, inString.Length(),
							&(const_cast<LString&>(inString))[1]);
}


// ---------------------------------------------------------------------------
//	¥ GetText														  [public]
// ---------------------------------------------------------------------------

void
LStaticText::GetText(
	Ptr		outBuffer,
	Size	inBufferLength,
	Size*	outTextLength) const
{
	mControlImp->GetDataTag(0, kControlStaticTextTextTag,
							inBufferLength, outBuffer, outTextLength);
}


void
LStaticText::GetText(
	LString&	outString) const
{
	UInt16	maxLen = (UInt16) (outString.GetMaxLength() - 1);
	Size	strLen;

	mControlImp->GetDataTag(0, kControlStaticTextTextTag,
							maxLen, outString.TextPtr(), &strLen);

	if (strLen > maxLen) {
		strLen = maxLen;
	}
	outString[0] = (UInt8) strLen;
}


PP_End_Namespace_PowerPlant
