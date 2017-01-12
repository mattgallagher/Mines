// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPane.h						PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LPane
#define _H_LPane
#pragma once

#include <LAttachable.h>
#include <Events.h>
#include <AEDataModel.h>
#include <CFString.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LView;
class	LStream;

struct	SDimension16	{
	SInt16	width;
	SInt16	height;
};

struct	SDimension32	{
	SInt32	width;
	SInt32	height;
};

typedef Point	SPoint16;

struct	SPoint32	{
	SInt32	h;
	SInt32	v;
};

struct	SRect32  {
	SInt32	left;
	SInt32	top;
	SInt32	right;
	SInt32	bottom;
};

struct	SMouseDownEvent	{
	Point		wherePort;
	Point		whereLocal;
	EventRecord	macEvent;
	Boolean		delaySelect;
};

struct SAdjustMouseEvent {
	EventRecord		event;
	RgnHandle		mouseRgn;
};

struct PanelSpec {
	UInt32	horizIndex;
	UInt32	vertIndex;
	UInt32	pageNumber;
};

struct PanePlace {
	SDimension16	frameSize;
	SPoint32		frameLocation;
};

//#pragma options align=mac68k

struct	SBooleanRect	{
	Boolean	left;
	Boolean	top;
	Boolean	right;
	Boolean	bottom;
};

struct SPaneInfo {
	PaneIDT			paneID;
	SInt16			width;
	SInt16			height;
	Boolean			visible;
	Boolean			enabled;
	SBooleanRect	bindings;
	SInt32			left;
	SInt32			top;
	SInt32			userCon;
	LView*			superView;
};

//#pragma options align=reset


const SInt16	min_QDCoord	= -32768;
const SInt16	max_QDCoord = 32767;

#define			Default_SuperView	((LView*) -1)

// ---------------------------------------------------------------------------
//	Flag for defining functions to copy/set descriptor using a CFString
//
//	CFString requires Carbon

#define			PP_Uses_CFDescriptor		TARGET_API_MAC_CARBON


// ---------------------------------------------------------------------------

class LPane : public virtual LAttachable {
	friend class LView;
public:
	enum { class_ID = FOUR_CHAR_CODE('pane') };

						LPane();
						LPane(	const LPane		&inOriginal);
						LPane(	const SPaneInfo	&inPaneInfo);
						LPane(	LStream			*inStream);

	virtual				~LPane();

	virtual void		FinishCreate();
	PaneIDT				GetPaneID() const	{ return mPaneID; }

	void				SetPaneID( PaneIDT inPaneID )
							{
								mPaneID = inPaneID;
							}

	virtual LPane*		FindPaneByID( PaneIDT inPaneID );
	
	virtual const LPane*	FindConstPaneByID( PaneIDT inPaneID ) const;

	static LView*		GetDefaultView()	{ return sDefaultView; }

	static void			SetDefaultView( LView *inView )
							{
								sDefaultView = inView;
							}

	virtual GrafPtr		GetMacPort() const;

	virtual WindowPtr	GetMacWindow() const;
	
	virtual CGContextRef	GetCGContext() const;

	virtual SInt32		GetValue() const;
	virtual void		SetValue( SInt32 inValue );

	virtual StringPtr	GetDescriptor( Str255 outDescriptor ) const;
	virtual void		SetDescriptor( ConstStringPtr inDescriptor );
	
	#if PP_Uses_CFDescriptor
	
	virtual CFStringRef		CopyCFDescriptor() const;
	virtual void			SetCFDescriptor( CFStringRef inStringRef );
	
	#endif

	virtual SInt32		GetUserCon() const;
	virtual void		SetUserCon( SInt32 inUserCon );

	void				SetRefreshAllWhenResized( Boolean inRefreshAll )
							{
								mRefreshAllWhenResized = inRefreshAll;
							}

	virtual	void		GetSelection( AEDesc& outDesc ) const;

