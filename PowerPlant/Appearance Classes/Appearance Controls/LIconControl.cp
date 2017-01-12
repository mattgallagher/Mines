// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LIconControl.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LIconControl.h>
#include <LControlImp.h>
#include <LStream.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LIconControl							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LIconControl::LIconControl(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlPane(inStream, inImpID)
{
	IconAlignmentType	alignment;
	*inStream >> alignment;

	SetIconAlignment(alignment);

	mValue = 0;
}


// ---------------------------------------------------------------------------
//	¥ LIconControl							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LIconControl::LIconControl(
	const SPaneInfo&	inPaneInfo,
	MessageT			inValueMessage,
	SInt16				inControlKind,
	ResIDT				inResID,
	ClassIDT			inImpID)

	: LControlPane(inPaneInfo, inImpID, inControlKind,
			Str_Empty, 0, inValueMessage, inResID)
{
	mValue = 0;
}


// ---------------------------------------------------------------------------
//	¥ ~LIconControl							Destructor				  [public]
// ---------------------------------------------------------------------------

LIconControl::~LIconControl()
{
}


// ---------------------------------------------------------------------------
//	¥ SetIconAlignment												  [public]
// ---------------------------------------------------------------------------
//	Specify where the icon draws within the frame

void
LIconControl::SetIconAlignment(
	IconAlignmentType	inAlignment)
{
	SetDataTag(0, kControlIconAlignmentTag, sizeof(inAlignment), &inAlignment);
}


// ---------------------------------------------------------------------------
//	¥ GetIconAlignment												  [public]
// ---------------------------------------------------------------------------
//	Return where the icon draws within the frame

IconAlignmentType
LIconControl::GetIconAlignment() const
{
	IconAlignmentType	alignment;
	GetDataTag(0, kControlIconAlignmentTag, sizeof(alignment), &alignment);
	return alignment;
}


// ---------------------------------------------------------------------------
//	¥ SetIconTransform												  [public]
// ---------------------------------------------------------------------------
//	Specify how to draw the icon

void
LIconControl::SetIconTransform(
	IconTransformType	inTransform)
{
	SetDataTag(0, kControlIconTransformTag, sizeof(inTransform), &inTransform);
}


// ---------------------------------------------------------------------------
//	¥ GetIconTransform												  [public]
// ---------------------------------------------------------------------------
//	Return how the icon draws

IconTransformType
LIconControl::GetIconTransform() const
{
	IconTransformType	transform;
	GetDataTag(0, kControlIconTransformTag, sizeof(transform), &transform);
	return transform;
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult												   [protected]
// ---------------------------------------------------------------------------

void
LIconControl::HotSpotResult(
	SInt16	/* inHotSpot */)
{
	BroadcastValueMessage();		// Although value doesn't change,
									//   send message to inform Listeners
									//   that icon was clicked
}


PP_End_Namespace_PowerPlant
