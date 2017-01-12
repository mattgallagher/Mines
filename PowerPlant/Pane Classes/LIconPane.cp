// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LIconPane.cp				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Pane which draws a single icon from an icon family

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LIconPane.h>
#include <LStream.h>
#include <PP_Constants.h>

#include <Icons.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LIconPane								Default Constructor		  [public]
// ---------------------------------------------------------------------------

LIconPane::LIconPane()
{
	mIconID = resID_Undefined;
}


// ---------------------------------------------------------------------------
//	¥ LIconPane								Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LIconPane::LIconPane(
	const SPaneInfo&	inPaneInfo,
	ResIDT				inIconID)

	: LPane(inPaneInfo)
{
	mIconID = inIconID;
}


// ---------------------------------------------------------------------------
//	¥ LIconPane								Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LIconPane::LIconPane(
	LStream*	inStream)

	: LPane(inStream)
{
	*inStream >> mIconID;
}


// ---------------------------------------------------------------------------
//	¥ ~LIconPane							Destructor				  [public]
// ---------------------------------------------------------------------------

LIconPane::~LIconPane()
{
}


// ---------------------------------------------------------------------------
//	¥ SetIconID														  [public]
// ---------------------------------------------------------------------------

void
LIconPane::SetIconID(
	ResIDT	inIconID)
{
	mIconID = inIconID;
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LIconPane::DrawSelf()
{
	Rect	frame;
	CalcLocalFrameRect(frame);

	::PlotIconID(&frame, kAlignNone, kTransformNone, mIconID);
}


PP_End_Namespace_PowerPlant
