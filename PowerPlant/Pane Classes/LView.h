// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LView.h						PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LView
#define _H_LView
#pragma once

#include <LPane.h>
#include <TArray.h>
#include <URegions.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

#pragma options align=mac68k

typedef struct	SViewInfo {
	SDimension32	imageSize;
	SPoint32		scrollPos;
	SPoint32		scrollUnit;
	SInt16			reconcileOverhang;
} SViewInfo;

#pragma options align=reset

// ---------------------------------------------------------------------------

class LView : public LPane {
public:
	enum { class_ID = FOUR_CHAR_CODE('view') };

						LView();
						
						LView(	const LView&		inOriginal);
						
						LView(	const SPaneInfo&	inPaneInfo,
								const SViewInfo&	inViewInfo);
								
						LView(	LStream				*inStream);
						
						LView(	LStream*			inStream,
								bool				inNoViewData);
								
	virtual				~LView();

	virtual void		FinishCreate();
	
	virtual void		GetImageSize( SDimension32& outSize ) const;

	void				GetImageLocation( SPoint32& outLocation ) const
							{
								outLocation = mImageLocation;	// Port Coords
							}

	virtual void		GetScrollPosition( SPoint32& outScrollPosition ) const;

	void				GetRevealedRect( Rect& outRect ) const
							{
								outRect = mRevealedRect;	// Port Coords
							}

	virtual void		CalcRevealedRect();

	virtual void		CalcPortOrigin();
	
	virtual void		GetPortOrigin( Point& outOrigin ) const;

	virtual void		ResizeFrameBy(
								SInt16				inWidthDelta,
								SInt16				inHeightDelta,
								Boolean				inRefresh);

	virtual void		MoveBy(	SInt32				inHorizDelta,
								SInt32				inVertDelta,
								Boolean				inRefresh);

	virtual void		SavePlace( LStream* outPlace );
								
	virtual void		RestorePlace( LStream* inPlace );

	virtual void		AdaptToNewSurroundings();
	
	virtual void		AdaptToSuperFrameSize(
								SInt32				inSurrWidthDelta,
								SInt32				inSurrHeightDelta,
								Boolean				inRefresh);

	virtual void		Show();
	virtual void		Hide();

	virtual void		Activate();
	virtual void		Deactivate();

	virtual void		Enable();
	virtual void		Disable();

	virtual Boolean		EstablishPort();
	virtual void		Refresh();
	virtual void		DontRefresh( Boolean inOKIfHidden = false );
	
	static void			OutOfFocus( LView* inView );
	
	virtual Boolean		FocusDraw( LPane* inSubPane = nil );
	
	virtual Boolean		FocusExposed( Boolean inAlwaysFocus = false );

	virtual void		Draw( RgnHandle inSuperDrawRgnH );
	
	virtual RgnHandle	GetLocalUpdateRgn();
	
	void				SetReconcileOverhang( Boolean inSetting );

	virtual void		CountPanels(
								UInt32&				outHorizPanels,
								UInt32&				outVertPanels);
								
	virtual void		PrintPanel(
								const PanelSpec&	inPanel,
								RgnHandle			inSuperPrintRgnH);
								
	virtual void		SuperPrintPanel(
								const PanelSpec&	inSuperPanel,
								RgnHandle			inSuperPrintRgnH);

	virtual Boolean		ScrollToPanel( const PanelSpec& inPanel );
	
	virtual void		FindPageBreak(
								const SPoint32&		inTopLeft,
								SInt16&				ioHeight,
								SInt16&				ioWidth);

	void				ScrollImageTo(
								SInt32				inLeftLocation,
								SInt32				inTopLocation,
								Boolean				inRefresh);

	virtual void		ScrollImageBy(
								SInt32				inLeftDelta,
								SInt32				inTopDelta,
								Boolean				inRefresh);

	Boolean				ScrollPinnedImageTo(
								SInt32				inLeftLocation,
								SInt32				inTopLocation,
								Boolean				inRefresh);

	virtual Boolean		ScrollPinnedImageBy(
								SInt32				inLeftDelta,
								SInt32				inTopDelta,
								Boolean				inRefresh);

