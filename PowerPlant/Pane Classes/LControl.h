// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LControl.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Base class for visual elements that have a numerical value, track mouse
//	clicks, and broadcast a message when the value changes

#ifndef _H_LControl
#define _H_LControl
#pragma once

#include <LPane.h>
#include <LBroadcaster.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LCommander;

#pragma options align=mac68k

typedef struct	SControlInfo {
	MessageT	valueMessage;
	SInt32		value;
	SInt32		minValue;
	SInt32		maxValue;
} SControlInfo;

#pragma options align=reset

// ---------------------------------------------------------------------------

class	LControl : public LPane,
				   public LBroadcaster {
public:
	enum { class_ID = FOUR_CHAR_CODE('cntl') };

						LControl();
						
						LControl( const LControl& inOriginal );
						
						LControl(
								const SPaneInfo&	inPaneInfo,
								MessageT			inValueMessage = 0,
								SInt32				inValue = 0,
								SInt32				inMinValue = 0,
								SInt32				inMaxValue = 0);
								
						LControl( LStream* inStream );
						
	virtual				~LControl();

	MessageT			GetValueMessage() const	{ return mValueMessage; }

	void				SetValueMessage( MessageT inValueMessage )
							{
								mValueMessage = inValueMessage;
							}

	virtual SInt32		GetValue() const;
	
	virtual void		SetValue( SInt32 inValue );
	
	virtual void		IncrementValue( SInt32 inIncrement );

	SInt32				GetMinValue() const		{ return mMinValue; }

	virtual void		SetMinValue( SInt32 inMinValue );

	SInt32				GetMaxValue() const		{ return mMaxValue; }

	virtual void		SetMaxValue( SInt32 inMaxValue );

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

	virtual void		SimulateHotSpotClick( SInt16 inHotSpot );
	
	virtual void		HotSpotResult( SInt16 inHotSpot );

protected:
	MessageT	mValueMessage;
	SInt32		mValue;
	SInt32		mMinValue;
	SInt32		mMaxValue;

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
								
	virtual void		DoneTracking(
								SInt16			inHotSpot,
								Boolean			inGoodTrack);

	virtual void		BroadcastValueMessage();

	virtual void		ClickSelf( const SMouseDownEvent& inMouseDown );
	
	// -----------------------------------------------------------------------
	//	Callback function used during mouse down tracking

	static void			TrackingCallback( void* inUserData );

	struct STrackingState {			// Pointer to this struct is passed
		LControl*	control;		//   to TrackingCallback()
		SInt16		hotSpot;
		Boolean		isInside;
	};
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
