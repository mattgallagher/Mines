// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMTextGroupBoxImp.cp			PowerPlant 2.2.2©2000-2005 Metrowerks Inc.
// ===========================================================================
//	Implementation for an Appearance Manager TextGroupBox control
//
//	[Workaround] Appearance Manager Bug
//		When changing the title of a text group box to an empty string,
//		the control manager does not properly readjust the border. It
//		leaves an empty space the size of the former title and does not
//		move the top of the border up to the top of the bounds. The is
//		for AM 1.1.1 and earlier.
//
//	[Workaround] Mac OS X Bug
//		On Mac OS X Public Beta, a text group box with no title still moves
//		the top edge of the border down to the vertical position when there
//		is a title. On earlier AM versions, the top edge of the border is
//		at the top of the bounds when there is no title.
//
//	To workaround both these bugs, this implementation checks for an
//	empty title. When the title is empty, it draws the group box border
//	using Theme drawing primitives rather than drawing the control.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LAMTextGroupBoxImp.h>
#include <LStream.h>
#include <UAppearance.h>
#include <UTBAccessors.h>

#include <Appearance.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LAMTextGroupBoxImp				Stream/Default Constructor	  [public]
// ---------------------------------------------------------------------------
//	Do not access inStream. It may be nil. Read Stream data in Init()

LAMTextGroupBoxImp::LAMTextGroupBoxImp(
	LStream*	inStream)

	: LAMControlImp(inStream)
{
	mIsPrimary	  = true;
	mTitleIsEmpty = false;
}


// ---------------------------------------------------------------------------
//	¥ ~LAMTextGroupBoxImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LAMTextGroupBoxImp::~LAMTextGroupBoxImp()
{
}


// ---------------------------------------------------------------------------
//	¥ Init									Parameterized Initializer [public]
// ---------------------------------------------------------------------------

void
LAMTextGroupBoxImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			inRefCon)
{
		// We need to store whether this is a primary (or secondary)
		// group box and if the title is an empty string

	mIsPrimary	  = (inControlKind == kControlGroupBoxTextTitleProc);
	mTitleIsEmpty = (inTitle[0] == 0);
	
	LAMControlImp::Init(inControlPane, inControlKind, inTitle,
							inTextTraitsID, inRefCon);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LAMTextGroupBoxImp::DrawSelf()
{
	if (mTitleIsEmpty) {
	
			// [Workaround] Appearance Manager bug, see comments at top
			//
			// When there is no title, the group box border occupies
			// the entire frame. We draw the proper kind of border using
			// Theme drawing primitives.
	
		Rect	frame;
		CalcLocalFrameRect(frame);
	
		ThemeDrawState	themeState = kThemeStateInactive;
		if (IsActive() and IsEnabled()) {
			themeState = kThemeStateActive;
		}
		
		UAppearance::DrawThemeGroupBorder(&frame, themeState, mIsPrimary);
	
	} else {							// Group box has a title
		LAMControlImp::DrawSelf();		// Let inherited function draw it
	}
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------
//	Set title of the TextGroupBox

void
LAMTextGroupBoxImp::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
	mTitleIsEmpty = (inDescriptor[0] == 0);	// Set flag for empty title
	
		// SetControlTitle() automatically redraws the control. For
		// TextGroupBoxes, this erases its contents, obscuring its
		// subpanes. To workaround this, we hide the control without
		// redrawing, set the title, then restore the control's
		// visible state. Afterwards, we refresh.

	mControlPane->FocusDraw();
	
	OSErr	err;
	SInt16	beforeBottom = 0; 
	Rect	titleRect;
	
		// Get title area before we change the title. We only care about
		// the bottom edge.
	
	err = ::GetControlData( mMacControlH,
							kControlEntireControl,
							kControlGroupBoxTitleRectTag,
							sizeof(Rect), &titleRect, nil);
							
	if (err == noErr) {
		beforeBottom = titleRect.bottom;
	}
	
		// Toggle visibilty around setting the title so the Control Mgr
		// doesn't draw
	
	Boolean	saveVis = ::IsControlVisible(mMacControlH);
	::SetControlVisibility(mMacControlH, false, Refresh_No);
	::SetControlTitle(mMacControlH, inDescriptor);
	::SetControlVisibility(mMacControlH, saveVis, Refresh_No);
	
		// Now get the title area for the changed title

	err = ::GetControlData( mMacControlH,
							kControlEntireControl,
							kControlGroupBoxTitleRectTag,
							sizeof(Rect), &titleRect, nil);
							
	if (err == noErr) {
	
			// Refresh the top of the group box, extending down to
			// the bottom of the title area (using the larger of the
			// before and after bottom edges) and including the entire
			// width. The handles all situations of the new/old title
			// being longer, shorter, or empty.
		
		Rect	bounds;
		::GetControlBounds(mMacControlH, &bounds);
		
		bounds.bottom = beforeBottom;
		
		if (titleRect.bottom > beforeBottom) {
			bounds.bottom = titleRect.bottom;
		}
		
		mControlPane->RefreshRect(bounds);
		
	} else {							// Can't get the title area
		mControlPane->Refresh();		// Refresh the entire group box
	}
}	


PP_End_Namespace_PowerPlant
