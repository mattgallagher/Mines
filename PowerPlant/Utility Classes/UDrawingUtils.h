// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UDrawingUtils.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UDrawingUtils
#define _H_UDrawingUtils
#pragma once

#include <UDrawingState.h>

#include <CarbonEvents.h>
#include <Quickdraw.h>
#include <Script.h>
#include <TextEdit.h>
#include <TextUtils.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

namespace	UDrawingUtils {

	bool		IsColorGrafPort( GrafPtr inMacPort );

	SInt16		GetPortPixelDepth( GrafPtr inMacPort );

	bool		IsActiveScreenDevice( GDHandle inDevice );

	bool		DeviceSupportsColor( GDHandle inDevice );

	void		SetHiliteModeOn();
	
	bool		IsPortBuffered( GrafPtr inMacPort );
		
	void		FlushPortBuffer(
						GrafPtr		inMacPort,
						RgnHandle	inRegion);
						
	void		FlushPortBuffer(
						GrafPtr		inMacPort,
						const Rect&	inRect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

class	StColorDrawLoop {
public:
						StColorDrawLoop( const Rect& inLocalRect );
						
						~StColorDrawLoop();

	bool				NextDepth( SInt16& outDepth );

	GDHandle			GetCurrentDevice()		{ return mCurrentDevice; }
	
protected:
	bool				NextDevice( SInt16& outDepth );

protected:
	StClipRgnState	mClipState;
	Rect			mGlobalRect;
	GDHandle		mCurrentDevice;
};

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

class	StDeviceLoop : public StColorDrawLoop {
public:
						StDeviceLoop( const Rect& inLocalRect );
						
						~StDeviceLoop();
						
	bool				NextDepth( SInt16& outDepth );

protected:
	bool			mHasOldQD;
	bool			mOldQDPending;
};

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

class	UMarchingAnts {
public:
	static Boolean		BeginMarch();
	static void			EndMarch();

	static void			UseAntsPattern();

protected:
	static Pattern	sAntsPattern;
	static UInt32	sNextAntMarchTime;
};

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

class	LMarchingAnts {
public:
						LMarchingAnts( UInt32 inMarchInterval = 3 );

						~LMarchingAnts();

	Boolean				BeginMarch();

	void				EndMarch();

	void				UseAntsPattern();

protected:
	Pattern			mAntsPattern;
	UInt32			mMarchInterval;
	UInt32			mNextAntMarchTime;
};

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

namespace	UTextDrawing {

	void		DrawWithJustification(
						Ptr				inText,
						SInt32			inLength,
						const Rect&		inRect,
						SInt16			inJustification,
						bool			inFirstLeading = true);

	void		DrawTruncatedWithJust(
						ConstStringPtr	inString,
				  		const Rect&		inRect,
				  		SInt16			inJustification,
						bool			inTruncateString = true,
				  		bool	 		inPreferOutline = false,
				  		TruncCode		inTruncWhere = truncEnd);

	inline
	SInt16		GetActualJustification( SInt16 inJustification )
					{
						return (inJustification == teFlushDefault) ?
									::GetSysDirection() :
									inJustification;
					}

	void		DimTextColor( RGBColor& ioColor );
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

namespace	UMouseTracking {

	typedef void (* TrackActionFunc)( void * );

	void	TrackMouseDown(
					GrafPtr					inPort,
					Point&					outPoint,
					MouseTrackingResult&	outResult);

	void	TrackMouseDownWithAction(
					GrafPtr					inPort,
					Point&					outPoint,
					MouseTrackingResult&	outResult,
					TrackActionFunc			inActionFunc,
					UInt32					inTickInterval,
					void*					inUserData);
					
	void	TrackMouseDownInRegion(
					GrafPtr					inPort,
					RgnHandle				inRegion,
					Boolean&				ioWasInRgn,
					MouseTrackingResult&	outResult);
					
	void	TrackMouseDownInRegionWithAction(
					GrafPtr					inPort,
					RgnHandle				inRegion,
					Boolean&				ioWasInRgn,
					MouseTrackingResult&	outResult,
					TrackActionFunc			inActionFunc,
					UInt32					inTickInterval,
					void*					inUserData);
}

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
