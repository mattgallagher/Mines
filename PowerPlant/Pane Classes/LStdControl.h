// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LStdControl.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LStdControl
#define _H_LStdControl
#pragma once

#include <LControl.h>
#include <Controls.h>
#include <ControlDefinitions.h>


#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

#pragma options align=mac68k

typedef struct	SCNTLResource {				// Toolbox CNTL Resource Format
	Rect		bounds;
	SInt16		value;
	SInt16		visible;
	SInt16		max;
	SInt16		min;
	UInt16		procID;
	SInt32		refCon;
	Str255		title;
} SCNTLResource, **SCNTLResourceH;

#pragma options align=reset


// ---------------------------------------------------------------------------
//	Universal Proc Ptr for Thumb Action Proc
//
//		The custom ThumbActionUPP and related symbols are obsolete.
//		Use DragGrayRgnUPP and related symbols defined in QuickDraw.h
//		[Change made in PP 2.1]

#if PP_Explain_Name_Changes

	#define ThumbActionFunc		$$ Use DragGrayRgnProcPtr instead of ThumbActionFunc

	#define	ThumbActionUPP		$$ Use DragGrayRgnUPP instead of ThumbActionUPP

	#define	CallThumbActionProc	$$ Use InvokeDragGrayRgnUPP instead of CallThumbActionProc

	#define	NewThumbActionProc	$$ Use NewDragGrayRgnUPP instead of NewThumbActionProc

	// For UPP's created with NewDragGrayRgnUPP, use DisposeDragGrayRgnUPP
	// to dispose them instead of DisposeRoutineDescriptor

#endif

// ---------------------------------------------------------------------------


class	LStdControl : public LControl {
public:
	enum { class_ID = FOUR_CHAR_CODE('sctl') };
	
	static LStdControl*	CreateFromCNTL(
								ResIDT			inCNTLid,
								MessageT		inValueMessage,
								ResIDT			inTextTraitsID,
								LView			*inSuperView);

						LStdControl();

						LStdControl(
								const LStdControl	&inOriginal);

						LStdControl(
								SInt16			inControlKind);

						LStdControl(
								const SPaneInfo	&inPaneInfo,
								MessageT		inValueMessage,
								SInt32			inValue,
								SInt32			inMinValue,
								SInt32			inMaxValue,
								SInt16			inControlKind,
								ResIDT			inTextTraitsID,
								ConstStringPtr	inTitle,
								SInt32			inMacRefCon);

						LStdControl(
								const SPaneInfo	&inPaneInfo,
								MessageT		inValueMessage,
								SInt32			inValue,
								SInt32			inMinValue,
								SInt32			inMaxValue,
								SInt16			inControlKind,
								ResIDT			inTextTraitsID,
								ControlHandle	inMacControlH);

						LStdControl(
								LStream			*inStream);

	virtual				~LStdControl();

	void				SetStdMinAndMax();
	ControlHandle		GetMacControl() const		{ return mMacControlH; }

	virtual void		ResizeFrameBy(
								SInt16			inWidthDelta,
								SInt16			inHeightDelta,
								Boolean			inRefresh);

	virtual void		MoveBy(	SInt32			inHorizDelta,
								SInt32			inVertDelta,
								Boolean			inRefresh);

	virtual StringPtr	GetDescriptor( Str255 outDescriptor ) const;
								
	virtual void		SetDescriptor( ConstStringPtr inDescriptor );

	virtual void		SetValue( SInt32 inValue );
	
	virtual void		SetMinValue( SInt32 inMinValue );
	
	virtual void		SetMaxValue( SInt32 inMaxValue );

	void				SynchValue();

	void				SetActionProc( ControlActionUPP inActionProc )
							{
								::SetControlAction(mMacControlH, inActionProc);
							}

	void				SetThumbFunc( DragGrayRgnUPP inThumbFunc )
							{
								mThumbFunc = inThumbFunc;
							}

	virtual void		SetTextTraitsID( ResIDT inTextTraitsID );

	ResIDT				GetTextTraitsID() const	{ return mTextTraitsID; }

	virtual Boolean		FocusDraw( LPane* inSubPane = nil );

	static LStdControl*	GetTrackingControl()	{ return sTrackingControl; }

protected:
	ControlHandle		mMacControlH;
	DragGrayRgnUPP		mThumbFunc;
	SInt16				mControlKind;
	ResIDT				mTextTraitsID;
	Boolean				mUsingBigValues;

	static LStdControl*	sTrackingControl;

	static Boolean		ValueIsInStdRange( SInt32 inValue );

	void				AlignControlRect();
	
	SInt16				CalcSmallValue( SInt32 inBigValue );
	
	SInt32				CalcBigValue( SInt16 inSmallValue );

	virtual SInt16		FindHotSpot( Point inPoint ) const;
	
	virtual Boolean		PointInHotSpot(
								Point			inPoint,
								SInt16			inHotSpot) const;
								
