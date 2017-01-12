// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInPlaceEditField.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LInPlaceEditField
#define _H_LInPlaceEditField
#pragma once

#include <LBroadcasterEditField.h>
#include <LAttachment.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ===========================================================================
//		* LInPlaceEditField
// ===========================================================================
//	LInPlaceEditField is an edit field that behaves like the in-place
//	editor fields in the Finder. It resizes itself automatically when
//	the uesr changes the text of the edit field and it goes away
//	automatically when the user clicks outside the edit field or
//	chooses a menu command that isn't handled by the edit field.

class LInPlaceEditField :	public LBroadcasterEditField,
							public LAttachment {

public:
							LInPlaceEditField(
									const SPaneInfo&	inPaneInfo,
									Str255				inString,
									ResIDT				inTextTraitsID,
									SInt16				inMaxChars,
									UInt8				inAttributes,
									TEKeyFilterFunc		inKeyFilter,
									LCommander*			inSuper);
	virtual					~LInPlaceEditField();

	virtual void			UserChangedText();
	virtual void			AdjustSize();

	virtual void			ResizeFrameBy(
									SInt16				inWidthDelta,
									SInt16				inHeightDelta,
									Boolean				inRefresh);

	virtual void			StopEditing();

	inline Boolean			ClosingEditField() const
									{ return mClosingEditField; }

	virtual Boolean			Execute(
									MessageT			inMessage,
									void*				ioParam);

protected:
	virtual void			DeactivateSelf();

	virtual Boolean			HandleKeyPress(
									const EventRecord&	inKeyEvent);
	virtual Boolean			ObeyCommand(
									CommandT			inCommand,
									void*				ioParam);

	virtual void			DontBeTarget();

	virtual void			AlignTextEditRects();

protected:
	SDimension16			mMaximumSize;				// maximum room for edit field to grow
	Boolean					mHasBox;					// true if edit field has a box
	Boolean					mClosingEditField;			// true if edit field is being closed
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
