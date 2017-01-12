// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LImageWell.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LImageWell.h>
#include <PP_Messages.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LImageWell							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LImageWell::LImageWell(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlView(inStream, inImpID)
{
	SetMinValue(0);
	SetValue(0);
	SetMaxValue(2);
}


// ---------------------------------------------------------------------------
//	¥ LImageWell							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LImageWell::LImageWell(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	MessageT			inValueMessage,
	SInt16				inContentType,
	ResIDT				inContentResID,
	ClassIDT			inImpID)

	: LControlView(inPaneInfo, inViewInfo, inImpID,
						kControlImageWellProc, Str_Empty, 0, inValueMessage,
						inContentResID, inContentType, 0)
{
	SetMinValue(0);
	SetValue(0);
}


// ---------------------------------------------------------------------------
//	¥ ~LImageWell							Destructor				  [public]
// ---------------------------------------------------------------------------

LImageWell::~LImageWell()
{
}


void
LImageWell::SetContentInfo(
	ControlButtonContentInfo&	inInfo)
{
	SetDataTag(0, kControlImageWellContentTag,
				sizeof(ControlButtonContentInfo), &inInfo);
}


void
LImageWell::GetContentInfo(
	ControlButtonContentInfo&	outInfo) const
{
	GetDataTag(0, kControlImageWellContentTag,
				sizeof(ControlButtonContentInfo), &outInfo);
}


void
LImageWell::SetTransform(
	IconTransformType	inTransform)
{
	SetDataTag(0, kControlImageWellTransformTag,
				sizeof(IconTransformType), &inTransform);
}


IconTransformType
LImageWell::GetTransform() const
{
	IconTransformType	transform;
	GetDataTag(0, kControlImageWellTransformTag,
				sizeof(IconTransformType), &transform);
	return transform;
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult													  [public]
// ---------------------------------------------------------------------------

void
LImageWell::HotSpotResult(
	SInt16	/* inHotSpot */)
{
	MessageT	message = mControlSubPane->GetValueMessage();
	if (message != msg_Nothing) {
		BroadcastMessage(message, this);
	}
}


PP_End_Namespace_PowerPlant