	virtual Boolean		TrackHotSpot(
								SInt16			inHotSpot,
								Point			inPoint,
								SInt16			inModifiers);
								
	virtual void		HotSpotAction(
								SInt16			inHotSpot,
								Boolean			inCurrInside,
								Boolean			inPrevInside);
								
	virtual void		HotSpotResult( SInt16 inHotSpot );

	virtual void		DrawSelf();
	virtual void		ShowSelf();
	virtual void		HideSelf();
	virtual void		EnableSelf();
	virtual void		DisableSelf();

	void				InitStdControl(
								SInt16			inControlKind,
								ResIDT			inTextTraitsID,
								ConstStringPtr	inTitle,
								SInt32			inMacRefCon);

};


// ===========================================================================
class	LStdButton : public LStdControl {
public:
	enum { class_ID = FOUR_CHAR_CODE('pbut') };

						LStdButton();
					
						LStdButton( const LStdButton& inOriginal );
					
						LStdButton(
								const SPaneInfo&	inPaneInfo,
								MessageT			inValueMessage,
								ResIDT				inTextTraitsID,
								Str255				inTitle);
							
						LStdButton(
								const SPaneInfo&	inPaneInfo,
								MessageT			inValueMessage,
								ResIDT				inTextTraitsID,
								ControlHandle		inMacControlH);
							
						LStdButton( LStream* inStream );

	virtual Boolean		CalcPortExposedRect(
								Rect&		outRect,
								bool		inOKIfHidden = false) const;

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

protected:
	bool				CalcPortStructureRect( Rect& outRect ) const;
	
	virtual void		DrawSelf();
	
	virtual void		HotSpotResult( SInt16 inHotSpot );
	
protected:
	bool		mIsDefault;
};


// ===========================================================================
class	LStdCheckBox : public LStdControl {
public:
	enum { class_ID = FOUR_CHAR_CODE('cbox') };

					LStdCheckBox();
					LStdCheckBox(
							const LStdCheckBox	&inOriginal);
					LStdCheckBox(
							const SPaneInfo		&inPaneInfo,
							MessageT			inValueMessage,
							SInt32				inValue,
							ResIDT				inTextTraitsID,
							Str255				inTitle);
					LStdCheckBox(
							const SPaneInfo		&inPaneInfo,
							MessageT			inValueMessage,
							SInt32				inValue,
							ResIDT				inTextTraitsID,
							ControlHandle		inMacControlH);
					LStdCheckBox(
							LStream				*inStream);

protected:
	virtual void	HotSpotResult(
							SInt16				inHotSpot);
};


// ===========================================================================
class	LStdRadioButton : public LStdControl {
public:
	enum { class_ID = FOUR_CHAR_CODE('rbut') };

					LStdRadioButton();
					LStdRadioButton(
							const SPaneInfo		&inPaneInfo,
							MessageT			inValueMessage,
							SInt32				inValue,
							ResIDT				inTextTraitsID,
							Str255				inTitle);
					LStdRadioButton(
							const SPaneInfo		&inPaneInfo,
							MessageT			inValueMessage,
							SInt32				inValue,
							ResIDT				inTextTraitsID,
							ControlHandle		inMacControlH);
					LStdRadioButton(
							const LStdRadioButton	&inOriginal);
					LStdRadioButton(
							LStream				*inStream);

	virtual void	SetValue(
							SInt32				inValue);

protected:
	virtual void	HotSpotResult(
							SInt16				inHotSpot);
};


// ===========================================================================
class	LStdPopupMenu : public LStdControl {
public:
	enum { class_ID = FOUR_CHAR_CODE('popm') };
					LStdPopupMenu(
							const SPaneInfo		&inPaneInfo,
							MessageT			inValueMessage,
							SInt16				inTitleOptions,
							ResIDT				inMENUid,
							SInt16				inTitleWidth,
							SInt16				inPopupVariation,
							ResIDT				inTextTraitsID,
							Str255				inTitle,
							OSType				inResTypeMENU,
							SInt16				inInitialMenuItem);
					LStdPopupMenu(
							const SPaneInfo		&inPaneInfo,
							MessageT			inValueMessage,
							SInt32				inMaxValue,
							ResIDT				inTextTraitsID,
							ControlHandle		inMacControlH);
					LStdPopupMenu(
							LStream				*inStream);

	virtual			~LStdPopupMenu();

	virtual MenuHandle	GetMacMenuH() const;

protected:
	virtual Boolean	TrackHotSpot(
							SInt16				inHotSpot,
							Point				inPoint,
							SInt16				inModifiers);

	virtual void	DrawSelf();

private:
	void			InitStdPopupMenu(
							SInt16				inInitialMenuItem);

					LStdPopupMenu(				// Disallow copy constructor
							const LStdPopupMenu		&inOriginal);
};


// ===========================================================================
class	LDefaultOutline : public LPane {
public:
				LDefaultOutline(LPane *inHostPane);
protected:
	virtual void	DrawSelf();
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