	virtual SInt32		Perform(
								MessageT	inMessage,
								void		*ioParam);

		// ¥ Frame Management

	void				GetFrameSize( SDimension16& outSize ) const;
	
	void				GetFrameLocation( SPoint32& outLocation ) const;

	void				SetFrameBinding( const SBooleanRect& inFrameBinding );
	
	void				GetFrameBinding( SBooleanRect &outFrameBinding ) const;

	virtual Boolean		CalcPortFrameRect( Rect& outRect ) const;

	virtual Boolean		CalcLocalFrameRect( Rect& outRect ) const;

	virtual Boolean		CalcPortExposedRect(
								Rect&		outRect,
								bool		inOKIfHidden = false) const;

	void				ResizeFrameTo(
								SInt16		inWidth,
								SInt16		inHeight,
								Boolean		inRefresh);
	virtual void		ResizeFrameBy(
								SInt16		inWidthDelta,
								SInt16		inHeightDelta,
								Boolean		inRefresh);

	virtual void		MoveBy(	SInt32		inHorizDelta,
								SInt32		inVertDelta,
								Boolean		inRefresh);

	void				PlaceInSuperFrameAt(
								SInt32		inHorizOffset,
								SInt32		inVertOffset,
								Boolean		inRefresh);
	virtual void		PlaceInSuperImageAt(
								SInt32		inHorizOffset,
								SInt32		inVertOffset,
								Boolean		inRefresh);

	virtual void		SavePlace( LStream* outPlace );
	
	virtual void		RestorePlace( LStream* inPlace );

	void				PutInside(
								LView*		inView,
								Boolean		inOrient = true);

	LView*				GetSuperView() const	{ return mSuperView; }

		// ¥ Adapting to changes in SuperView

	virtual void		AdaptToNewSurroundings();
	virtual void		AdaptToSuperFrameSize(
								SInt32		inSurrWidthDelta,
								SInt32		inSurrHeightDelta,
								Boolean		inRefresh);
	virtual void		AdaptToSuperScroll(
								SInt32		inHorizScroll,
								SInt32		inVertScroll);

		// ¥ Hit Detection

	virtual Boolean		PointIsInFrame(
								SInt32		inHorizPort,
								SInt32		inVertPort) const;
								
	virtual Boolean		IsHitBy(SInt32		inHorizPort,
								SInt32		inVertPort);
								
	virtual LPane*		FindSubPaneHitBy(
								SInt32		inHorizPort,
								SInt32		inVertPort) const;

	virtual Boolean		Contains(
								SInt32		inHorizPort,
								SInt32		inVertPort) const;
								
	virtual LPane*		FindDeepSubPaneContaining(
								SInt32		inHorizPort,
								SInt32		inVertPort) const;
								
	virtual LPane*		FindShallowSubPaneContaining(
								SInt32		inHorizPort,
								SInt32		inVertPort) const;

		// ¥ Mousing

	virtual void		Click( SMouseDownEvent& inMouseDown );
	
	virtual void		ClickSelf( const SMouseDownEvent& inMouseDown );
								
	virtual void		EventMouseUp( const EventRecord& inMacEvent );
								
	virtual void		UpdateClickCount(
								const SMouseDownEvent& inMouseDown);
								
	virtual void		AdjustMouse(
								Point				inPortPt,
								const EventRecord&	inMacEvent,
								RgnHandle			outMouseRgn);
								
	virtual void		AdjustMouseSelf(
								Point				inPortPt,
								const EventRecord&	inMacEvent,
								RgnHandle			outMouseRgn);

	virtual void		MouseEnter(
								Point				inPortPt,
								const EventRecord&	inMacEvent);
								
	virtual void		MouseLeave();
	
	virtual void		MouseWithin(
								Point				inPortPt,
								const EventRecord&	inMacEvent);

	virtual Boolean		PointsAreClose(
								Point 				inFirstPt,
								Point				inSecondPt) const;
								
