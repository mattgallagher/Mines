// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMLTEPane.h					PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================
//
//	Text view based on Textension, aka Multilingual Text Edit (MLTE)

#ifndef _H_LMLTEPane
#define _H_LMLTEPane

#include <LCommander.h>
#include <LPane.h>
#include <LPeriodical.h>
#include <UKeyFilters.h>

#include <MacTextEditor.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

		// On OS X and CarbonLib 1.3 or later, MLTE automatically flashes
		// the insertion cursor via a timer task. On Classic or earlier
		// CarbonLib versions, we need to use a Periodical to flash the
		// cursor.
		
#define	PP_MLTE_Manual_Idle	(PP_Target_Classic || (PP_Minimum_Carbon_Version < 0x0130))


// ---------------------------------------------------------------------------

#if PP_MLTE_Manual_Idle

class LMLTEPane : public LPane,
				  public LCommander,
				  public LPeriodical {
				  
#else

class LMLTEPane : public LPane,
				  public LCommander {

#endif

public:
	enum { class_ID = FOUR_CHAR_CODE('mlte') };
	
	
	// -----------------------------------------------------------------------
	
	static void			Initialize(
								ResIDT				inFontMenuID = 0,
								ResIDT				inStartHierMenuID = 175,
								TXNMacOSPreferredFontDescription
													inFonts[] = nil,
								ItemCount			inFontCount = 0,
								TXNInitOptions		inOptions = 0);
								
	static void			Terminate();
								
	// -----------------------------------------------------------------------
	
						LMLTEPane();
						
						LMLTEPane( LStream* inStream );
						
						LMLTEPane(
								const SPaneInfo&	inPaneInfo,
								LCommander*			inSuperCommander,
								const FSSpec*		inFileSpec,
								TXNFrameOptions		inFrameOptions,
								TXNFrameType		inFrameType,
								TXNFileType			inFileType,
								TXNPermanentTextEncodingType	inEncodingType);
						
	virtual				~LMLTEPane();
	
	TXNObject			GetTextObject();
	
	virtual StringPtr	GetDescriptor( Str255 outDescriptor ) const;

	virtual void		SetDescriptor( ConstStringPtr inDescriptor );
	
	virtual void		SetTextPtr(
								const void*			inTextP,
								ByteCount			inTextLength);
	
	#if PP_Uses_CFDescriptor
	
	virtual CFStringRef	CopyCFDescriptor() const;
	
	virtual void		SetCFDescriptor( CFStringRef inStringRef );
	
	#endif

	virtual Boolean		ObeyCommand(
								CommandT			inCommand,
								void*				ioParam);
								
	virtual void		FindCommandStatus(
								CommandT			inCommand,
								Boolean&			outEnabled,
								Boolean&			outUsesMark,
								UInt16&				outMark,
								Str255				outName);
								
	virtual Boolean		HandleKeyPress( const EventRecord& inKeyEvent );

#if PP_MLTE_Manual_Idle
	virtual	void		SpendTime( const EventRecord& inMacEvent );
#endif
								
	virtual void		AdjustMouseSelf(
								Point				inPortPt,
								const EventRecord&	inMacEvent,
								RgnHandle			outMouseRgn);

	virtual void		DrawSelf();
	
	virtual void		ClickSelf( const SMouseDownEvent& inMouseDown );

	virtual void		ResizeFrameBy(
								SInt16				inWidthDelta,
								SInt16				inHeightDelta,
								Boolean				inRefresh);

	virtual void		MoveBy(	SInt32				inHorizDelta,
								SInt32				inVertDelta,
								Boolean				inRefresh);
	
	virtual Boolean		FocusDraw( LPane* inSubPane = nil );

	virtual void		PortToLocalPoint( Point &ioPoint ) const;
	
	virtual void		LocalToPortPoint( Point &ioPoint) const;
	
	virtual void		SetOneControlTag(
								TXNControlTag		inTag,
								TXNControlData		inData);
								
	void				SetCGContext( CGContextRef inCGContext );
	
	virtual void		SetOneTypeAttribute(
								TXNTypeAttributes*	inAttribute,
								TXNOffset			inStartOffset,
								TXNOffset			inEndOffset);
								
	virtual void		SetOneTypeAttributeForSelection( TXNTypeAttributes* inAttribute );
								
	void				SetFontName( ConstStringPtr inName );
	
	void				SetFontSize( Fixed inSize );
	
	void				SetFontStyle( Style inStyle );
	
	void				SetFontColor( const RGBColor& inColor );

protected:
	void				InitMLTEPane(
								const FSSpec*		inFileSpec,
								TXNFrameOptions		inFrameOptions,
								TXNFrameType		inFrameType,
								TXNFileType			inFileType,
								TXNPermanentTextEncodingType	inEncodingType);

	virtual void		BeTarget();
	
	virtual void		DontBeTarget();
	
protected:
	TXNObject		mTXNObject;
	TXNFrameID		mTXNFrameID;
	
	static TXNFontMenuObject	sTXNFontMenuObject;
	static ResIDT				sFontMenuID;
	static ResIDT				sStartHierMenuID;
	static ResIDT				sEndHierMenuID;
	static bool					sMLTEIsInitialized;
};

// ---------------------------------------------------------------------------
//	Inline Functions

inline
TXNObject
LMLTEPane::GetTextObject()
{
	return mTXNObject;
}


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
