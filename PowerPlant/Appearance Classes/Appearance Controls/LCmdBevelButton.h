// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCmdBevelButton.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LCmdBevelButton
#define _H_LCmdBevelButton
#pragma once

#include <LBevelButton.h>
#include <LPeriodical.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LCmdBevelButton: public LBevelButton,
						 public LPeriodical {
public:
	enum { class_ID = FOUR_CHAR_CODE('cbbt') };

						LCmdBevelButton(
								LStream			*inStream,
								ClassIDT		inImpID = imp_class_ID);

						LCmdBevelButton(
								const SPaneInfo	&inPaneInfo,
								MessageT		inValueMessage,
								SInt16			inBevelProc,
								SInt16			inBehavior,
								SInt16			inContentType,
								SInt16			inContentResID,
								ResIDT			inTextTraits,
								ConstStringPtr	inTitle,
								SInt16			inInitialValue,
								SInt16			inTitlePlacement,
								SInt16			inTitleAlignment,
								SInt16			inTitleOffset,
								SInt16			inGraphicAlignment,
								Point			inGraphicOffset,
								CommandT		inCommand,
								ClassIDT		inImpID = imp_class_ID);

						LCmdBevelButton(
								const SPaneInfo	&inPaneInfo,
								MessageT		inValueMessage,
								SInt16			inBevelProc,
								ResIDT			inMenuID,
								SInt16			inMenuPlacement,
								SInt16			inContentType,
								SInt16			inContentResID,
								ResIDT			inTextTraits,
								ConstStringPtr	inTitle,
								SInt16			inInitialValue,
								SInt16			inTitlePlacement,
								SInt16			inTitleAlignment,
								SInt16			inTitleOffset,
								SInt16			inGraphicAlignment,
								Point			inGraphicOffset,
								Boolean			inCenterPopupGlyph,
								CommandT		inCommand,
								ClassIDT		inImpID = imp_class_ID);

	virtual				~LCmdBevelButton();

	void				SetCommandNumber( CommandT inCommand )
							{
								mCommand = inCommand;
							}

	CommandT			GetCommandNumber()		{ return mCommand; }

	virtual void		SpendTime( const EventRecord& inMacEvent );

protected:
	CommandT		mCommand;

	virtual void		HotSpotResult(
								SInt16			inHotSpot);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
