// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LEditField.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A Pane containing editable text

#ifndef _H_LEditField
#define _H_LEditField
#pragma once

#include <LCommander.h>
#include <LPane.h>
#include <LPeriodical.h>
#include <UKeyFilters.h>

#include <TextEdit.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LTETypingAction;

enum {
	editAttr_Box			= 0x80,
	editAttr_WordWrap		= 0x40,
	editAttr_AutoScroll		= 0x20,
	editAttr_TextBuffer		= 0x10,
	editAttr_OutlineHilite	= 0x08,
	editAttr_InlineInput	= 0x04,
	editAttr_TextServices	= 0x02
};

// ---------------------------------------------------------------------------

class LEditField : public LPane,
				   public LCommander,
				   public LPeriodical {
public:
	enum { class_ID = FOUR_CHAR_CODE('edit') };

						LEditField();
						
						LEditField( const LEditField& inOriginal );
						LEditField(
								const SPaneInfo&	inPaneInfo,
								ConstStringPtr		inString,
								ResIDT				inTextTraitsID,
								SInt16				inMaxChars,
								Boolean				inHasBox,
								Boolean				inHasWordWrap,
								TEKeyFilterFunc		inKeyFilter,
								LCommander*			inSuper);
								
						LEditField(
								const SPaneInfo&	inPaneInfo,
								ConstStringPtr		inString,
								ResIDT				inTextTraitsID,
								SInt16				inMaxChars,
								UInt8				inAttributes,
								TEKeyFilterFunc		inKeyFilter,
								LCommander*			inSuper);
								
						LEditField( LStream* inStream );
						
	virtual				~LEditField();

	virtual SInt32		GetValue() const;
	
	virtual void		SetValue( SInt32 inValue );

	virtual StringPtr	GetDescriptor( Str255 outDescriptor ) const;
	
	virtual void		SetDescriptor( ConstStringPtr inDescriptor );

	TEHandle			GetMacTEH()			{ return mTextEditH; }

	virtual void		SetMaxChars( SInt16 inMaxChars );
	
	virtual void		SetTextTraitsID( ResIDT	 inTextTraitsID );
	
	virtual void		UseWordWrap( Boolean inSetting );
	
	virtual bool		HasSelection() const;

	virtual Boolean		ObeyCommand(
								CommandT			inCommand,
								void*				ioParam);
								
	virtual void		FindCommandStatus(
								CommandT			inCommand,
								Boolean&			outEnabled,
								Boolean&			outUsesMark,
								UInt16&				outMark,
								Str255				outName);
								
	virtual	void		SpendTime( const EventRecord& inMacEvent );

	TEKeyFilterFunc		GetKeyFilter() const		{ return mKeyFilter; }

	virtual void		SetKeyFilter( TEKeyFilterFunc inKeyFilter );

	virtual Boolean		HandleKeyPress( const EventRecord& inKeyEvent );

	virtual void		ResizeFrameBy(
								SInt16				inWidthDelta,
								SInt16				inHeightDelta,
								Boolean				inRefresh);

	virtual void		MoveBy(	SInt32				inHorizDelta,
								SInt32				inVertDelta,
								Boolean				inRefresh);

	virtual Boolean		FocusDraw( LPane* inSubPane = nil );
	
	virtual void		SelectAll();
	
	virtual void		UserChangedText();

	virtual	void		GetSelection( AEDesc& outDesc ) const;

	virtual void		SavePlace( LStream* outPlace );

	virtual void		RestorePlace( LStream* inPlace );

protected:
	TEHandle			mTextEditH;
	TEKeyFilterFunc		mKeyFilter;
	LTETypingAction*	mTypingAction;
	SInt16				mMaxChars;
	ResIDT				mTextTraitsID;
	Boolean				mHasBox;
	Boolean				mHasWordWrap;

	virtual void		DrawSelf();
	virtual void		DrawBox();

	virtual void		ClickSelf( const SMouseDownEvent& inMouseDown );

	virtual void		AdjustMouseSelf(
								Point				inPortPt,
								const EventRecord&	inMacEvent,
								RgnHandle			outMouseRgn);

	virtual void		HideSelf();

	virtual void		EnableSelf();
	virtual void		DisableSelf();

	virtual void		BeTarget();
	virtual void		DontBeTarget();

	virtual void		AlignTextEditRects();
	virtual void		AdjustTextWidth( Boolean inShrinkToText );

	virtual Boolean		TooManyCharacters( SInt32 inCharsToAdd );

private:
	void				InitEditField( UInt8 inAttributes );
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
