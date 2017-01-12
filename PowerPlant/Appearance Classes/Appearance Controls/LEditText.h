// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LEditText.h					PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LEditText
#define _H_LEditText
#pragma once

#include <LControlPane.h>
#include <LCommander.h>
#include <LPeriodical.h>
#include <UKeyFilters.h>

#include <TextEdit.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LTETypingAction;
class	LString;

// ---------------------------------------------------------------------------

class LEditText : public LControlPane,
				  public LCommander,
				  public LPeriodical {

public:
	enum {	class_ID		= FOUR_CHAR_CODE('etxt'),
			imp_class_ID	= FOUR_CHAR_CODE('ietx') };

						LEditText(
								LStream*		inStream,
								ClassIDT		inImpID = imp_class_ID);

						LEditText(
								const SPaneInfo&	inPaneInfo,
								LCommander*			inSuperCommander,
								ConstStringPtr		inInitialText,
								ResIDT				inTextTraitsID,
								MessageT			inMessage,
								SInt16				inMaxChars,
								UInt8				inAttributes,
								TEKeyFilterFunc		inKeyFilter,
								bool				inPasswordField = false,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LEditText();

	virtual void		SetDescriptor( ConstStringPtr inDescriptor );

	virtual StringPtr	GetDescriptor( Str255 outDescriptor ) const;

	virtual void		SetValue( SInt32 inValue );

	virtual SInt32		GetValue() const;

	void				SetText(
								const void*		inTextPtr,
								Size			inTextLength);

	void				SetText(
								const LString&	inString);

	void				GetText(
								Ptr				outBuffer,
								Size			inBufferLength,
								Size*			outTextLength) const;

	void				GetText( LString& outString ) const;

	virtual void		SetTextTraitsID( ResIDT inTextTraitsID );
	virtual ResIDT		GetTextTraitsID() const;

	void				SetMacTEH( TEHandle inMacTEH )
							{
								mTextEditH = inMacTEH;
							}

	TEHandle			GetMacTEH() const		{ return mTextEditH; }

	void				SetSelection( ControlEditTextSelectionRec& inSelection );

	void				SelectAll();

	virtual void		GetSelection( ControlEditTextSelectionRec& outSelection ) const;

	virtual	void		GetSelection( AEDesc& outDesc ) const;
	
	virtual bool		HasSelection() const;

	TEKeyFilterFunc		GetKeyFilter() const	{ return mKeyFilter; }

	void				SetKeyFilter( TEKeyFilterFunc inKeyFilter )
							{
								mKeyFilter = inKeyFilter;
							}

	SInt16				GetMaxChars() const		{ return mMaxChars; }

	void				SetMaxChars( SInt16 inMaxChars )
							{
								mMaxChars = inMaxChars;
							}

	bool				HasFocusRing() const	{ return mHasFocusRing; }

	static void			SetPasswordChar( UInt8 inChar );

	virtual void		SetDataTag(
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inDataSize,
								void*			inDataPtr);

	virtual void		GetDataTag(
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inBufferSize,
								void*			inBuffer,
								Size*			outDataSize = nil) const;

	virtual Boolean		FocusDraw( LPane* inSubPane = nil );

	virtual void		ResizeFrameBy(
								SInt16		inWidthDelta,
								SInt16		inHeightDelta,
								Boolean		inRefresh);

	virtual void		MoveBy(	SInt32		inHorizDelta,
								SInt32		inVertDelta,
								Boolean		inRefresh);

	virtual Boolean		HandleKeyPress( const EventRecord& inKeyEvent );

	virtual Boolean		ObeyCommand(
								CommandT			inCommand,
								void*				ioParam);

	virtual void		FindCommandStatus(
								CommandT			inCommand,
								Boolean&			outEnabled,
								Boolean&			outUsesMark,
								UInt16&				outMark,
								Str255				outName);

	virtual void		UserChangedText();

	virtual void		RefreshText();

	virtual void		SpendTime( const EventRecord& inMacEvent );

protected:
	static CQDProcs		sPasswordProcs;
	static bool			sProcsInited;
	static UInt8		sPasswordChars[4];

	TEHandle			mTextEditH;
	TEKeyFilterFunc		mKeyFilter;
	LTETypingAction		*mTypingAction;
	SInt16				mMaxChars;
	ResIDT				mTextTraitsID;
	bool				mHasWordWrap;
	bool				mIsPassword;
	bool				mHasFocusRing;

	void				InitEditText(
								ConstStringPtr		inText,
								ResIDT				inTextTraitsID,
								UInt8				inAttributes);

	virtual void		DrawSelf();
	virtual void		HideSelf();

	virtual void		ActivateSelf();
	virtual void		DeactivateSelf();

	virtual void		EnableSelf();
	virtual void		DisableSelf();

	virtual void		ClickSelf( const SMouseDownEvent& inMouseDown );

	virtual void		AdjustMouseSelf(
								Point				inPortPt,
								const EventRecord&	inMacEvent,
								RgnHandle			outMouseRgn);

	virtual void		BeTarget();
	virtual void		DontBeTarget();
	virtual void		RedrawImp();

	virtual void		AlignTextEditRects();
	virtual void		AdjustTextWidth( bool inShrinkToText );

	virtual Boolean		TooManyCharacters( SInt32 inCharsToAdd );

	static pascal void	PasswordStdText(
								SInt16		inByteCount,
								const void*	inTextPtr,
								Point		inNumer,
								Point		inDenom);

	static pascal SInt16	PasswordStdTxMeas(
								SInt16		inByteCount,
								const void*	inTextPtr,
								Point*		ioNumer,
								Point*		ioDenom,
								FontInfo*	inFontInfo);

private:
						LEditText();
						LEditText( const LEditText& );
	LEditText&			operator = ( const LEditText& );

};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
