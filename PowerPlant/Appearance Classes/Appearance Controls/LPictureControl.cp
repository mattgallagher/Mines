// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPictureControl.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LPictureControl.h>
#include <PP_Messages.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LPictureControl						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LPictureControl::LPictureControl(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlView(inStream, inImpID)
{
}


// ---------------------------------------------------------------------------
//	¥ LPictureControl						Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LPictureControl::LPictureControl(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	MessageT			inMessage,
	bool				inTracking,
	ResIDT				inPICTResID,
	ClassIDT			inImpID)

	: LControlView(inPaneInfo, inViewInfo, inImpID,
					inTracking ? kControlPictureProc
							   : kControlPictureNoTrackProc,
					Str_Empty, 0, inMessage, inPICTResID)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LPictureControl						Destructor				  [public]
// ---------------------------------------------------------------------------

LPictureControl::~LPictureControl()
{
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult													  [public]
// ---------------------------------------------------------------------------

void
LPictureControl::HotSpotResult(
	SInt16	/* inHotSpot */)
{
	MessageT	message = mControlSubPane->GetValueMessage();
	if (message != msg_Nothing) {
		BroadcastMessage(message, this);
	}
}


PP_End_Namespace_PowerPlant
