// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTabGroupView.cp			PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	LTabGroupView implements a tab group (see LTabGroup.cp). Subpanes of this
//	view are considered members of the tab group.
//
//	This is useful over LTabGroup as it gives you a visual element to
//	manupulate (in Constructor, for FindPaneByID, etc.). The same reasoning
//	as why use LRadioGroupView over LRadioGroup. Also, this allows you to
//	easily pick and choose what objects are and are not part of the tab
//	group.
//
//	Additionally, this facilitates the ability to have multiple tab groups
//	in a window. This is possible to do with LTabGroup, but LTabGroupView
//	is a little easier to use.
//
//	Most of the functionality of this class comes via inheritance from
//	LTabGroup itself.


#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LTabGroupView.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTabGroupView							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LTabGroupView::LTabGroupView()
{
}


// ---------------------------------------------------------------------------
//	¥ LTabGroupView							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LTabGroupView::LTabGroupView(
	LStream*	inStream)

	: LView(inStream),
	  LTabGroup(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ LTabGroupView							Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LTabGroupView::LTabGroupView(
	const LTabGroupView&	inOriginal)

	: LView(inOriginal),
	  LTabGroup(inOriginal)
{
}


// ---------------------------------------------------------------------------
//	¥ LTabGroupView							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LTabGroupView::LTabGroupView(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	LCommander*			inSuper)

	: LView(inPaneInfo, inViewInfo),
	  LTabGroup(inSuper)
{
}


// ---------------------------------------------------------------------------
//	¥ LTabGroupView							Destructor				  [public]
// ---------------------------------------------------------------------------

LTabGroupView::~LTabGroupView()
{
}


// ---------------------------------------------------------------------------
//	¥ Hide
// ---------------------------------------------------------------------------
//	Make a TabGroupView invisible

void
LTabGroupView::Hide()
{
		// A TabGroupView normally has subcommanders that will also
		// become hidden. To avoid a cascade of each subcommander
		// giving up being the Target as it is hidden, we switch
		// the Target here rather than in HideSelf().

	if (IsOnDuty()) {					// Hidden field can't be the Target
		SwitchTarget(GetSuperCommander());
	}
	
	LView::Hide();
}


// ---------------------------------------------------------------------------
//	¥ SuperHide
// ---------------------------------------------------------------------------
//	The SuperView of a TabGroupView has been hidden

void
LTabGroupView::SuperHide()
{
		// A TabGroupView normally has subcommanders that will also
		// become hidden. To avoid a cascade of each subcommander
		// giving up being the Target as it is hidden, we switch
		// the Target here rather than in HideSelf().

	if (IsOnDuty()) {					// Hidden field can't be the Target
		SwitchTarget(GetSuperCommander());
	}
	
	LView::SuperHide();
}


PP_End_Namespace_PowerPlant
