// Copyright й2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LWindow.h					PowerPlant 2.2.2	й1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LWindow
#define _H_LWindow
#pragma once

#include <LView.h>
#include <LCommander.h>
#include <LModelObject.h>
#include <LAttachment.h>

#include <Appearance.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

#pragma options align=mac68k

typedef struct	SWINDResource {		// Format of a 'WIND' Resource
	Rect	bounds;
	SInt16	procID;
	SInt16	visible;
	SInt16	hasGoAway;
	SInt32	refCon;
	Str255	title;					// Not really, but OK as long as we
									//   only use this for typecasting
									//   WIND resource handles
} SWINDResource, *SWINDResourceP, **SWINDResourceH;

struct SWindowInfo {
	ResIDT			WINDid;
	SInt16			layer;
	UInt32			attributes;
	SInt16			minimumWidth;
	SInt16			minimumHeight;
	SInt16			maximumWidth;
	SInt16			maximumHeight;
	SDimension16	standardSize;
	SInt32			userCon;
};

#pragma options align=reset

typedef	UInt32	EWindAttr;

const EWindAttr	windAttr_Modal			= 0x80000000;
const EWindAttr	windAttr_Floating		= 0x40000000;
const EWindAttr	windAttr_Regular		= 0x20000000;
const EWindAttr	windAttr_CloseBox		= 0x10000000;
const EWindAttr	windAttr_TitleBar		= 0x08000000;
const EWindAttr	windAttr_Resizable		= 0x04000000;
const EWindAttr	windAttr_SizeBox		= 0x02000000;
const EWindAttr	windAttr_Zoomable		= 0x01000000;
const EWindAttr	windAttr_ShowNew		= 0x00800000;
const EWindAttr	windAttr_Enabled		= 0x00400000;
const EWindAttr	windAttr_Targetable		= 0x00200000;
const EWindAttr	windAttr_GetSelectClick	= 0x00100000;
const EWindAttr	windAttr_HideOnSuspend	= 0x00080000;
const EWindAttr	windAttr_DelaySelect	= 0x00040000;
const EWindAttr	windAttr_EraseOnUpdate	= 0x00020000;


const SInt16	PP_Window_Kind = 20000;
const SInt16	click_OutsideModal = 42;

const WindowPtr	window_InFront	= (WindowPtr) -1;
const WindowPtr	window_InBack	= (WindowPtr) 0;

enum {
	windLayer_Modal,
	windLayer_Floating,
	windLayer_Regular
};

class	LWindowEventHandlers;

// ---------------------------------------------------------------------------
//	Option for using Window Manager 2.0 Routines
//
//		We use the preprocessor to exclude all Window Mgr 2.0 for
//		68K machines since the routines don't exist. For PPC, we
//		can weak link and perform a runtime check.

#if (TARGET_OS_MAC && TARGET_CPU_68K)
	#define PP_Uses_WindowMgr20_Routines	0
#else
	#define PP_Uses_WindowMgr20_Routines	1
#endif


#pragma mark LWindow
// ===========================================================================
// е LWindow
// ===========================================================================

