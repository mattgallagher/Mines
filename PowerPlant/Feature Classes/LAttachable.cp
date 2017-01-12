// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAttachable.cp				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	Abstract class for an object which has an associated list of Attachments
//	that can be executed on demand.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LAttachable.h>
#include <LAttachment.h>
#include <TArrayIterator.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Class Variables

LAttachable*	LAttachable::sDefaultAttachable = nil;


// ---------------------------------------------------------------------------
//	¥ LAttachable							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LAttachable::LAttachable()
{
	mAttachments = nil;
	SetDefaultAttachable(this);
}


// ---------------------------------------------------------------------------
//	¥ LAttachable							Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LAttachable::LAttachable(
	const LAttachable& /* inOriginal */)
{
	mAttachments = nil;						// Shallow copy
	SetDefaultAttachable(this);
}


// ---------------------------------------------------------------------------
//	¥ ~LAttachable							Destructor				  [public]
// ---------------------------------------------------------------------------

LAttachable::~LAttachable()
{
	RemoveAllAttachments();

	if (sDefaultAttachable == this) {
		SetDefaultAttachable(nil);
	}
}


// ---------------------------------------------------------------------------
//	¥ AddAttachment													  [public]
// ---------------------------------------------------------------------------
//	Add an Attachment to an Attachable before another Attachment
//
//	Specify "nil" for inBefore to add at the end
//
//	If inOwnsAttachment is true, the Attachable assumes ownership of the
//	Attachment and will delete the Attachment when the Attachable itself
//	is deleted

void
LAttachable::AddAttachment(
	LAttachment*	inAttachment,
	LAttachment*	inBefore,
	Boolean			inOwnsAttachment)
{
	if (mAttachments == nil) {		// Create array if it doesn't exist
		mAttachments = new TArray<LAttachment*>;
	}

									// Find place to insert new Attachment
	SInt32	atIndex = LArray::index_Last;
	if (inBefore != nil) {
		atIndex = mAttachments->FetchIndexOf(inBefore);
		if (atIndex == LArray::index_Bad) {
			SignalStringLiteral_("Attachment not found");
			atIndex = LArray::index_Last;
		}
	}

	mAttachments->InsertItemsAt(1, atIndex, inAttachment);

	if (inOwnsAttachment) {
		inAttachment->SetOwnerHost(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveAttachment												  [public]
// ---------------------------------------------------------------------------
//	Remove an Attachment from an Attachable
//
//	If this Attachable is the owner of the Attachment, the Attachment's
//	owner is set to nil, and the caller should assume control of
//	the Attachment.

void
LAttachable::RemoveAttachment(
	LAttachment*	inAttachment)
{
	if (mAttachments != nil) {
		mAttachments->Remove(inAttachment);
		if (inAttachment->GetOwnerHost() == this) {
			inAttachment->SetOwnerHost(nil);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveAllAttachments											  [public]
// ---------------------------------------------------------------------------
//	Remove all Attachments from an Attachable
//
//	All Attachments owned by this Attachable are deleted

void
LAttachable::RemoveAllAttachments()
{
	if (mAttachments != nil) {
		TArrayIterator<LAttachment*>	iterator(*mAttachments);
		LAttachment		*theAttachment;
		while (iterator.Next(theAttachment)) {
			if (theAttachment->GetOwnerHost() == this) {
				delete theAttachment;
			}
		}

		delete mAttachments;
		mAttachments = nil;
	}
}


// ---------------------------------------------------------------------------
//	¥ ExecuteAttachments											  [public]
// ---------------------------------------------------------------------------
//	Tell all associated Attachments to execute themselves for the
//	specified message
//
//	The return value specifies whether the default Host action should be
//	executed. The value is false if any Attachment's Execute() function
//	returns false, otherwise it's true.

Boolean
LAttachable::ExecuteAttachments(
	MessageT		inMessage,
	void*			ioParam)
{
	Boolean	executeHost = true;

	if (mAttachments != nil) {
		TArrayIterator<LAttachment*>	iterator(*mAttachments);
		LAttachment		*theAttachment;
		while (iterator.Next(theAttachment)) {
			executeHost &= theAttachment->Execute(inMessage, ioParam);
		}
	}

	return executeHost;
}


PP_End_Namespace_PowerPlant
