// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDocApplication.h			PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LDocApplication
#define _H_LDocApplication
#pragma once

#include <LApplication.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LDocApplication : public LApplication {
public:
							LDocApplication();
							
							~LDocApplication();

	virtual Boolean			ObeyCommand(
									CommandT			inCommand,
									void*				ioParam);

	virtual void			FindCommandStatus(
									CommandT			inCommand,
									Boolean&			outEnabled,
									Boolean&			outUsesMark,
									UInt16&				outMark,
									Str255				outName);

	virtual void			SendAEOpenDoc( FSSpec& inFileSpec );

	virtual void			SendAEOpenDocList( const AEDescList& inFileList );

	virtual void			SendAECreateDocument();

	virtual void			DoAEOpenOrPrintDoc(
									const AppleEvent&	inAppleEvent,
									AppleEvent&			outAEReply,
									SInt32				inAENumber);

	virtual void			OpenOrPrintDocList(
									const AEDescList&	inDocList,
									SInt32				inAENumber);

	virtual void			OpenDocument( FSSpec* inMacFSSpec );

	virtual void			PrintDocument( FSSpec* inMacFSSpec );

	virtual LModelObject*	MakeNewDocument();
	virtual void			ChooseDocument();
	virtual void			SetupPage();

			// ¥ Handle AppleEvents

	virtual void			HandleAppleEvent(
									const AppleEvent&	inAppleEvent,
									AppleEvent&			outAEReply,
									AEDesc&				outResult,
									SInt32				inAENumber);

	virtual LModelObject*	HandleCreateElementEvent(
									DescType			inElemClass,
									DescType			inInsertPosition,
									LModelObject*		inTargetObject,
									const AppleEvent&	inAppleEvent,
									AppleEvent&			outAEReply);

	virtual SInt32			CountSubModels( DescType inModelID ) const;

	virtual void			GetSubModelByPosition(
									DescType			inModelID,
									SInt32				inPosition,
									AEDesc&				outToken) const;

	virtual void			GetSubModelByName(
									DescType			inModelID,
									Str255				inName,
									AEDesc&				outToken) const;

	virtual SInt32			GetPositionOfSubModel(
									DescType			inModelID,
									const LModelObject*	inSubModel) const;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