	virtual Boolean		ClickTimesAreClose( UInt32 inLastClickTime ) const;

	static LPane*		GetLastPaneClicked()	{ return sLastPaneClicked; }

#if PP_Uses_ContextMenus
	virtual OSStatus	ContextClick ( Point inGlobalPt );
	
	virtual bool		ContextClickSelf ( Point inGlobalPt );
#endif

		// ¥ Showing

	Boolean				IsVisible() const			{ return (mVisible == triState_On); }
	ETriState			GetVisibleState() const		{ return mVisible; }
	void				SetVisibleState(
								ETriState	inState)
							{
								mVisible = inState;
							}
							
	virtual void		Show();
	virtual void		Hide();
	void				SetVisible( bool inMakeVisible );

		// ¥ Activating

	Boolean				IsActive() const;
	ETriState			GetActiveState() const		{ return mActive; }
	
	virtual void		Activate();
	virtual void		Deactivate();
	void				SetActive( bool inMakeActive );

		// ¥ Enabling

	Boolean				IsEnabled() const;
	ETriState			GetEnabledState() const		{ return mEnabled; }
	
	virtual void		Enable();
	virtual void		Disable();
	void				SetEnabled( bool inMakeEnabled );

		// ¥ Refreshing

	virtual void		Refresh();
	virtual void		RefreshRect(
								const Rect&	inRect);
	virtual void		RefreshRgn(
								RgnHandle	inRegionH);

	virtual void		DontRefresh(
								Boolean		inOKIfHidden = false);

	virtual void		DontRefreshRect(
								const Rect&	inRect,
								Boolean		inOKIfHidden = false);

	virtual void		DontRefreshRgn(
								RgnHandle	inRegionH,
								Boolean		inOKIfHidden = false);

	virtual void		InvalPortRect( const Rect *inRect );
	
	virtual void		InvalPortRgn( RgnHandle inRgnH );
	
	virtual void		ValidPortRect( const Rect *inRect );
	
	virtual void		ValidPortRgn( RgnHandle inRgnH );

	virtual RgnHandle	GetLocalUpdateRgn();

		// ¥ Color State

	virtual void		ApplyForeAndBackColors() const;
	
	virtual void		SetForeAndBackColors(
								const RGBColor *inForeColor,
								const RGBColor *inBackColor);
								
	virtual void		GetForeAndBackColors(
								RGBColor		*outForeColor,
								RGBColor		*outBackColor) const;

	virtual void		GetEmbeddedTextColor(
								SInt16		inDepth,
								bool		inHasColor,
								bool		inIsActive,
								RGBColor&	outColor) const;

		// ¥ Drawing

	virtual void		FlushPortBuffer() const;
	virtual void		UpdatePort();
	virtual Boolean		FocusDraw(
								LPane		*inSubPane = nil);
	virtual Boolean		FocusExposed(
								Boolean		inAlwaysFocus = false);
	virtual void		Draw(	RgnHandle	inSuperDrawRgnH);
	virtual void		DrawSelf();

		// ¥ Printing

	virtual void		CountPanels(
								UInt32		&outHorizPanels,
								UInt32		&outVertPanels);
	virtual Boolean		ScrollToPanel(
								const PanelSpec		&inPanel);
	virtual void		PrintPanel(
								const PanelSpec		&inPanel,
								RgnHandle			inSuperPrintRgnH);
	virtual void		SuperPrintPanel(
								const PanelSpec		&inSuperPanel,
								RgnHandle			inSuperPrintRgnH);
	virtual void		PrintPanelSelf(
								const PanelSpec		&inPanel);

		// ¥ Coordinate Transformations

	virtual void		PortToLocalPoint( Point &ioPoint ) const;
	
	virtual void		LocalToPortPoint( Point &ioPoint ) const;

	virtual void		GlobalToPortPoint( Point &ioPoint ) const;
	
