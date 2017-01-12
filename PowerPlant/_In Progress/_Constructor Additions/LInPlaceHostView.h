// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInPlaceHostView.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LInPlaceHostView
#define _H_LInPlaceHostView
#pragma once

#include <LBroadcaster.h>
#include <LCommander.h>
#include <LString.h>
#include <LView.h>
#include <UKeyFilters.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


class LInPlaceEditField;


// ===========================================================================
//		¥ LInPlaceHostView
// ===========================================================================
//	LInPlaceHostView is a view that can host an in-place editor.
//	When the in-place editor is active, the host view draws nothing.
//	Otherwise it draws the text that would be present.

class LInPlaceHostView :	public LView,
							public LCommander,
							public LBroadcaster {

public:
	enum { class_ID = FOUR_CHAR_CODE('inpe') };

							LInPlaceHostView(
									LStream*		inStream);

							LInPlaceHostView();

	virtual					~LInPlaceHostView();

	// title attributes

	virtual StringPtr		GetDescriptor(
									Str255			outDescriptor) const;

	virtual void			SetDescriptor(
									ConstStringPtr	inDescriptor);

	virtual SInt32			GetValue() const;

	virtual void			SetValue(
									SInt32			inValue);

	inline MessageT			GetValueMessage() const
									{ return mValueMessage; }

	virtual void			SetValueMessage(
									MessageT		inValueMessage);

	inline ResIDT			GetTextTraitsID() const
									{ return mTextTraitsID; }

	inline void				SetTextTraitsID(ResIDT inTextTraitsID)
									{ mTextTraitsID = inTextTraitsID; }

	virtual Boolean			IsHitBy(
									SInt32			inHorizPort,
									SInt32			inVertPort);

	virtual void			CalcPortTextRect(
									Rect&			outFrame);

	// drawing behavior

protected:
	virtual void			DrawSelf();
	virtual void			RemoveSubPane(
									LPane*			inSub);

	// mouse-down behavior

	virtual void			ClickSelf(
									const SMouseDownEvent& inMouseDown);
	virtual void			SpawnInPlaceEditor();

	// cached text area

	virtual void			UpdateCachedFrame();
	virtual void			FinishCreateSelf();


	LStr255					mText;
	ResIDT					mTextTraitsID;
	Rect					mTextLocalFrame;
	SInt16					mMaxChars;
	UInt8					mEditFieldAttributes;
	TEKeyFilterFunc			mKeyFilter;
	MessageT				mValueMessage;

	LInPlaceEditField*		mEditField;


	// common initialization

private:
	void					InitHostView();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
