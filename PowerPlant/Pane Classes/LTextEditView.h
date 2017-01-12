// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTextEditView.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub

#ifndef _H_LTextEditView
#define _H_LTextEditView
#pragma once

#include <LCommander.h>
#include <LPeriodical.h>
#include <LView.h>
#include <UTEViewTextAction.h>
#include <UTextEdit.h>

#include <TextEdit.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

enum {
	err_32kLimit		= FOUR_CHAR_CODE('32k ')
};

class LTENewTypingAction;

// ---------------------------------------------------------------------------

class LTextEditView :	public LView,
						public LCommander,
						public LPeriodical {
public:
	enum { class_ID = FOUR_CHAR_CODE('txtv') };

	enum { kMaxChars = 31 * 1024L };	// Just under 32K

							LTextEditView();
							LTextEditView(
									const SPaneInfo&	inPaneInfo,
									const SViewInfo&	inViewInfo,
									UInt16				inTextAttributes,
									ResIDT				inTextTraitsID );
							LTextEditView(
									LStream*			inStream );
	virtual					~LTextEditView();

	virtual Boolean			HandleKeyPress(
									const EventRecord& 	inKeyEvent);
	virtual Boolean			ObeyCommand(
									CommandT			inCommand,
									void*				ioParam);
	virtual void			FindCommandStatus(
									CommandT			inCommand,
									Boolean&			outEnabled,
									Boolean&			outUsesMark,
									UInt16&				outMark,
									Str255				outName);

	virtual	void			SpendTime(
									const EventRecord&	inMacEvent);

	virtual void			AlignTextEditRects();
	virtual void			ResizeFrameBy(
									SInt16				inWidthDelta,
									SInt16				inHeightDelta,
									Boolean				inRefresh);
	virtual void			MoveBy(	SInt32				inHorizDelta,
									SInt32				inVertDelta,
									Boolean				inRefresh);
	virtual void			ScrollImageBy(
									SInt32				inLeftDelta,
									SInt32				inTopDelta,
									Boolean				inRefresh);

#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
		// Click Loop callback has different prototype for Classic 68K

	static pascal Boolean	MyClickLoop();
	virtual void			SetClickLoop( void* inClickLoop );

#else

	static pascal Boolean	MyClickLoop(TERec* inTERec);
	virtual	void			SetClickLoop( TEClickLoopProcPtr inClickLoop );

#endif

	virtual	void			ForceAutoScroll(
									const Rect&			inOldDestRect);
	virtual	void			ClickAutoScroll(
									const Point&		inLocalPoint);

	virtual SInt32			GetValue() const;
	virtual void			SetValue( SInt32 inValue );

	virtual StringPtr		GetDescriptor( Str255 outDescriptor ) const;
	virtual void			SetDescriptor( ConstStringPtr inDescriptor );

	virtual void			SetTextHandle(
									Handle				inTextH,
									StScrpHandle		inStyleH = nil );
	virtual void			SetTextPtr(
									const void*			inTextP,
									SInt32				inTextLen,
									StScrpHandle 		inStyleH = nil);
			Handle			GetTextHandle();
			TEHandle		GetMacTEH();

	virtual	void			Insert(	const void*			inText,
									SInt32				inLength,
									StScrpHandle		inStyleH = nil,
									Boolean				inRefresh = false );

			ResIDT			GetTextTraitsID() const;
	virtual void			SetTextTraitsID( ResIDT inTextTraitsID );

			Boolean			HasAttribute( UInt16 inAttribute ) const;
			UInt16			GetAttributes() const;
			void			SetAttributes( UInt16 inAttributes );
			void			ToggleAttribute(
									UInt16				inAttribute,
									bool				inSetting );

	virtual	SInt32			CalcTEHeight();
	virtual	Boolean			TETooBig();

	virtual Boolean			FocusDraw( LPane* inSubPane = nil);

	virtual void			SelectAll();
	virtual void			UserChangedText();
	virtual void			AdjustImageToText();

	virtual	void			GetSelection( AEDesc& outDesc) const;

	virtual void			SetSelectionRange(
									SInt16				inStartPos,
									SInt16				inEndPos);
									
	virtual bool			HasSelection() const;

	virtual void			RestorePlace( LStream* inPlace );

	virtual	void			SetFont( SInt16 inFontNumber );
	
	virtual	void			SetFont( ConstStringPtr inFontName );
	
	virtual	Boolean			GetFont( SInt16& outFontNum );
	virtual	Boolean			GetFont( Str255 outName );

	virtual	void			SetSize( SInt16 inSize );
	virtual Boolean			GetSize( SInt16& outSize );
	virtual Boolean			ChangeFontSizeBy( SInt16 inDeltaSize );

	virtual	void			SetStyle( Style inStyle );
	virtual	Boolean			GetStyle( Style& outStyle );
	virtual	void			ToggleStyleAttribute( Style inStyleAttr );

	virtual	void			SetAlignment( SInt16 inAlign );
	virtual	SInt16			GetAlignment();

	virtual	void			SetColor( const RGBColor& inColor );
	virtual	Boolean			GetColor( RGBColor& outColor );

	static	LTextEditView	*sTextEditViewP;

protected:

			LTEViewTypingAction*	mTypingAction;
			TEHandle				mTextEditH;
			ResIDT					mTextTraitsID;
			UInt16					mTextAttributes;
			TEClickLoopUPP			mClickLoopUPP;

	virtual void			DrawSelf();
	
	virtual void			PrintPanelSelf( const PanelSpec& inPanel );
	
	virtual void			HideSelf();

	virtual void			ClickSelf(
									const SMouseDownEvent&	inMouseDown);
									
	virtual void			AdjustMouseSelf(
									Point				inPortPt,
									const EventRecord&	inMacEvent,
									RgnHandle			outMouseRgn);

	virtual void			BeTarget();
	virtual void			DontBeTarget();

	virtual STextEditUndoH	SaveStateForUndo();

private:
			void			InitTextEditView(
									ResIDT				inTextTraitsID );

			// defensive programming

							LTextEditView(
									const LTextEditView&	inOriginal );
			LTextEditView&	operator=(
									const LTextEditView&	inOriginal );
};


