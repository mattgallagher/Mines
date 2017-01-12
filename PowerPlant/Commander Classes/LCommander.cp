// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCommander.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Mix-in class for an object that responds to menu commands and keystrokes

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LCommander.h>
#include <TArrayIterator.h>
#include <LAction.h>
#include <PP_Messages.h>
#include <UMemoryMgr.h>

#if __MACH__
	#include <Carbon/Carbon.h>
#else
	#include <Balloons.h>
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Class Variables

LCommander*		LCommander::sTopCommander = nil;
LCommander*		LCommander::sTarget = nil;
LCommander*		LCommander::sDefaultCommander = nil;
Boolean			LCommander::sUpdateCommandStatus = false;


// ---------------------------------------------------------------------------
//	¥ LCommander							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LCommander::LCommander()
{
	InitCommander(GetDefaultCommander());
}


// ---------------------------------------------------------------------------
//	¥ LCommander							Copy Constructor		  [public]
// ---------------------------------------------------------------------------
//	Makes a shallow copy; SubCommanders are not copied.

LCommander::LCommander(
	const LCommander	&inOriginal)
{
	InitCommander(inOriginal.mSuperCommander);
}


// ---------------------------------------------------------------------------
//	¥ LCommander							Constructor				  [public]
// ---------------------------------------------------------------------------

LCommander::LCommander(
	LCommander*		inSuper)
{
	InitCommander(inSuper);
}


// ---------------------------------------------------------------------------
//	¥ InitCommander							Initializer				 [private]
// ---------------------------------------------------------------------------
//	Private function for initializing data members from Constructors

void
LCommander::InitCommander(
	LCommander	*inSuper)
{
	mOnDuty = triState_Off;
	mSuperCommander = nil;
	SetSuperCommander(inSuper);
	SetDefaultCommander(this);
}


// ---------------------------------------------------------------------------
//	¥ ~LCommander							Destructor				  [public]
// ---------------------------------------------------------------------------

