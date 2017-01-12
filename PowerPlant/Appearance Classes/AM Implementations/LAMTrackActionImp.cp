// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMTrackActionImp.cp		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Implementation for Appearance Manager controls which support live
//	tracking and 32-bit values

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LAMTrackActionImp.h>
#include <LScrollBar.h>
#include <LStream.h>
#include <UControlMgr.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LAMTrackActionImp					Stream/Default Constructor	  [public]
// ---------------------------------------------------------------------------
//	Do not access inStream. It may be nil. Read Stream data in Init()

LAMTrackActionImp::LAMTrackActionImp(
	LStream*	inStream)

	: LAMControlImp(inStream)
{
	mUsingBigValues = false;
}


// ---------------------------------------------------------------------------
//	¥ Init									Parameterized Initializer [public]
// ---------------------------------------------------------------------------

void
LAMTrackActionImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			inRefCon)
{
	LAMControlImp::Init(inControlPane, inControlKind, inTitle, inTextTraitsID,
						inRefCon);
						
		// We install an ActionProc callback routine which gets while
		// tracking the mouse. Carbon, CFM, and 68K have different
		// ways of setting up a callback function. We use local static
		// variables as storage.
						
#if TARGET_API_MAC_CARBON

	static StControlActionUPP	sTrackActionUPP(LAMTrackActionImp::TrackAction);

#else

	#if TARGET_RT_MAC_CFM		// For CFM, a UPP is a pointer to a
								//   RoutineDescriptor struct.

		static RoutineDescriptor	sTrackActionDesc =
				BUILD_ROUTINE_DESCRIPTOR( uppControlActionProcInfo,
										  LAMTrackActionImp::TrackAction );

		static ControlActionUPP		sTrackActionUPP = &sTrackActionDesc;

	#else						// For non-CFM, a UPP is just a function
								//   pointer
		static ControlActionUPP		sTrackActionUPP = LAMTrackActionImp::TrackAction;
	#endif

#endif

		// Attach ActionProc to the control and store a pointer to this
		// object in the refCon so the ActionProc can retrieve it
		
	::SetControlAction(mMacControlH, sTrackActionUPP);
	::SetControlReference(mMacControlH, (long) this);

									// Scale from 32 to 16 bits if necessary
	mUsingBigValues = ApplySmallSettings( mControlPane->PeekValue(),
										  mControlPane->GetMinValue(),
										  mControlPane->GetMaxValue() );
}


// ---------------------------------------------------------------------------
//	¥ ~LAMTrackActionImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LAMTrackActionImp::~LAMTrackActionImp()
{
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LAMTrackActionImp::SetValue(
	SInt32		inValue)
{
	SInt16	ctlValue = (SInt16) inValue; // Scale to 32-bit value if necessary
	if (mUsingBigValues) {
		ctlValue = CalcSmallValue( inValue,
								   mControlPane->GetMinValue(),
								   mControlPane->GetMaxValue() );
	}

	StFocusAndClipIfHidden	focus(mControlPane);
	::SetControlValue(mMacControlH, ctlValue);
}


// ---------------------------------------------------------------------------
//	¥ SetMinValue													  [public]
// ---------------------------------------------------------------------------

void
LAMTrackActionImp::SetMinValue(
	SInt32		inMinValue)
{
									// Scale to 16-bit values if necessary
	mUsingBigValues =
			ApplySmallSettings( mControlPane->PeekValue(),
							    inMinValue,
							    mControlPane->GetMaxValue() );
}


// ---------------------------------------------------------------------------
//	¥ SetMaxValue													  [public]
// ---------------------------------------------------------------------------

void
LAMTrackActionImp::SetMaxValue(
	SInt32		inMaxValue)
{
									// Scale to 16-bit values if necessary
	mUsingBigValues =
			ApplySmallSettings( mControlPane->PeekValue(),
							    mControlPane->GetMinValue(),
							    inMaxValue );
}


// ---------------------------------------------------------------------------
//	¥ TrackHotSpot													  [public]
// ---------------------------------------------------------------------------
//	Track mouse while it is being pressed

Boolean
LAMTrackActionImp::TrackHotSpot(
	SInt16		inHotSpot,
	Point		inPoint,
	SInt16		inModifiers)
{
		// Set proper callback function. For non-thumb parts, we pass
		// (-1), which means to use the function set via SetControlAction().
		//
		// For thumb parts, the Control Manager does some bizarre stuff.
		// For non-live thumb parts, we don't want any callback (and the
		// API for such callbacks is different anyway). But for live
		// thumb parts, we can use the same callback as for other parts.

	ControlActionUPP	actionProc = (ControlActionUPP) (-1);
	if (inHotSpot >= kControlIndicatorPart) {
		UInt32	features;
		::GetControlFeatures(mMacControlH, &features);
		if (!(features & kControlSupportsLiveFeedback)) {
			actionProc = nil;
		}
	}

										// Save current value so we can
										//   see if it changed while tracking
	SInt16	origValue = ::GetControlValue(mMacControlH);

		// HandleControlClick handles tracking and returns kControlNoPart
		// if the mouse is released outside the HotSpot
	Boolean	releasedInHotSpot =
		::HandleControlClick( mMacControlH, inPoint,
								(EventModifiers) inModifiers,
								actionProc )
									!= kControlNoPart;

		// Control Manager can change the value while tracking.
		// If it did, we need to call SetValue() to update the
		// class's copy of the value.

	SInt32	currValue = ::GetControlValue(mMacControlH);
	if (currValue != origValue) {
		SInt32	newValue = currValue;
		if (mUsingBigValues) {
			newValue = CalcBigValue((SInt16) currValue);
		}
		mControlPane->SetValue(newValue);
	}

	return releasedInHotSpot;
}


// ---------------------------------------------------------------------------
//	¥ DoTrackAction													  [public]
// ---------------------------------------------------------------------------
//	Member function called by ActionProc

void
LAMTrackActionImp::DoTrackAction(
	SInt16		inHotSpot)
{
									// Scale to 32-bit value if necessary
	SInt32	value = ::GetControlValue(mMacControlH);
	if (mUsingBigValues) {
		value = CalcBigValue((SInt16) value);
	}
									// ControlPane handles tracking action
	mControlPane->DoTrackAction(inHotSpot, value);

	mControlPane->FocusDraw();		// Action could have changed focus
}


// ---------------------------------------------------------------------------
//	¥ TrackAction										  [static] [protected]
// ---------------------------------------------------------------------------
//	ControlManager ActionProc callback function

pascal void
LAMTrackActionImp::TrackAction(
	ControlHandle	inMacControl,
	SInt16			inPart)
{
	try {
		LAMTrackActionImp	*theImp = (LAMTrackActionImp*)
										::GetControlReference(inMacControl);
		theImp->DoTrackAction(inPart);
	}

	catch (...) { }			// Don't propagate exception into Toolbox!
}


PP_End_Namespace_PowerPlant
