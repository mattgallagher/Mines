/********************************************************************************/
/*																				*/
/*	© 2006, Aurbach & Associates, Inc.  All rights reserved.					*/
/*																				*/
/*	Redistribution and use in source and binary forms, with or without			*/
/*	modification, are permitted provided that the following condition			*/
/*	are met:																	*/
/*																				*/
/*	  ¥	Redistributions of source code must retain the above copyright			*/
/*		notice, this list of conditions and the following disclaimer.			*/
/*																				*/
/*	  ¥	Redistributions in binary form must reproduce the above copyright		*/
/*		notice, this list of conditions and the following disclaimer in the		*/
/*		documentation and/or other materials provided with the distribution.	*/
/*																				*/
/*	  ¥	Neither the name of Aurbach & Associates, Inc. nor the names of any		*/
/*		of its employees may be used to endorse or promote products derived		*/
/*		from this software without specific prior written permission.			*/
/*																				*/
/*	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS			*/
/*	ÒAS ISÓ AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT			*/
/*	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A		*/
/*	PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER	*/
/*	OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,	*/
/*	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,			*/
/*	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; */
/*	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,	*/
/*	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR		*/
/*	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF		*/
/*	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.									*/
/*																				*/
/********************************************************************************/
/*
 *	Class Name:			LContextMenuAttachment
 *
 *	Change History:
 *
 *	Who     	Date	    Description
 * ---------------------------------------------------------------------------
 *	RLA		28-Sep-2006		Original Code (with significant contributions from
 *							  LCMAttachment by John C. Daub)
 *	RLA		27-Sep-2006		Adapted for contribution to open-powerplant
 */

#ifndef _H_LCMAttachment
#define _H_LCMAttachment
#pragma once

#include <LAttachment.h>
#include <LPane.h>
#include <LString.h>
#include <LCommander.h>
#include <Menus.h>

#include <vector>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

enum	{
	kCMAItemSeparator		= 0,
	kCMAItemMenuItem		= 1,
	kCMAItemMenuID			= 2
};

typedef struct	{
	SInt16					mtype;
	UInt32					mval;
} MENTRY;

class	LContextMenuHelper;

// ---------------------------------------------------------------------------

class	LContextMenuAttachment : public LAttachment {
public:
	enum { class_ID = FOUR_CHAR_CODE('CMat') };

						LContextMenuAttachment( LStream* inStream );
						
						LContextMenuAttachment(
								MessageT				inMessage = msg_ContextClick,
								Boolean					inExecuteHost = true,
								SInt16					inBaseMenuID = 0,
								UInt32					inHelpType = kCMHelpItemNoHelp,
								const LStr255 &			inHelpString = (ConstStringPtr) Str_Empty,
								SInt16					inEntryCount = 0,
								MENTRY *				inEntries = nil );

	virtual				~LContextMenuAttachment() {}

protected:
	SInt16				mMENUid;
	UInt32				mHelpType;
	LStr255				mHelpString;
	std::vector<MENTRY>	mEntryList;

	virtual	void		ExecuteSelf(
								MessageT				inMessage,
								void*					ioParam);
	
	virtual LContextMenuHelper *
						CreateHelper (
								SInt16					inMenuID,
								LCommander *			inCtxTarget );
	
	virtual void		PrepareForMenu (
								LContextMenuHelper *	/* inHelper */ ) {}

	virtual	bool		FindCommandString(
								CommandT				inCommand,
								LString &				outString );

	virtual	LCommander*	FindCommandTarget();

private:
						LContextMenuAttachment();
						
						LContextMenuAttachment( const LContextMenuAttachment& );

	LContextMenuAttachment&		
						operator = ( const LContextMenuAttachment& );
};

// ---------------------------------------------------------------------------

struct	SCMForward	{
	Point				globalPt;
	LAttachable *		ownerHost;
};


class	LContextMenuForwarder	:	public	LAttachment	{
public:
	enum { class_ID = FOUR_CHAR_CODE('CMfd') };
	
						LContextMenuForwarder ( LStream * inStream );
						
						LContextMenuForwarder (
								LCommander *			inCommandTarget = nil,
								Boolean					inExecuteHost = true );
						
	virtual				~LContextMenuForwarder() {}
	
			void		SetCommandTarget (
								LCommander *			inCommandTarget ) 
								{
									mTarget = inCommandTarget;
								}

protected:
	LCommander *	mTarget;
	
	virtual void		ExecuteSelf (
								MessageT				inMessage,
								void *					ioParam );
	
	virtual LCommander*	FindCommandTarget();

private:
						LContextMenuForwarder();

						LContextMenuForwarder( const LContextMenuForwarder& );
	
	LContextMenuForwarder&
						operator = ( const LContextMenuForwarder& );
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