LCommander::~LCommander()
{
	LCommander*		saveSuper = mSuperCommander;

	if (IsOnDuty()) {

			// If this Commander is on duty, then it or one of its
			// SubCommanders is the Target. This means that the Target
			// is going to be deleted. To avoid leaving a dangling
			// Target, we make the SuperCommander the new Target
			// after informing the Commanders in the currently active
			// chain that they are going off duty.

		sTarget->DontBeTarget();
		sTarget->TakeChainOffDuty(mSuperCommander);

			// Before setting the Target to the SuperCommander, we detach
			// this object from the command chain. This avoids some problems
			// with Commanders that automatically switch the Target when
			// they become the Target. We don't want this object switched
			// back to being the Target.
			//
			// Afterwards, we re-attach this object since deleting its
			// subcommanders may have side effects that rely on the
			// chain of command being intact.
			
		SetSuperCommander(nil);			// Detach from SuperCommander first
		SetTarget(saveSuper);
		SetSuperCommander(saveSuper);	// Re-attach to SuperCommander
	}

									// Delete all SubCommanders
	TArrayIterator<LCommander*> iterator(mSubCommanders, LArrayIterator::from_End);
	LCommander*		theSub;
	while (iterator.Previous(theSub)) {
		mSubCommanders.RemoveItemsAt(1, iterator.GetCurrentIndex());
		delete theSub;
	}

	SetSuperCommander(nil);			// Detach from SuperCommander

	if (sTarget == this) {

			// At this point, we really shouldn't be the Target since
			// we set the Target to our SuperCommander if we were on
			// duty at the top of this function. But if we are the Target,
			// we try again to set the Target to our SuperCommander.

		SetTarget(saveSuper);

		if (sTarget == this) {

				// Our SuperCommander insists on resetting the Target
				// to this object as a side effect of being notified
				// that it is the Target. We can't leave a dangling Target,
				// so we explicitly set the Target to our SuperCommander.

			sTarget = saveSuper;
		}
	}

	if (sDefaultCommander == this) {
		SetDefaultCommander(nil);	// Don't leave a dangling Default Cmdr
	}

	if (sTopCommander == this) {
		sTopCommander = nil;		// Don't leave a dangling Top Cmdr
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetSuperCommander
// ---------------------------------------------------------------------------

void
LCommander::SetSuperCommander(
	LCommander*		inSuper)
{
	if (mSuperCommander != nil) {
		mSuperCommander->RemoveSubCommander(this);
	}

	mSuperCommander = inSuper;

	if (inSuper != nil) {
		inSuper->AddSubCommander(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ AllowSubRemoval
// ---------------------------------------------------------------------------

Boolean
LCommander::AllowSubRemoval(
	LCommander*	/* inSub */)
{
	return true;
}


// ---------------------------------------------------------------------------
//	¥ AttemptQuit
// ---------------------------------------------------------------------------

Boolean
LCommander::AttemptQuit(
	long	inSaveOption)
{
	Boolean	quitAllowed = true;

	TArrayIterator<LCommander*> iterator(mSubCommanders);
	LCommander*		theSub;
	while (iterator.Next(theSub) && quitAllowed) {
		quitAllowed = theSub->AttemptQuit(inSaveOption);
	}

	if (quitAllowed) {
		quitAllowed = AttemptQuitSelf(inSaveOption);
	}

	return quitAllowed;
}


// ---------------------------------------------------------------------------
//	¥ AttemptQuitSelf
// ---------------------------------------------------------------------------

Boolean
LCommander::AttemptQuitSelf(
	SInt32	/* inSaveOption */)
{
	return true;
}


// ---------------------------------------------------------------------------
//	¥ AddSubCommander
// ---------------------------------------------------------------------------

void
LCommander::AddSubCommander(
	LCommander*		inSub)
{
	mSubCommanders.AddItem(inSub);
}


// ---------------------------------------------------------------------------
//	¥ RemoveSubCommander
// ---------------------------------------------------------------------------

void
LCommander::RemoveSubCommander(
	LCommander*		inSub)
{
	mSubCommanders.Remove(inSub);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetTarget										[inline] [static] [public]
// ---------------------------------------------------------------------------
//	Return a pointer to the Commander that is the current Target
//	Inline function


// ---------------------------------------------------------------------------
//	¥ SetTarget												 [static] [public]
// ---------------------------------------------------------------------------
//	Set the Target class variable. Called internally.

void
LCommander::SetTarget(
	LCommander*		inNewTarget)
{
	sTarget = inNewTarget;
	if (inNewTarget != nil) {
		SetUpdateCommandStatus(true);
		inNewTarget->BeTarget();

		LCommander*	commander = inNewTarget;
		do {
			commander->SubTargetChanged();
			commander = commander->GetSuperCommander();
		} while (commander != nil);
	}
}


// ---------------------------------------------------------------------------
//	¥ SwitchTarget											 [static] [public]
// ---------------------------------------------------------------------------
//	Try to change Target and return whether the specified Commander did
//	indeed become the Target.
//
//	Returns false if the current Target refuses to give up being the Target,
//	or if the potential new Target doesn't want to be the Target. This could
//	happen when performing data entry validation.

Boolean
LCommander::SwitchTarget(
	LCommander*		inNewTarget)
{
	Boolean		switchOK = true;

	if (inNewTarget != sTarget) {		// Do nothing if no change in Target

										// Ask current target if it wants to
										//   give up being the Target.
		if (sTarget != nil) {
			switchOK = sTarget->AllowDontBeTarget(inNewTarget);
		}
										// Ask potential new target if it
										//   wants to be the Target
		if (switchOK  &&  (inNewTarget != nil)) {
			switchOK = inNewTarget->AllowBeTarget(inNewTarget);
		}

		if (switchOK) {					// Carry out the Target switch
			ForceTargetSwitch(inNewTarget);
		}
	}

	return switchOK;
}


// ---------------------------------------------------------------------------
//	¥ ForceTargetSwitch										 [static] [public]
// ---------------------------------------------------------------------------
//	Change Target to the specified Commander.
//
//	Current and new Targets can't disallow the switch. Call this functions
//	instead of SwitchTarget() in situations where you can't reasonably
//	abort the target switch. For example, from within a destructor.

void
LCommander::ForceTargetSwitch(
	LCommander*		inNewTarget)
{
			// Exit if current and new target are the same
	if (inNewTarget == sTarget) {
		return;
	}

	if (inNewTarget == nil) {
			// New chain of command is empty
			// Take off duty all commanders in current chain of command
		if (sTarget != nil) {
			sTarget->DontBeTarget();
			sTarget->TakeChainOffDuty(nil);
		}

	} else if (sTarget == nil) {
			// There is no current chain of command
			// Put on duty all commanders in new chain
		inNewTarget->PutChainOnDuty(inNewTarget);

	} else {

			// Current and new targets both exist. We must determine
			// where the current and new chains of command overlap.
			// The "junction" is the deepest commander that is common to
			// both the current and new chains of command--it is a
			// Superior of both the current and new targets. Searching up
			// from the new target, the first on duty commander is the
			// junction.

		LCommander*		junction = inNewTarget;
		while ((junction != nil) && !junction->IsOnDuty()) {
			junction = junction->mSuperCommander;
		}

			// Tell current target that it will no longer be the target.
			// If that results in a target change, tell the new target that
			// it will no longer be the target either.

        LCommander*		oldTarget;
        do {
            oldTarget = sTarget;
            oldTarget->DontBeTarget();
        } while ( (sTarget != nil) &&
        		  (sTarget != inNewTarget) &&
        		  (sTarget != oldTarget) );

			// Determine relative positions of current and new targets in
			// the command chain. There are 5 possibilities.

        if (sTarget == nil) {
                // 1. There is no current chain of command
                							// Put on duty all commanders
                                            // in new chain
            inNewTarget->PutChainOnDuty(inNewTarget);

        } else if (sTarget == inNewTarget) {
                // 2. Target switching during some "DontBeTarget()" call
                //    brought us to the desired target
            return;

		} else if (junction == sTarget) {
				// 3. New target is a sub commander of the current target
											// Extend chain of command down to
											// the new target
			inNewTarget->PutChainOnDuty(inNewTarget);

		} else if (junction == inNewTarget) {
				// 4. New target is a super commander of the current target
											// Shorten chain of command up to
											// the new target
			sTarget->TakeChainOffDuty(inNewTarget);

		} else {
				// 5. New and current targets are on different branches
											// Take off duty current chain up
											// to, but not including, the
											// junction
			sTarget->TakeChainOffDuty(junction);
											// Put on duty new chain
			inNewTarget->PutChainOnDuty(inNewTarget);
		}
	}

	SetTarget(inNewTarget);			// Finally, change to new target
}


// ---------------------------------------------------------------------------
//	¥ RestoreTarget											 [static] [public]
// ---------------------------------------------------------------------------
//	Set target to the Commander which was the target when this Commander
//	was last on duty

void
LCommander::RestoreTarget()
{
		// When a command chain is taken off duty, the top Commander
		// is marked as off duty, while all its Subordinates that
		// were on duty are marked as latent. To restore the command
		// chain, we must set the new target to the deepest latent
		// Subordinate. If there are no latent Subordinates, then
		// this Commander is the new target.

	LCommander*	newTarget = this;	// Start with this Commander

									// Search downward for latent Subordinates
	LCommander*	latentSub = GetLatentSub();
	while (latentSub != nil) {
		newTarget = latentSub;
		latentSub = latentSub->GetLatentSub();
	}

	SwitchTarget(newTarget);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AllowBeTarget
// ---------------------------------------------------------------------------
//	Give permission to switch target to the specified Commander
//
//	Before switching targets, this function gets called for the potential new
//	target. The default action is to pass the request up the command chain.
//
//	Subclasses should override this function if they wish to disallow
//	a target switch under certain circumstances. sTarget is still
//	the current target.

Boolean
LCommander::AllowBeTarget(
	LCommander*		inNewTarget)
{
	Boolean	allowIt = true;

	if (mSuperCommander != nil) {
		allowIt = mSuperCommander->AllowBeTarget(inNewTarget);
	}

	return allowIt;
}


// ---------------------------------------------------------------------------
//	¥ AllowDontBeTarget
// ---------------------------------------------------------------------------
//	Give permission to switch target to the specified Commander
//
//	Before switching targets, this function gets called for the current
//	target. The default action is to pass the request up the command chain.
//
//	Subclasses should override this function if they wish to disallow
//	a target switch under certain circumstances (eg, performing validation
//	checks on the contents of the target). sTarget is still the current target.

Boolean
LCommander::AllowDontBeTarget(
	LCommander*		inNewTarget)
{
	Boolean	allowIt = true;

	if (mSuperCommander != nil) {
		allowIt = mSuperCommander->AllowDontBeTarget(inNewTarget);
	}

	return allowIt;
}


// ---------------------------------------------------------------------------
//	¥ BeTarget
// ---------------------------------------------------------------------------
//	Commander is becoming the Target
//
//	Subclasses should override this function if they wish to behave
//	differently when they are and are not the Target. At entry, the class
//	variable sTarget points to this Commander.

void
LCommander::BeTarget()
{
}


// ---------------------------------------------------------------------------
//	¥ DontBeTarget
// ---------------------------------------------------------------------------
//	Commander will no longer be the Target
//
//	Subclasses should override this function if they wish to behave
//	differently when they are and are not the Target. At entry, the class
//	variable sTarget points to this Commander. sTarget will be changed
//	soon afterwards to the new target.

void
LCommander::DontBeTarget()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetLatentSub
// ---------------------------------------------------------------------------
//	Find the latent SubCommander of a Commander
//
//	A Commander may have one or no latent SubCommander.

LCommander*
LCommander::GetLatentSub() const
{
	LCommander*		latentSub = nil;
	TArrayIterator<LCommander*> iterator(mSubCommanders);
	LCommander*		theSub;
	while (iterator.Next(theSub)) {
		if (theSub->mOnDuty == triState_Latent) {
			latentSub = theSub;
			break;
		}
	}
	return latentSub;
}


// ---------------------------------------------------------------------------
//	¥ SetLatentSub
// ---------------------------------------------------------------------------
//	Specify the SubCommander that will be put on duty when RestoreTarget()
//	is called for this Commander.
//
//	Does nothing if this Commander is already on duty
//
//	inSub may be nil, in which case this Commander will have no Latent
//	SubCommander. This will also be the case if inSub is not a
//	Subordinate of this Commander (which raises a Signal).

void
LCommander::SetLatentSub(
	LCommander*		inSub)
{
		// A Commander can have a Latent SubCommander only if it
		// is not currently on duty

	if (mOnDuty != triState_On) {

		LCommander*		mostLatent   = this;
		LCommander*		theCommander = inSub;

		if (inSub != nil) {
				// Verify that this Commander is a Superior of inSub
			do {
				theCommander = theCommander->mSuperCommander;
				if (theCommander == this) {
					break;
				}
			} while (theCommander != nil);

			mostLatent = inSub;
			if (theCommander == nil) {
					// This Commander is not a Superior of inSub
					// Treat the same as if inSub were nil
				mostLatent = this;
				SignalStringLiteral_("inSub is not a subordinate");
			}
		}

			// For all Commanders from mostLatent to "this", inclusive,
			// turn off all currently latent SubCommanders

		theCommander = mostLatent;
		do {
			LCommander*		currLatent = theCommander->GetLatentSub();
			if (currLatent != nil) {
				currLatent->mOnDuty = triState_Off;
			}

			if (theCommander == this) {
				break;
			}
			theCommander = theCommander->mSuperCommander;
		} while (true);

			// Make Latent all Commanders from mostLatent up to,
			// but not including, "this"

		theCommander = mostLatent;
		while (theCommander != this) {
			theCommander->mOnDuty = triState_Latent;
			theCommander = theCommander->mSuperCommander;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ PutChainOnDuty
// ---------------------------------------------------------------------------
//	Put on duty a chain of Commanders
//
//	inNewTarget is the Commander that is becoming the Target, which is
//	this Commander or one of its SubCommanders.
//
//	This is a wrapper function that guarantees that a Commander can't be
//	put on duty until all its Superiors are on duty.

void
LCommander::PutChainOnDuty(
	LCommander*		inNewTarget)
{
		// Recursive function. Must put SuperCommander on duty before
		// this Commander can be put on duty. Recursion stops when reaching
		// a Commander that is already on duty, or a Commander that has
		// no SuperCommander

	if (mOnDuty != triState_On) {
		if (mSuperCommander != nil) {
			mSuperCommander->PutChainOnDuty(inNewTarget);
		}

			// We are now returning from all the recursive calls

			// Take off duty all subcommanders. The proper subcommander
			// will be put on duty as we continue returning from
			// the recursive calls

		TArrayIterator<LCommander*> iterator(mSubCommanders);
		LCommander*		theSub;
		while (iterator.Next(theSub)) {
			theSub->mOnDuty = triState_Off;
		}

		mOnDuty = triState_On;
		PutOnDuty(inNewTarget);
	}
}


// ---------------------------------------------------------------------------
//	¥ PutOnDuty
// ---------------------------------------------------------------------------
//	A Commander is going on duty.
//
//	inNewTarget is the Commander that is becoming the Target, which is
//	this Commander or one of its SubCommanders.
//
//	Subclasses should override this function if they wish to behave
//	differently when on duty than when off duty

void
LCommander::PutOnDuty(
	LCommander*	/* inNewTarget */)
{
}


// ---------------------------------------------------------------------------
//	¥ TakeChainOffDuty
// ---------------------------------------------------------------------------
//	Take off duty a chain of Commanders
//
//	This is a wrapper function that traverses a command chain from the
//	Commander that first receives this message up to the specified
//	Commander. The inUpToCmdr is *not* taken off duty.
//
//	It is an error if inUpToCmdr is not a Superior of the Commander that
//	first receives this message. Note that this includes the case where
//	inUpToCmdr is the Commander that first receives this message (since a
//	Commander is not a Superior of itself). nil is a valid value for
//	inUpToCmdr, since the Commander at the top of a chain of command has
//	a nil SuperCommander.

void
LCommander::TakeChainOffDuty(
	LCommander*		inUpToCmdr)
{
		// Move up chain of command until reaching the Commander whose
		// SuperCommander is inUpToCmdr. Set the "on duty" flag as follows:

		//		Chain of Cmd	OnDuty
		//		------------	------
		//		inUpToCmdr		On			Could be nil
		//		cmdr 1			Off			SuperCommander is inUpToCmdr
		//		cmdr 2			Latent
		//		...				...
		//		cmdr n			Latent		First to receive message

	SignalIf_(mOnDuty != triState_On);		// Must be on duty

	LCommander*		theCmdr  = this;		// Start iterating here

	do {
		theCmdr->mOnDuty = triState_Latent;	// Commander becomes latent and
		theCmdr->TakeOffDuty();				//   not on duty

		if (theCmdr->mSuperCommander == inUpToCmdr) {
											// This is "cmdr 1" in the table
											//   above. Make it "off duty"
											//   and stop iterating
			theCmdr->mOnDuty = triState_Off;
			break;
		}

		theCmdr = theCmdr->mSuperCommander;	// Move up command chain

	} while (theCmdr != nil);				// We shouldn't hit this condition
											//   but check to be safe
}


// ---------------------------------------------------------------------------
//	¥ TakeOffDuty
// ---------------------------------------------------------------------------
//	A Commander is going off duty
//
//	Subclasses should override this function if they wish to behave
//	differently when on duty than when off duty

void
LCommander::TakeOffDuty()
{
}


// ---------------------------------------------------------------------------
//	¥ SubTargetChanged
// ---------------------------------------------------------------------------
//	A subcommander of a Commander has become the new Target
//
//	When the Target changes, this functions is called for each of its
//	super commanders. Call sequence starts at the immediate super commander
//	of the new Target and moves up the chain of command.
//
//	Use Note: Subclasses should override this function is they wish to detect
//	Target changes which don't change their on duty status. For example, if
//	the Target changes from one of its subcommander to another, a Commander
//	will remain on duty (and TakeOffDuty doesn't get called).

void
LCommander::SubTargetChanged()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ProcessCommand
// ---------------------------------------------------------------------------
//	Issue a Command to a Commander
//
//	Returns whether or not the command was handled
//
//	This function lets Attachments handle the Command before calling the
//	normal ObeyCommand() function.

Boolean
LCommander::ProcessCommand(
	CommandT	inCommand,
	void*		ioParam)
{
	Boolean	cmdHandled = true;
	if (ExecuteAttachments(inCommand, ioParam)) {
		cmdHandled = ObeyCommand(inCommand, ioParam);
	}
	return cmdHandled;
}


// ---------------------------------------------------------------------------
//	¥ ObeyCommand
// ---------------------------------------------------------------------------
//	Issue a Command to a Commander
//
//	Returns whether or not the command was handled
//
//	Subclasses must override this method in order to respond to commands.

Boolean
LCommander::ObeyCommand(
	CommandT	inCommand,
	void*		ioParam)
{
	Boolean	cmdHandled = false;
	if (mSuperCommander != nil) {
		cmdHandled = mSuperCommander->ProcessCommand(inCommand, ioParam);
	}
	return cmdHandled;
}


// ---------------------------------------------------------------------------
//	¥ ProcessCommandStatus
// ---------------------------------------------------------------------------
//	Pass back the status of a Command
//
//	This function lets Attachments set the command status before calling
//	the normal FindCommandStatus() function.

void
LCommander::ProcessCommandStatus(
	CommandT	inCommand,
	Boolean&	outEnabled,
	Boolean&	outUsesMark,
	UInt16&		outMark,
	Str255		outName)
{
	SCommandStatus	status;
	status.command	= inCommand;
	status.enabled	= &outEnabled;
	status.usesMark	= &outUsesMark,
	status.mark		= &outMark;
	status.name		= outName;

	if (ExecuteAttachments(msg_CommandStatus, &status)) {
		FindCommandStatus(inCommand, outEnabled, outUsesMark, outMark,
					outName);
	}
}


// ---------------------------------------------------------------------------
//	¥ FindCommandStatus
// ---------------------------------------------------------------------------
//	Pass back the status of a Command
//
//	Subclasses must override to enable/disable and mark commands
//
//	PowerPlant uses the enabling and marking information to set the
//	appearance of Menu items.

void
LCommander::FindCommandStatus(
	CommandT	inCommand,
	Boolean&	outEnabled,
	Boolean&	outUsesMark,
	UInt16&		outMark,
	Str255		outName)
{
	if (mSuperCommander != nil) {
		mSuperCommander->ProcessCommandStatus(inCommand, outEnabled,
											outUsesMark, outMark, outName);
	} else {
		outEnabled  = false;	// Query has reached the top of a command
		outUsesMark = false;	//   chain without any object dealing with
								//   it, so command is disabled and unmarked
	}
}


// ---------------------------------------------------------------------------
//	¥ IsSyntheticCommand											  [static]
// ---------------------------------------------------------------------------
//	Return whether a Command is synthetic. If so, pass back the associated
//	Menu ID and item number. If not synthetic, outMenuID and outMenuItem
//	are undefined.
//
//	A synthetic command number has the Menu ID in the high 16 bits
//	and the item number in the low 16 bits, with the result negatated.
//		syntheticCmd = - (MenuID << 16) - ItemNumber
//	A synthetic command is the negative of the value returned by
//	the Toolbox traps MenuSelect and MenuKey.
//
//	The LMenu and LMenBar classes return synthetic command numbers for
//	menu items whose actual command number is cmd_UseMenuItem. You should
//	use synthetic command numbers when the menu choice depends on the
//	runtime name of the menu item. For example, an item in a Font menu.
//
//	NOTE: This function does not work for the Help Menu, which has a
//	negative menu ID. Use the IsHelpMenuCommand() function instead.

Boolean
LCommander::IsSyntheticCommand(
	CommandT	inCommand,
	ResIDT&		outMenuID,
	SInt16&		outMenuItem)
{
	Boolean	isSynthetic = false;
	
	if (inCommand < 0) {				// A synthetic command is negative
										// with a non-zero hi word
		outMenuID = (SInt16) ((-inCommand) >> 16);
		if (outMenuID > 0) {
			outMenuItem = (SInt16) (-inCommand);
			isSynthetic = true;
		}
	}
	
	return isSynthetic;
}


// ---------------------------------------------------------------------------
//	¥ IsHelpMenuCommand												  [static]
// ---------------------------------------------------------------------------
//	Return whether the Command corresponds to a choice from the Help Menu,
//	and if so, pass back the item index number
//
//	The system Help Menu has a negative menu ID, which interferes with
//	PowerPlant's scheme of encoding synthetic command numbers. Thus, we
//	need a special function to handle the Help Menu.

Boolean
LCommander::IsHelpMenuCommand(
	CommandT	inCommand,
	SInt16&		outMenuItem)
{
	Boolean	isHelpItem = false;
	
	if ( (SInt16) ((-inCommand) >> 16) == kHMHelpMenuID) {
		outMenuItem = (SInt16) (-inCommand);
		isHelpItem = true;
	}
	
	return isHelpItem;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ProcessKeyPress
// ---------------------------------------------------------------------------

Boolean
LCommander::ProcessKeyPress(
	const EventRecord&	inKeyEvent)
{
	Boolean	keyHandled = true;
	if( ExecuteAttachments(msg_KeyPress, (void*) &inKeyEvent)) {
		keyHandled = HandleKeyPress(inKeyEvent);
	}
	return keyHandled;
}


// ---------------------------------------------------------------------------
//	¥ HandleKeyPress
// ---------------------------------------------------------------------------
//	Tell Commander to handle a keystroke
//
//	Returns whether or not the keystroke was handled
//
//	Subclasses must override this function to respond to keystrokes. In
//	general subclasses should call this function, which passes keystrokes
//	up the command chain, if they do not handle a particular keystroke.
//
//	In pseudo-code, a subclass's HandleKeyPress function might be:
//		if (inKeyEvent is a keystroke I handle) {
//			do something with inKeyEvent
//			return true;
//		} else {
//			return LCommander::HandleKeyPress(inKeyEvent);
//		}

Boolean
LCommander::HandleKeyPress(
	const EventRecord&	inKeyEvent)
{
	Boolean	keyHandled = false;
	if (mSuperCommander != nil) {
		keyHandled = mSuperCommander->ProcessKeyPress(inKeyEvent);
	}
	return keyHandled;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ PostAction
// ---------------------------------------------------------------------------
//	If an attachment doesn't intercept the msg_PostAction, this function will
//	Post the action up to supercommander of this object.
//
//	The default NULL parameter will effectively clear or "commit" the last
//	semantic action.
//
//	If an attachment didn't process the posting and there's no supercommander,
//	this function will attempt to Redo, Finalize, and delete the action.

void
LCommander::PostAction(
	LAction*	inAction)
{
	SetUpdateCommandStatus(true);

	if (ExecuteAttachments(msg_PostAction, inAction)) {
		if (mSuperCommander != nil) {
			mSuperCommander->PostAction(inAction);

		} else {		// There's no Undoer in the command hierarchy
						// So, you won't be able to undo.
			if (inAction != nil) {
										// Delete even if exception occurs
				StDeleter<LAction>	theAction(inAction);

				inAction->Redo();
				inAction->Finalize();
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ PostAnAction													  [static]
// ---------------------------------------------------------------------------
//	Post the Action to the present target.  This is a static member
//	function that can be used by anything needing to post an action.
//
//	If there is no present target, function will attempt to Redo, Finalize,
//	and delete the action.

void
LCommander::PostAnAction(
	LAction*	inAction)
{
	if (sTarget != nil) {
		sTarget->PostAction(inAction);

	} else {							// There's no Target to store the
										//   Action for later undoing
		if (inAction != nil) {

				// The only thing we can do here is perform the Action,
				// Finalize it, and then delete it.

			SetUpdateCommandStatus(true);

										// Delete even if exception occurs
			StDeleter<LAction>	theAction(inAction);

			inAction->Redo();
			inAction->Finalize();
		}
	}
}


PP_End_Namespace_PowerPlant