class	LWindow : public LView,
				  public LCommander,
				  public LModelObject {
public:
	enum { class_ID = FOUR_CHAR_CODE('wind') };

						LWindow();

						LWindow( const SWindowInfo& inWindowInfo );

						LWindow(
								ResIDT				inWINDid,
								UInt32				inAttributes,
								LCommander*			inSuperCommander);

						LWindow(
								LCommander*			inSuperCommander,
								const Rect&			inGlobalBounds,
								ConstStringPtr		inTitle,
								SInt16				inProcID,
								UInt32				inAttributes,
								WindowPtr			inBehind);

	#if PP_Uses_WindowMgr20_Routines
								
	static LWindow*		CreateWindow(
								WindowPtr			inMacWindow,
								ResIDT				inPPobID,
								LCommander*			inSuperCommander);
	
						LWindow(
								WindowClass			inWindowClass,
								WindowAttributes	inAttributes,
								const Rect&			inBounds,
								LCommander*			inSuperCommander);
								
						LWindow(
								WindowPtr			inMacWindow,
								LCommander*			inSuperCommander);
								
	#endif // PP_Uses_WindowMgr20_Routines

						LWindow( LStream* inStream );

	virtual				~LWindow();

	static LWindow*		CreateWindow(
								ResIDT				inWindowID,
								LCommander*			inSuperCommander);

	static LWindow*		FetchWindowObject(
								WindowPtr			inWindowP);

	virtual GrafPtr		GetMacPort() const;

	virtual WindowPtr	GetMacWindow() const;
	
	virtual CGContextRef	GetCGContext() const;
	
	CGContextRef		GetSyncedCGContext() const;

	Boolean				HasAttribute( EWindAttr inAttribute ) const
							{ return ((mAttributes & inAttribute) != 0); }

	void				SetAttribute( EWindAttr inAttribute )
									{ mAttributes |= inAttribute; }

	void				ClearAttribute( EWindAttr inAttribute )
									{ mAttributes &= ~inAttribute; }

	void				GetMinMaxSize( Rect& outRect ) const
									{ outRect = mMinMaxSize; }

	void				SetMinMaxSize( const Rect& inRect )
									{ mMinMaxSize = inRect; }

	void				GetStandardSize( SDimension16& outStdSize ) const
									{ outStdSize = mStandardSize; }

	void				SetStandardSize( SDimension16 inStdSize )
									{ mStandardSize = inStdSize; }

	void				GetGlobalBounds( Rect& outBounds ) const;

	virtual StringPtr	GetDescriptor( Str255 outDescriptor ) const;
								
	virtual void		SetDescriptor( ConstStringPtr inDescriptor );
	
	#if PP_Uses_CFDescriptor
	
	virtual CFStringRef		CopyCFDescriptor() const;
	
	virtual void			SetCFDescriptor( CFStringRef inStringRef );
	
	#endif

	virtual void		UpdatePort();
	virtual Boolean		EstablishPort();

	virtual void		ApplyForeAndBackColors() const;

	virtual void		SetForeAndBackColors(
								const RGBColor		*inForeColor,
								const RGBColor		*inBackColor);

	virtual void		GetForeAndBackColors(
								RGBColor			*outForeColor,
								RGBColor			*outBackColor) const;

	virtual void		SetThemeData(
								ThemeBrush			inActiveBackBrush,
								ThemeBrush			inInactiveBackBrush,
								ThemeTextColor		inActiveTextColor,
								ThemeTextColor		inInactiveTextColor);

	virtual void		GetEmbeddedTextColor(
								SInt16				inDepth,
								bool				inHasColor,
								bool				inIsActive,
								RGBColor&			outColor) const;

	virtual void		InvalPortRect( const Rect* inRect );
								
	virtual void		InvalPortRgn( RgnHandle inRgnH );
								
	virtual void		ValidPortRect( const Rect* inRect );
								
	virtual void		ValidPortRgn( RgnHandle inRgnH );
								
	virtual void		ResizeFrameBy(
								SInt16				inWidthDelta,
								SInt16				inHeightDelta,
								Boolean				inRefresh);

	virtual void		Select();
	
	virtual void		AdjustContentMouse(
								Point				inPortPt,
								const EventRecord&	inMacEvent,
								RgnHandle			outMouseRgn);
	
	virtual void		AdjustStructureMouse(
								WindowPartCode		inPart,
								const EventRecord&	inMacEvent,
								RgnHandle			outMouseRgn);
	
	virtual void		Show();
	virtual void		Activate();
	virtual void		Deactivate();
	virtual void		Enable();

	virtual void		Suspend();
	virtual void		Resume();

	virtual void		GlobalToPortPoint( Point& ioPoint ) const;
	
	virtual void		PortToGlobalPoint( Point& ioPoint ) const;

	virtual void		ExpandSubPane(
								LPane*				inSub,
								Boolean				inExpandHoriz,
								Boolean				inExpandVert);

	virtual Boolean		ObeyCommand(
								CommandT			inCommand,
								void*				ioParam);
								
	virtual void		FindCommandStatus(
								CommandT			inCommand,
								Boolean&			outEnabled,
								Boolean&			outUsesMark,
								UInt16&				outMark,
								Str255				outName);

		// ее Performing Actions
		
			// е Click Handling
								
	virtual void		ClickInContent( const EventRecord& inMacEvent );

	virtual void		HandleClick(
								const EventRecord&	inMacEvent,
								SInt16				inPart);

	virtual void		ClickInTitleBar(
								const EventRecord&	inMacEvent,
								SInt16				inPart);

	virtual void		ClickInDrag( const EventRecord& inMacEvent );
			
	virtual void		ClickInGrow( const EventRecord& inMacEvent );
			
	virtual void		ClickInZoom(
								const EventRecord&	inMacEvent,
								SInt16				inZoomDirection);

	virtual void		ClickInGoAway( const EventRecord& inMacEvent );

			// е Changing Position

	virtual void		SendAESetPosition(
								Point				inPosition,
								Boolean				inExecuteAE);

	virtual void		DoSetPosition( Point inPosition );

	void				MoveWindowTo(
								SInt16				inHoriz,
								SInt16				inVert);

	void				MoveWindowBy(
								SInt16				inHorizDelta,
								SInt16				inVertDelta);

			// е Changing Size and/or Position
	
	virtual void		SendAESetBounds(
								Rect*				inBounds,
								Boolean				inExecuteAE);
								
	virtual void		DoSetBounds( const Rect& inBounds );
	
	void				AdaptToBoundsChange( UInt32 inWhatChanged );
								
	void				ResizeWindowTo(
								SInt16				inWidth,
								SInt16				inHeight);
								
	void				ResizeWindowBy(
								SInt16				inWidthDelta,
								SInt16				inHeightDelta);

			// е Zooming
								
	virtual Boolean		CalcStandardBounds( Rect& outStdBounds ) const;
								
	virtual void		CalcStandardBoundsForScreen(
								const Rect&			inScreenBounds,
								Rect&				outStdBounds) const;
								
	virtual void		SendAESetZoom();
	
	virtual void		DoSetZoom( Boolean inZoomToStdState );

			// е Closing

	virtual void		AttemptClose();
	virtual void		DoClose();


		// ее AppleEvent Object Model Support ее

	virtual void		GetAEProperty(
								DescType			inProperty,
								const AEDesc&		inRequestedType,
								AEDesc&				outPropertyDesc) const;

	virtual void		SetAEProperty(
								DescType			inProperty,
								const AEDesc&		inValue,
								AEDesc&				outAEReply);

	virtual bool		AEPropertyExists( DescType inProperty ) const;

	virtual StringPtr	GetModelName( Str255 outModelName ) const;

	void				GetAEWindowAttribute(
								EWindAttr			inAttribute,
								AEDesc&				outPropertyDesc) const;

	virtual void		HandleAppleEvent(
								const AppleEvent&	inAppleEvent,
								AppleEvent&			outAEReply,
								AEDesc&				outResult,
								SInt32				inAENumber);
		// ее Utilities ее

	static LWindow*		FindWindowByID( PaneIDT inWindowID );

protected:
	WindowPtr		mMacWindowP;
	Rect			mMinMaxSize;
	SDimension16	mStandardSize;
	Rect			mUserBounds;
	UInt32			mAttributes;
	RGBColor		mForeColor;
	RGBColor		mBackColor;
	ThemeBrush		mActiveBackgroundBrush;
	ThemeBrush		mInactiveBackgroundBrush;
	ThemeTextColor	mActiveTextColor;
	ThemeTextColor	mInactiveTextColor;
	mutable CGContextRef	mCGContext;
	bool			mMoveOnlyUserZoom;
	
	#if PP_Uses_Carbon_Events
		LWindowEventHandlers*	mEventHandlers;
	#endif
	
	virtual void		FinishCreateSelf();

	virtual void		DrawSelf();
	virtual void		DrawSizeBox();

	virtual void		ShowSelf();
	virtual void		HideSelf();

	virtual void		ActivateSelf();
	virtual void		DeactivateSelf();
	
	virtual void		AdjustUserBounds();

	virtual void		MakeSelfSpecifier(
								AEDesc&			inSuperSpecifier,
								AEDesc&			outSelfSpecifier) const;


	static TArray<WindowPtr>	sWindowList;

private:
	void				InitWindow();
	void				InitWindow( const SWindowInfo &inWindowInfo );
	void				MakeMacWindow( short inWINDid );
	
	#if PP_Uses_WindowMgr20_Routines
	
	void				SetAttrsFromMacWindow();
	
	#endif // PP_Uses_WindowMgr20_Routines
};


#pragma mark LWindowThemeAttachment
// ===========================================================================
// е LWindowThemeAttachment
// ===========================================================================

class	LWindowThemeAttachment : public LAttachment {
public:
	enum { class_ID = FOUR_CHAR_CODE('wtha') };

					LWindowThemeAttachment( LStream* inStream );

	virtual Boolean	Execute(MessageT inMessage, void* ioParam);

protected:
	ThemeBrush		mActiveBackgroundBrush;
	ThemeBrush		mInactiveBackgroundBrush;
	ThemeTextColor	mActiveTextColor;
	ThemeTextColor	mInactiveTextColor;
};


PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
