// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPushButton.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LPushButton.h>
#include <LControlImp.h>
#include <LStream.h>
#include <LView.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LPushButton							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LPushButton::LPushButton(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlPane(inStream, inImpID)
{
	mValue = 0;

	Boolean	isDefault;
	*inStream >> isDefault;

	if (isDefault) {
		SetDefaultButton(true);
	}
}


// ---------------------------------------------------------------------------
//	¥ LPushButton							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LPushButton::LPushButton(
	const SPaneInfo&	inPaneInfo,
	MessageT			inValueMessage,
	ConstStringPtr		inTitle,
	SInt16				inKind,
	ResIDT				inTextTraitsID,
	SInt16				inCicnID,
	ClassIDT			inImpID)

	: LControlPane(inPaneInfo, inImpID, inKind, inTitle,
						inTextTraitsID, inValueMessage, 0, 0, inCicnID)
{
	mValue = 0;
}


// ---------------------------------------------------------------------------
//	¥ ~LPushButton							Destructor				  [public]
// ---------------------------------------------------------------------------

LPushButton::~LPushButton()
{
}


// ---------------------------------------------------------------------------
//	¥ SetDefaultButton												  [public]
// ---------------------------------------------------------------------------

void
LPushButton::SetDefaultButton(
	Boolean		inDefault)
{
	if (inDefault != IsDefaultButton()) {	// Default state is changing
		SetDataTag(0, kControlPushButtonDefaultTag,
					sizeof(Boolean), &inDefault);
	}
}


// ---------------------------------------------------------------------------
//	¥ IsDefaultButton												  [public]
// ---------------------------------------------------------------------------

Boolean
LPushButton::IsDefaultButton() const
{
	Boolean	isDefault = false;
	GetDataTag(0, kControlPushButtonDefaultTag, sizeof(Boolean), &isDefault);
	return isDefault;
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult												   [protected]
// ---------------------------------------------------------------------------

void
LPushButton::HotSpotResult(
	SInt16	/* inHotSpot */)
{
	BroadcastValueMessage();		// Although value doesn't change,
									//   send message to inform Listeners
									//   that button was clicked
}


PP_End_Namespace_PowerPlant
