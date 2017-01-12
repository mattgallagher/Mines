// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAttachment.h				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	An Attachment executes in response to a message number and an optional
//	parameter.

#ifndef _H_LAttachment
#define _H_LAttachment
#pragma once

#include <PP_Prefix.h>
#include <PP_Messages.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LAttachable;
class	LStream;

// ---------------------------------------------------------------------------

class	LAttachment {
public:
	enum { class_ID = FOUR_CHAR_CODE('atch') };

					LAttachment(
							MessageT	inMessage = msg_AnyMessage,
							Boolean		inExecuteHost = true);

					LAttachment( LStream* inStream );

	virtual			~LAttachment();

	MessageT		GetMessage() const		{ return mMessage; }

	virtual void	SetMessage( MessageT inMessage );

	Boolean			GetExecuteHost() const	{ return mExecuteHost; }

	virtual void	SetExecuteHost( Boolean inExecuteHost );

	virtual void	SetOwnerHost( LAttachable *inOwnerHost );

	LAttachable*	GetOwnerHost() const	{ return mOwnerHost; }

	virtual	Boolean	Execute(
							MessageT	inMessage,
							void*		ioParam);

protected:
	LAttachable*	mOwnerHost;
	MessageT		mMessage;
	Boolean			mExecuteHost;

	virtual void	ExecuteSelf(
							MessageT	inMessage,
							void*		ioParam);
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
