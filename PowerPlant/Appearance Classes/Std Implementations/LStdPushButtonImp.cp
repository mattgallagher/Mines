// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LStdPushButtonImp.cp		PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LStdPushButtonImp.h>
#include <UDrawingState.h>
#include <UEnvironment.h>
#include <UTBAccessors.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LStdPushButtonImp					Stream/Default Constructor	  [public]
// ---------------------------------------------------------------------------
//	Do not access inStream. It may be nil. Read Stream data in Init()

LStdPushButtonImp::LStdPushButtonImp(
	LStream*	inStream)

	: LStdControlImp(inStream)
{
	mIsDefault = false;
}


// ---------------------------------------------------------------------------
//	¥ ~LStdPushButtonImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LStdPushButtonImp::~LStdPushButtonImp()
{
}


// ---------------------------------------------------------------------------
//	¥ SetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LStdPushButtonImp::SetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
	if (inTag == kControlPushButtonDefaultTag) {
	
		Boolean	becomeDefault = *(Boolean *) inDataPtr;
	
		if (mIsDefault != becomeDefault) {
		
			mIsDefault = becomeDefault;
			
			if (UEnvironment::IsAppearanceRunning()) {
											// Appearance Manager will draw
											//   the button properly
				::SetControlData(mMacControlH, inPartCode, inTag,
										inDataSize, inDataPtr);
			}
		
			if (becomeDefault) {			// Immediately draw default ring
			
				mControlPane->ImpStructureChanged();
				mControlPane->Draw(nil);
			
			} else {						// Ring will be erased when area
											//   underneath it redraws
				mControlPane->Refresh();
				mControlPane->ImpStructureChanged();
			
			}
		}
		
	} else if (inTag == kControlPushButtonCancelTag) {
	
		// Do nothing. In Standard UI, cancel button does not have
		// a special appearance.
	
	} else {
		LStdControlImp::SetDataTag(inPartCode, inTag, inDataSize, inDataPtr);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LStdPushButtonImp::GetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size*			outDataSize) const
{
	if (inTag == kControlPushButtonDefaultTag) {
	
		if (inBufferSize >= sizeof(Boolean)) {
			*(Boolean *) inBuffer = mIsDefault;
		}
		
		if (outDataSize != nil) {
			*outDataSize = sizeof(Boolean);
		}
	
	} else {
		LStdControlImp::GetDataTag(inPartCode, inTag, inBufferSize,
										inBuffer, outDataSize);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetStructureOffsets											  [public]
// ---------------------------------------------------------------------------

OSStatus
LStdPushButtonImp::GetStructureOffsets(
	Rect&	outOffsets) const
{
	if (mIsDefault) {				// Default ring is 4 pixels outset from
		outOffsets.left   = 4;		//   the button
		outOffsets.right  = 4;
		outOffsets.top    = 4;
		outOffsets.bottom = 4;
		
	} else {
		outOffsets = Rect_0000;
	}
	
	return noErr;
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LStdPushButtonImp::DrawSelf()
{
	LStdControlImp::DrawSelf();
	
	if (mIsDefault and not UEnvironment::IsAppearanceRunning()) {
	
			// When the Appearance Manager isn't running, the default
			// ring is a 3-pixel thick rounded rectangle
	
		Rect	frame;
		CalcLocalFrameRect(frame);
		::MacInsetRect(&frame, -4, -4);
		
		SInt16	roundCorner = (SInt16) ((frame.bottom - frame.top - 8) / 2);
		if (roundCorner < 16) {
			roundCorner = 16;
		}
		
		ApplyForeAndBackColors();
		::PenNormal();
		::PenSize(3, 3);
		
		if (::GetControlHilite(mMacControlH) != 0) {
										// Inactive button. Draw gray ring.
			Pattern	grayPat;
			UQDGlobals::GetGrayPat(&grayPat);
			::PenPat(&grayPat);
		}
		
		::FrameRoundRect(&frame, roundCorner, roundCorner);
	}
}


PP_End_Namespace_PowerPlant