	virtual SInt32		CalcPinnedScrollDelta(
								SInt32				inDelta,
								SInt32				inFrameMin,
								SInt32				inFrameMax,
								SInt32				inImageMin,
								SInt32				inImageMax) const;

	virtual void		ScrollBits(
								SInt32				inLeftDelta,
								SInt32				inTopDelta);

	virtual Boolean		AutoScrollImage( Point inLocalPt );

	void				ResizeImageTo(
								SInt32				inWidth,
								SInt32				inHeight,
								Boolean				inRefresh);
								
	virtual void		ResizeImageBy(
								SInt32				inWidthDelta,
								SInt32				inHeightDelta,
								Boolean				inRefresh);

	void				SetScrollUnit( const SPoint32& inScrollUnit );

	void				GetScrollUnit( SPoint32& outScrollUnit ) const;

	virtual void		SubImageChanged( LView* inSubView );

	virtual LPane*		FindSubPaneHitBy(
								SInt32				inHoriz,
								SInt32				inVert) const;
								
	virtual LPane*		FindDeepSubPaneContaining(
								SInt32				inHorizPort,
								SInt32				inVertPort) const;
								
	virtual LPane*		FindShallowSubPaneContaining(
								SInt32				inHorizPort,
								SInt32				inVertPort) const;

	virtual void		Click( SMouseDownEvent& inMouseDown );
								
	virtual void		AdjustMouse(
								Point				inPortPt,
								const EventRecord&	inMacEvent,
								RgnHandle			outMouseRgn);

	virtual LPane*		FindPaneByID( PaneIDT inPaneID );

	virtual const LPane* FindConstPaneByID( PaneIDT inPaneID ) const;

	virtual SInt32		GetValueForPaneID( PaneIDT inPaneID ) const;
								
	virtual void		SetValueForPaneID(
								PaneIDT				inPaneID,
								SInt32				inValue);

	virtual StringPtr	GetDescriptorForPaneID(
								PaneIDT				inPaneID,
								Str255				outDescriptor) const;
								
	virtual void		SetDescriptorForPaneID(
								PaneIDT				inPaneID,
								ConstStringPtr		inDescriptor);

	virtual void		PortToLocalPoint( Point& ioPoint ) const;
								
	virtual void		LocalToPortPoint( Point& ioPoint ) const;

	void				ImageToLocalPoint(
								const SPoint32&		inImagePt,
								Point&				outLocalPt) const;
								
	void				LocalToImagePoint(
								const Point&		inLocalPt,
								SPoint32&			outImagePt) const;

	Boolean				ImagePointIsInFrame(
								SInt32				inHorizImage,
								SInt32				inVertImage) const;
								
	Boolean				ImageRectIntersectsFrame(
								SInt32				inLeftImage,
								SInt32				inTopImage,
								SInt32				inRightImage,
								SInt32				inBottomImage) const;

	virtual void		AddSubPane( LPane* inSub );
	
	virtual void		RemoveSubPane( LPane* inSub );
	
	void				DeleteAllSubPanes();
	
	void				OrientAllSubPanes();
	
	virtual void		OrientSubPane( LPane* inSub );

	TArray<LPane*>&		GetSubPanes()			{ return mSubPanes; }

	virtual void		ExpandSubPane(
								LPane*				inSub,
								Boolean				inExpandHoriz,
								Boolean				inExpandVert);

	static LView*		GetInFocusView()		{ return sInFocusView; }

protected:
	static LView*	sInFocusView;			// View currently in focus

	SDimension32	mImageSize;
	SPoint32		mImageLocation;
	SPoint32		mScrollUnit;
	Point			mPortOrigin;
	TArray<LPane*>	mSubPanes;
	Rect			mRevealedRect;			// In Port coords
	StRegion		mUpdateRgn;
	Boolean			mReconcileOverhang;

	virtual void		ReconcileFrameAndImage( Boolean inRefresh );

	virtual void		SuperShow();
	virtual void		SuperHide();

	virtual void		SuperActivate();
	virtual void		SuperDeactivate();

	virtual void		SuperEnable();
	virtual void		SuperDisable();

protected:
	void				InitView( const SViewInfo& inViewInfo );
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
