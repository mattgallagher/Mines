// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMEditTextImp.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Implementation for Appearance Manager EditText item.
//
//	The class does not create a ControlHandle and it inherits from
//	LControlImp rather than LAMControlImp.
//
//	The LEditText ControlPane handles everything related to the text.
//	The only thing this class does is draw the frame and focus ring
//	around the text area.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LAMEditTextImp.h>
#include <LEditText.h>
#include <UDrawingState.h>
#include <UEnvironment.h>

#include <Appearance.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LAMEditTextImp						Constructor				  [public]
// ---------------------------------------------------------------------------

LAMEditTextImp::LAMEditTextImp(
	LControlPane*	inControlPane)

	: LControlImp(inControlPane)
{
}


// ---------------------------------------------------------------------------
//	¥ LAMEditTextImp						Stream Constructor		  [public]
// ---------------------------------------------------------------------------
//	Do not access inStream. It may be nil. Read Stream data in Init()

LAMEditTextImp::LAMEditTextImp(
	LStream*	inStream)

	: LControlImp(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LAMEditTextImp						Destructor				  [public]
// ---------------------------------------------------------------------------

LAMEditTextImp::~LAMEditTextImp()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LAMEditTextImp::DrawSelf()
{
	Rect	frame;
	CalcLocalFrameRect(frame);
	
		// On Mac OS X, DrawThemeEditTextFrame() draws in an
		// additive mode, where repeated calls to draw the same
		// frame create successively darker borders.
		//
		// To have the border to draw correctly, we erase before
		// drawing it. Note that we erase before applying the fore
		// and back colors. LEditText::FocusDraw() will have set
		// the correct background for drawing the interior of the
		// edit text pane.

	if (UEnvironment::HasFeature(env_HasAquaTheme)) {
		Rect		border = frame;
		StRegion	borderRgn = border;
		
		::MacInsetRect(&border, 5, 5);
		borderRgn -= border;
		
		::EraseRgn(borderRgn);
	}
		
	
	ThemeDrawState	state = kThemeStateInactive;
	if (IsActive() && IsEnabled()) {
		state = kThemeStateActive;
	}

	mControlPane->ApplyForeAndBackColors();
	::PenNormal();

		// The Toolbox routines for drawing the 3D border and focus
		// ring take a Rect that should be the same as the inner
		// border around the edit field. This is 3 pixels inset
		// from our Frame, which encloses the focus ring surrounding
		// the inner border.

	::MacInsetRect(&frame, 3, 3);

		// The focus ring draws over the outer edge of the 3D
		// border. Thus, we must draw the ring after the border
		// if the ring is visible. But, when the ring is not
		// visible, we must draw it first so that it erases the
		// area that would have been covered by a ring.
				
	if (((LEditText*) mControlPane)->HasFocusRing()) {
		::DrawThemeEditTextFrame(&frame, state);
		DrawFocusBorder(frame, true);

	} else {
		DrawFocusBorder(frame, false);
		::DrawThemeEditTextFrame(&frame, state);
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawFocusBorder											   [protected]
// ---------------------------------------------------------------------------

void
LAMEditTextImp::DrawFocusBorder(
	const Rect&	inFrame,
	bool		inHasFocus)
{
		
	// [Workaround] Mac OS 9.1 bug?
	//
	// DrawThemeFocusRect() does not draw properly during an
	// update event if the update region intersects both the
	// top and bottom edges, but not the left or right side.
	//
	//			-----  <-- Update region
	//			|	|
	//		----|	|----
	//		|	|	|	|  <-- Focus Rect
	//		----|	|----
	//			|	|
	//			-----
	//
	// The culprit seems to be the "ATI Graphics Accelerator"
	// extension, version 2.5.4.
	//
	// As a workaround, we call DrawThemeFocusRegion() instead
	// of DrawThemeFocusRect(). However, the former call requires
	// AM 1.0.1 or later.
		
	if (UEnvironment::HasFeature(env_HasAppearance101)) {
	
		::DrawThemeFocusRegion(StRegion(inFrame), inHasFocus);
		
	} else {
		::DrawThemeFocusRect(&inFrame, inHasFocus);
	}
}


PP_End_Namespace_PowerPlant
