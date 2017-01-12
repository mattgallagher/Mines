// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAttachable.h				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	Abstract class for an object which has an associated list of Attachments
//	that can be executed on demand.

#ifndef _H_LAttachable
#define _H_LAttachable
#pragma once

#include <PP_Prefix.h>
#include <TArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LAttachment;

// ---------------------------------------------------------------------------

class	LAttachable {
public:
						LAttachable();
						
						LAttachable( const LAttachable& inOriginal );

	virtual				~LAttachable();

	virtual void		AddAttachment(
								LAttachment*		inAttachment,
								LAttachment*		inBefore = nil,
								Boolean				inOwnsAttachment = true);

	virtual void		RemoveAttachment(
								LAttachment*		inAttachment);

	virtual void		RemoveAllAttachments();

	virtual Boolean		ExecuteAttachments(
								MessageT			inMessage,
								void*				ioParam);

	TArray<LAttachment*>*	GetAttachmentsList() const { return mAttachments; }

	static LAttachable*	GetDefaultAttachable()
							{
								return sDefaultAttachable;
							}

	static void			SetDefaultAttachable(
								LAttachable*		inAttachable)
							{
								sDefaultAttachable = inAttachable;
							}

protected:
	static LAttachable*	sDefaultAttachable;

	TArray<LAttachment*>	*mAttachments;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
