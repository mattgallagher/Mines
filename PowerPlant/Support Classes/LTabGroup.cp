// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTabGroup.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A TabGroup switches the Target amongst its SubCommanders in response to
//	Tab and Shift-Tab key presses. A SubCommander is responsible for
//	passing up Tab key presses to its SuperCommander if it wants to be
//	usuable with a TabGroup.
//
//	USAGE NOTE:
//	By default, the first SubCommander of a TabGroup will be the Target
//	when the Window containing the TabGroup is activated. SubCommanders
//	include all descendents not just children. SubCommanders are in
//	depth-first traversal order.
//
//	If there is more than one TabGroup in a Window, then the last TabGroup
//	will be the one on duty.
//
//	To make a particular Commander be the Target when its Window is
//	activated, call SetLatentSub() for that Commander.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTabGroup.h>
#include <TArrayIterator.h>
#include <LStream.h>
#include <LView.h>

#include <PP_KeyCodes.h>
#include <PP_Messages.h>

PP_Begin_Namespace_PowerPlant

const UInt16	mask_NonShiftModifiers	= cmdKey + optionKey + controlKey;


// ---------------------------------------------------------------------------
//	¥ LTabGroup								Default Constructor		  [public]
// ---------------------------------------------------------------------------

LTabGroup::LTabGroup()
{
		// TabGroup wants to be on duty when its SuperCommander
		// is put on duty.

	if (mSuperCommander != nil) {
		mSuperCommander->SetLatentSub(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ LTabGroup								Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LTabGroup::LTabGroup(
	const LTabGroup&	inOriginal)

	: LCommander(inOriginal)
{
}


// ---------------------------------------------------------------------------
//	¥ LTabGroup								Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LTabGroup::LTabGroup(
	LStream*	/* inStream */)
{
		// TabGroup wants to be on duty when its SuperCommander
		// is put on duty.

	if (mSuperCommander != nil) {
		mSuperCommander->SetLatentSub(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ LTabGroup								Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LTabGroup::LTabGroup(
	LCommander*	inSuper)

	: LCommander(inSuper)
{
		// TabGroup wants to be on duty when its SuperCommander
		// is put on duty.

	if (mSuperCommander != nil) {
		mSuperCommander->SetLatentSub(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LTabGroup							Destructor				  [public]
// ---------------------------------------------------------------------------

LTabGroup::~LTabGroup()
{
}


// ---------------------------------------------------------------------------
//	¥ HandleKeyPress												  [public]
// ---------------------------------------------------------------------------
//	Tab switches the Target to the next item in the TabGroup.
//	Shift-Tab to the previous item.
//	All other keystrokes (and Tabs with modifiers other than Shift)
//		get passed up.

Boolean
LTabGroup::HandleKeyPress(
	const EventRecord	&inKeyEvent)
{
	Boolean	keyHandled = true;
	UInt16	theChar = (UInt16) (inKeyEvent.message & charCodeMask);

		// Process Tab or Shift-Tab. Pass up if there are any other
		// modifiers keys pressed.

	if ((theChar == char_Tab) &&
		((inKeyEvent.modifiers & mask_NonShiftModifiers) == 0)) {

		RotateTarget((inKeyEvent.modifiers & shiftKey) != 0);

	} else {
		keyHandled = LCommander::HandleKeyPress(inKeyEvent);
	}

	return keyHandled;
}


// ---------------------------------------------------------------------------
//	¥ RotateTarget													  [public]
// ---------------------------------------------------------------------------
//	Switch Target to another SubCommander, either the one before or after
//	the current one

void
LTabGroup::RotateTarget(
	Boolean	inBackward)				// Rotate backward or forward?
{
		// Find starting point for rotation. Normally, the starting
		// point will be the current Target. However, if the Target
		// is not a SubCommander of the TabGroup, then we start
		// the rotation at the TabGroup itself.

	LCommander	*currTabTarget = GetTarget();
	SInt32		index = 1;

	if (currTabTarget == nil) {		// There is no Target
		currTabTarget = this;		// Start rotation at the TabGroup itself

	} else if (currTabTarget != this) {
									// Search up Command chain to see if
									//   the Target is a SubCommander of
									//   the TabGroup
		LCommander*	cmdr = currTabTarget;
		while ( (cmdr != nil) && (cmdr != this) ) {
			cmdr = cmdr->GetSuperCommander();
		}

		if (cmdr == this) {			// Target is a SubCommander of TabGroup
									// Find birth order of Target
			index = currTabTarget->GetSuperCommander()->
						GetSubCommanders().FetchIndexOf(currTabTarget);

		} else {					// TabGroup does not contain Target
			currTabTarget = this;	// Start rotation at the TabGroup itself
		}
	}

		// Now that we have the starting point, try to switch
		// the Target to the next or previous Commander. We need
		// to continue rotating until we find a Commander that
		// wants to be the Target, or we arrive at the same point
		// that we started.

	bool		switched = false;
	LCommander	*newTarget = currTabTarget;

	do {
		if (inBackward) {
			GetPrevTabCommander(newTarget, index);
		} else {
			GetNextTabCommander(newTarget, index);
		}

		if (newTarget != this) {
			switched = newTarget->ProcessCommand(msg_TabSelect, &inBackward);
		}

	} while (!switched && (newTarget != currTabTarget));

	if (switched) {					// We've found a willing Target
		SwitchTarget(newTarget);
	}
}


// ---------------------------------------------------------------------------
//	¥ BeTarget													   [protected]
// ---------------------------------------------------------------------------
//	TabGroup has become the Target

void
LTabGroup::BeTarget()
{
		// A TabGroup will become the Target if:
		//	> Its Window is activated and the TabGroup is latent but
		//		none of its SubCommanders are latent
		//	> A SubCommander that is the Target no longer wants to be the
		//		Target (for example, it's being deleted, hidden, or disabled)
		//	> Program explicitly switches the Target to the TabGroup
		//
		//	In each case, it makes sense for the TabGroup to try to
		//	make its first SubCommander the Target by calling
		//	RotateTarget

	RotateTarget(false);
}


// ---------------------------------------------------------------------------
//	¥ GetNextTabCommander											  [public]
// ---------------------------------------------------------------------------
//	Pass back the next commander in the TabGroup
//
//	SubCommanders of the TabGroup are in depth-first traversal order

void
LTabGroup::GetNextTabCommander(
	LCommander*	&ioCommander,
	SInt32		&ioIndex)
{
	if (ioCommander == this) {			// Cmdr after TabGroup is the deepest
										//   one along the first child chain
		ioIndex = 1;
		if (mSubCommanders.GetCount() > 0) {
			ioCommander = mSubCommanders[1];
			while (ioCommander->GetSubCommanders().GetCount() > 0) {
				ioCommander = (ioCommander->GetSubCommanders())[1];
			}
		}

	} else {							// First choice is deepest first
										//   child descendent of younger
										//   sibling
		TArray<LCommander*>	&siblings = ioCommander->GetSuperCommander()->
											GetSubCommanders();

		if ( siblings.FetchItemAt(++ioIndex, ioCommander) ) {
										// There is a younger sibling
										//   Find deepest descent along
										//   first child lineage
			while (ioCommander->GetSubCommanders().GetCount() > 0) {
				ioCommander = (ioCommander->GetSubCommanders())[1];
				ioIndex = 1;
			}

		} else {						// No younger sibling
										//   Next Cmdr is the parent
			ioCommander = ioCommander->GetSuperCommander();
			ioIndex = 1;

			if (ioCommander != this) {	// Find birth order of parent
				ioIndex = ioCommander->GetSuperCommander()->
								GetSubCommanders().FetchIndexOf(ioCommander);
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ GetPrevTabCommander											  [public]
// ---------------------------------------------------------------------------
//	Pass back the previous commander in the TabGroup
//
//	SubCommanders of the TabGroup are in depth-first traversal order

void
LTabGroup::GetPrevTabCommander(
	LCommander*	&ioCommander,
	SInt32		&ioIndex)
{
										// First choice is oldest child
	SInt32	index = (SInt32) ioCommander->GetSubCommanders().GetCount();
	if (index > 0) {
		ioCommander = ioCommander->GetSubCommanders()[index];
		ioIndex = index;
		return;
	}

	if (ioCommander == this) {			// If TabGroup has no children,
		ioIndex = 1;					//   there is no previous cmdr
		return;
	}

	index = ioIndex - 1;				// Second choice is older sibling
	if (index > 0) {
		ioCommander = ioCommander->GetSuperCommander()->
										GetSubCommanders()[index];
		ioIndex = index;
		return;
	}

										// Third choice is older sibling of
										//    parent (or nearest ancestor
										//    with an older sibling)
	ioCommander = ioCommander->GetSuperCommander();
	ioIndex = 1;

	while (ioCommander != this) {		// Stop if we reach the TabGroup
		TArray<LCommander*>	&siblings =
				ioCommander->GetSuperCommander()->GetSubCommanders();

		index = siblings.FetchIndexOf(ioCommander) - 1;

		if (index > 0) {				// There is an older sibling
			ioCommander = siblings[index];
			ioIndex = index;
			break;
		}
										// No older sibling, move up
										//   ancestor chain
		ioCommander = ioCommander->GetSuperCommander();
	}
}


PP_End_Namespace_PowerPlant
