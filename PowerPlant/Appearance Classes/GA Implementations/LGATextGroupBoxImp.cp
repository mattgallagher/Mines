// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGATextGroupBoxImp.cp		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGATextGroupBoxImp.h>
#include <LGAGroupBoxImp.h>

#include <LControlImp.h>
#include <LStream.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UTextTraits.h>
#include <LStaticText.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Constants

const SInt16	groupBox_HeaderLeftOffset		=	9;
const SInt16	groupBox_HeaderLineOffset		=	3;
const PaneIDT	groupBox_DefaultHeaderPaneID	=	FOUR_CHAR_CODE('grph');


// ---------------------------------------------------------------------------
//	¥ LGATextGroupBoxImp					Constructor				  [public]
// ---------------------------------------------------------------------------

LGATextGroupBoxImp::LGATextGroupBoxImp(
	LControlPane*	inControlPane)

	:LGAGroupBoxImp(inControlPane)
{
}


// ---------------------------------------------------------------------------
//	¥ LGATextGroupBoxImp					Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGATextGroupBoxImp::LGATextGroupBoxImp(
	LStream*	inStream)

	: LGAGroupBoxImp(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LGATextGroupBoxImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LGATextGroupBoxImp::~LGATextGroupBoxImp ()
{
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGATextGroupBoxImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream )
{
	SInt16	controlKind;
	Str255	title;
	ResIDT	textTraitID;

	*inStream >> controlKind;
	*inStream >> textTraitID;
	inStream->ReadPString ( title );

	Init(inControlPane, controlKind, title, textTraitID);
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGATextGroupBoxImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			/* inRefCon */)
{
	mControlPane   = inControlPane;
	mPrimaryBorder = (inControlKind == kControlGroupBoxTextTitleProc);

	BuildTextHeader(inTitle, inTextTraitsID, groupBox_DefaultHeaderPaneID);
}


// ---------------------------------------------------------------------------
//	¥ BuildTextHeader											   [protected]
// ---------------------------------------------------------------------------

void
LGATextGroupBoxImp::BuildTextHeader(
	ConstStringPtr		inDescriptor,
	ResIDT				inTextTraitID,
	PaneIDT				inHeaderID)
{
		// If there is no title, then we skip creating the header and
		// simply initialize the header object to nil and the baseline
		// to zero. This will create a group box that has no title.

	if (inDescriptor[0] != 0) {

		SPaneInfo paneInfo;

		paneInfo.paneID			 = inHeaderID;
		paneInfo.width			 = 100;
		paneInfo.height			 = 20;
		paneInfo.visible		 = true;
		paneInfo.enabled		 = true;
		paneInfo.bindings.left   = true;
		paneInfo.bindings.top    = true;
		paneInfo.bindings.right  = false;
		paneInfo.bindings.bottom = false;
		paneInfo.left			 = groupBox_HeaderLeftOffset +
										groupBox_HeaderLineOffset;
		paneInfo.top			 = 0;
		paneInfo.userCon		 = 0;
		paneInfo.superView		 = mControlPane->GetSuperView();

		mHeaderObject = new LStaticText(paneInfo, inDescriptor,
											inTextTraitID);

									// Now we need to resize the caption to
									// fit the header
		ResizeToBestRect(mBaseline, false);
	}
}


PP_End_Namespace_PowerPlant