//================== INLINES =====================

// ---------------------------------------------------------------------------
//	¥ GetMacTEH								[public]
// ---------------------------------------------------------------------------
//	Return a Handle to the Mac TextEdit Record associated with an LTextEditView
//
//	Caller may change record fields, and is responsible for redrawing the
//	TextEdit as necessary to reflect any changes. However, caller must
//	not dispose of the TEHandle.

inline
TEHandle
LTextEditView::GetMacTEH()
{
	return mTextEditH;
}

// ---------------------------------------------------------------------------
//	¥ GetTextTraitsID						[public]
// ---------------------------------------------------------------------------

inline
ResIDT
LTextEditView::GetTextTraitsID() const
{
	return mTextTraitsID;
}


// ---------------------------------------------------------------------------
//	¥ HasAttribute							[public]
// ---------------------------------------------------------------------------

inline
Boolean
LTextEditView::HasAttribute(
	UInt16 inAttribute ) const
{
	return ((mTextAttributes & inAttribute) != 0);
}


// ---------------------------------------------------------------------------
// 	¥ GetAttributes							[public]
// ---------------------------------------------------------------------------
//	Returns the raw attribute flags. Treat as read-only

inline
UInt16
LTextEditView::GetAttributes() const
{
	return mTextAttributes;
}

// ---------------------------------------------------------------------------
//	¥ SetAttributes							[public]
// ---------------------------------------------------------------------------
//	Allows the setting of the attributes. This does not toggle attributes
//	but rather replaces all existing attributes with the value passed as
//	argument. If you wish to change a single attribute, use ToggleAttribute
//	instead.
//
//	Be warned that changing some attribute bits could have adverse effects
//	at runtime or no effects at all (e.g. turning off the multistyle flag
//	will not change the object to be monostyled).

inline
void
LTextEditView::SetAttributes(
	UInt16	inAttribute )
{
	mTextAttributes = inAttribute;
}


// ---------------------------------------------------------------------------
//	¥ ToggleAttribute						[public]
// ---------------------------------------------------------------------------
//	Allows the toggling of an attribute setting. If you wish to change all
//	attributes in one call, use SetAttributes instead.
//
//	Be warned that changing some attribute bits could have adverse effects
//	at runtime or no effects at all (e.g. turning off the multistyle flag
//	will not change the object to be monostyled).

inline
void
LTextEditView::ToggleAttribute(
	UInt16		inAttribute,
	bool		inSetting )
{
	if (inSetting) {
		mTextAttributes |= inAttribute;
	} else {
		mTextAttributes &= ~inAttribute;
	}
}


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
