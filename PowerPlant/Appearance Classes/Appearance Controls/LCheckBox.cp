// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCheckBox.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LCheckBox.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LCheckBox								Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LCheckBox::LCheckBox(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlPane(inStream, inImpID)
{
}


// ---------------------------------------------------------------------------
//	¥ LCheckBox								Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LCheckBox::LCheckBox(
	const SPaneInfo	&inPaneInfo,
	MessageT		inValueMessage,
	SInt32			inValue,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	ClassIDT		inImpID)
		: LControlPane(inPaneInfo, inImpID, kControlCheckBoxProc,
						inTitle, inTextTraitsID, inValueMessage, inValue,
						Button_Off, Button_Mixed)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LCheckBox							Destructor				  [public]
// ---------------------------------------------------------------------------

LCheckBox::~LCheckBox()
{
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult
// ---------------------------------------------------------------------------

void
LCheckBox::HotSpotResult(
	SInt16	/* inHotSpot */)
{
	SInt16	newValue = Button_On;
	if (GetValue() == Button_On) {
		newValue = Button_Off;
	}
	SetValue(newValue);
}


PP_End_Namespace_PowerPlant
