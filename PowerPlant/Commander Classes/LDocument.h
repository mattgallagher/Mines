// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDocument.h					PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LDocument
#define _H_LDocument
#pragma once

#include <LCommander.h>
#include <LModelObject.h>
#include <UPrinting.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LDocument : public LCommander,
				  public LModelObject {
public:
						LDocument();

						LDocument( LCommander* inSuper );

	virtual				~LDocument();

	virtual void		Close();

	virtual Boolean		ObeyCommand(
								CommandT		inCommand,
								void*			ioParam);

	virtual void		FindCommandStatus(
								CommandT		inCommand,
								Boolean&		outEnabled,
								Boolean&		outUsesMark,
								UInt16&			outMark,
								Str255			outName);

	virtual Boolean		IsModified();

	virtual void		SetModified( Boolean inModified );

	virtual Boolean		IsSpecified() const;
	
	virtual void		SetSpecified( Boolean inSpecified );

	virtual StringPtr	GetDescriptor( Str255 outDescriptor ) const = 0;

	virtual Boolean		UsesFileSpec( const FSSpec& inFileSpec ) const;
	
	LPrintSpec&			GetPrintSpec();

	virtual void		MakeCurrent();

	virtual Boolean		AskSaveAs(
								FSSpec			&outFSSpec,
								Boolean			inRecordIt);

	virtual OSType		GetFileType() const;

	virtual void		AttemptClose( Boolean inRecordIt );

	virtual Boolean		AttemptQuitSelf( SInt32 inSaveOption );

	virtual bool		AskConfirmRevert();

	virtual SInt16		AskSaveChanges( bool inQuitting );

	virtual void		DoAEClose( const AppleEvent& inCloseAE );

	virtual void		HandleAESave( const AppleEvent& inSaveAE );

	virtual void		DoAESave(
								FSSpec&			inFileSpec,
								OSType			inFileType);

	virtual void		DoSave();
	virtual void		DoRevert();

	virtual void		HandlePageSetup();
	virtual void		HandlePrint();
	virtual void		HandlePrintOne();
	virtual void		DoPrint();

		// AppleEvent Object Model Support

	virtual void		MakeSelfSpecifier(
								AEDesc&			inSuperSpecifier,
								AEDesc&			outSelfSpecifier) const;

	virtual void		HandleAppleEvent(
								const AppleEvent&	inAppleEvent,
								AppleEvent&			outAEReply,
								AEDesc&				outResult,
								SInt32				inAENumber);

	virtual void		GetAEProperty(
								DescType		inProperty,
								const AEDesc&	inRequestedType,
								AEDesc&			outPropertyDesc) const;

	virtual bool		AEPropertyExists( DescType inProperty ) const;

			// Sending AppleEvents

	virtual void		SendAESaveAs(
								FSSpec&			inFileSpec,
								OSType			inFileType,
								Boolean			inExecute);

	virtual void		SendAEClose(
								SInt32			inSaveOption,
								FSSpec&			inFileSpec,
								Boolean			inExecute);
								
		// Accessing list of existing Document objects

	static LDocument*	FindNamedDocument( ConstStringPtr inName );

	static LDocument*	FindByFileSpec( const FSSpec& inFileSpec );

	static TArray<LDocument*>& GetDocumentList()
							{
								return sDocumentList;
							}

protected:
	static TArray<LDocument*>	sDocumentList;

	LPrintSpec			mPrintSpec;
	bool				mIsModified;
	bool				mIsSpecified;
};


const	OSType	fileType_Default	= FOUR_CHAR_CODE('****');

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
