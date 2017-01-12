// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAttachment.cp				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	An Attachment executes in response to a message number and an optional
//	parameter.
//
//	Usually, an Attachment is associated with a host Attachable object as a
//	means of altering the runtime behavior its host.
//
//	In most cases, subclasses of LAttachment only need to override the
//	ExecuteSelf() function and have a constructor.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LAttachment.h>
#include <LAttachable.h>
#include <LStream.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LAttachment							Constructor				  [public]
// ---------------------------------------------------------------------------

LAttachment::LAttachment(
	MessageT	inMessage,
	Boolean		inExecuteHost)
{
	mOwnerHost	 = nil;
	mMessage	 = inMessage;
	mExecuteHost = inExecuteHost;
}


// ---------------------------------------------------------------------------
//	¥ LAttachment							Stream Constructor		  [public]
// ---------------------------------------------------------------------------
//	Data in Stream must be:
//		MessageT	Message that Attachment responds to
//		Boolean		Execute Host?
//		Boolean		Host Owns Me?

LAttachment::LAttachment(
	LStream*	inStream)
{
	*inStream >> mMessage;
	*inStream >> mExecuteHost;

	mOwnerHost = nil;
	Boolean		hostOwnsMe;
	*inStream >> hostOwnsMe;

	LAttachable	*host = LAttachable::GetDefaultAttachable();

	if (host != nil) {
		host->AddAttachment(this, nil, hostOwnsMe);
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LAttachment							Destructor				  [public]
// ---------------------------------------------------------------------------

LAttachment::~LAttachment()
{
	if (mOwnerHost != nil) {
		mOwnerHost->RemoveAttachment(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetMessage													  [public]
// ---------------------------------------------------------------------------
//	Specify the message for which an Attachment is executed

void
LAttachment::SetMessage(
	MessageT	inMessage)
{
	mMessage = inMessage;
}


// ---------------------------------------------------------------------------
//	¥ SetExecuteHost												  [public]
// ---------------------------------------------------------------------------
//	Specify whether the host action should execute (after executing all
//	Attachments)

void
LAttachment::SetExecuteHost(
	Boolean		inExecuteHost)
{
	mExecuteHost = inExecuteHost;
}


// ---------------------------------------------------------------------------
//	¥ SetOwnerHost													  [public]
// ---------------------------------------------------------------------------
//	Specify the host Attachable object that owns an Attachment.
//
//	An Attachment is automatically deleted when its owner host is deleted.
//	The owner host may be nil.

void
LAttachment::SetOwnerHost(
	LAttachable*	inHost)
{
	mOwnerHost = inHost;
}


// ---------------------------------------------------------------------------
//	¥ Execute														  [public]
// ---------------------------------------------------------------------------
//	Execute an Attachment and return whether or not to execute the host
//	action (after executing all Attachments)

Boolean
LAttachment::Execute(
	MessageT	inMessage,
	void*		ioParam)
{
	Boolean	executeHost = true;

	if ((inMessage == mMessage) || (mMessage == msg_AnyMessage)) {
		ExecuteSelf(inMessage, ioParam);
		executeHost = mExecuteHost;
	}

	return executeHost;
}


// ---------------------------------------------------------------------------
//	¥ ExecuteSelf												   [protected]
// ---------------------------------------------------------------------------
//	Perform the action of an Attachment
//
//	Override this function to do something when the Attachment is executed.
//	You can set mExecuteHost in this function if whether or not to execute
//	the host action depends on the current runtime conditions.
//
//	This function is called only when inMessage matches mMessage (or if
//	mMessage is msg_AnyMessage). The data pointed to by ioParam depends
//	on the message.

void
LAttachment::ExecuteSelf(
	MessageT	/* inMessage */,
	void*		/* ioParam */)
{
}


PP_End_Namespace_PowerPlant
