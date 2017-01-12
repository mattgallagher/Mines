// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LBroadcasterEditField.h		PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LBroadcasterEditField
#define _H_LBroadcasterEditField
#pragma once

#include <LEditField.h>
#include <LBroadcaster.h>
#include <UKeyFilters.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ===========================================================================
//		¥ LBroadcasterEditField
// ===========================================================================
//	LBroadcasterEditField is an edit field that broadcasts a value
//	changed message whenever the text value is changed. It is sort
//	of a hybrid between LControl and LEditField.

class LBroadcasterEditField :	public LEditField,
								public LBroadcaster {

public:
	enum { class_ID = FOUR_CHAR_CODE('bref') };

						LBroadcasterEditField();
						LBroadcasterEditField(
								LStream*			inStream);

						LBroadcasterEditField(
								const SPaneInfo&	inPaneInfo,
								ConstStringPtr		inString,
								ResIDT				inTextTraitsID,
								SInt16				inMaxChars,
								UInt8				inAttributes,
								TEKeyFilterFunc		inKeyFilter,
								LCommander*			inSuper);

	virtual				~LBroadcasterEditField();

	MessageT			GetValueMessage() const		{ return mValueMessage; }
	void				SetValueMessage(
								MessageT			inValueMessage)
							{
								mValueMessage = inValueMessage;
							}


	virtual void		FindCommandStatus(
								CommandT	inCommand,
								Boolean&	outEnabled,
								Boolean&	outUsesMark,
								UInt16&		outMark,
								Str255		outName);

	virtual void		BroadcastValueMessage();

protected:
	MessageT			mValueMessage;

	virtual void		DontBeTarget();

	virtual Boolean		HandleKeyPress(
								const EventRecord&	inKeyEvent);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