	virtual void		PortToGlobalPoint( Point &ioPoint ) const;

		// ¥ Static Utilities

	static Boolean		IsAreaInQDSpace(
								SInt32		inLeft,
								SInt32		inTop,
								SInt16		inWidth,
								SInt16		inHeight);

	static SInt16		GetClickCount()		{ return sClickCount; }

	static LPane*		GetLastPaneMoused()	{ return sLastPaneMoused; }
	
	static void			SetLastPaneMoused( LPane* inPane )
							{
								sLastPaneMoused = inPane;
							}
							
	static EventModifiers	GetModifiersForLastMouseDown()
								{
									return sModifiersLastMouseDown;
								}

		// ¥ Device Information

						// GetBitDepth() returns current bit depth for drawing.
						// Will return 0 if not called from within DrawSelf
						// Warning: Always returns 0 for buffered ports, which
						//   means drawing into windows on Mac OS X.
						//
						// GetDeviceInfo() returns the bit depth of the Pane's device.
						// Returns the correct values for buffered ports and for
						// classic ports at all times.
						
	static SInt16		GetBitDepth()	{ return sBitDepth; }

	virtual void		GetDeviceInfo(
								SInt16&		outBitDepth,
								bool&		outHasColor) const;

protected:
	void			InitPane( const SPaneInfo &inPaneInfo );
	virtual void	FinishCreateSelf();

	virtual void	ShowSelf();
	virtual void	SuperShow();
	virtual void	HideSelf();
	virtual void	SuperHide();

	virtual void	ActivateSelf();
	virtual void	SuperActivate();
	virtual void	DeactivateSelf();
	virtual void	SuperDeactivate();

	virtual void	EnableSelf();
	virtual void	SuperEnable();
	virtual void	DisableSelf();
	virtual void	SuperDisable();
	
protected:									// Member Variables
	PaneIDT			mPaneID;
	SDimension16	mFrameSize;
	SPoint32		mFrameLocation;			// In Port coords
	SBooleanRect	mFrameBinding;
	SInt32			mUserCon;
	LView			*mSuperView;
	ETriState		mVisible;
	ETriState		mActive;
	ETriState		mEnabled;
	Boolean			mRefreshAllWhenResized;

protected:									// Class Variables
	static LView	*sDefaultView;
	static LPane	*sLastPaneClicked;
	static LPane	*sLastPaneMoused;
	static UInt32	sWhenLastMouseUp;
	
	static UInt32			sWhenLastMouseDown;
	static Point			sWhereLastMouseDown;
	static EventModifiers	sModifiersLastMouseDown;
	
	static SInt16	sClickCount;
	static SInt16	sBitDepth;
	static bool		sDeviceHasColor;

#if PP_Explain_Name_Changes

		// Use AdjustMouse() and AdjustMouseSelf() instead of
		// AdjustCursor() and AdjustCursorSelf(). The "mouse"
		// routines have a RgnHandle parameter that is the
		// mouse region passed to ::WaitNextEvent().
		//
		// The defintions below are designed so that you will get
		// a warning about hidden virtual functions if you have
		// overrides of these functions in your code.

private:
	virtual void	AdjustCursor() { }
								
	virtual void	AdjustCursorSelf() { }
#endif
};


// ---------------------------------------------------------------------------
//	Stack-based class for setting the focus and making an empty clip region
//	if the Pane is invisible

class	StFocusAndClipIfHidden {
public:
					StFocusAndClipIfHidden( LPane* inPane );
					~StFocusAndClipIfHidden();
private:
	RgnHandle		mClipRgn;
};


// ---------------------------------------------------------------------------
//	Stack-based class for temporarily changing a Pane's visibile state

class	StPaneVisibleState {
public:
					StPaneVisibleState( LPane* inPane, ETriState inState );
					~StPaneVisibleState();
private:
	LPane*			mPane;
	ETriState		mSaveState;
};


PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
