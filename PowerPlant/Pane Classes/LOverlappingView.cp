// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOverlappingView.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	A View which draws properly when sibling views (views within the same
//	superview) overlap its Frame.
//
//	Subclass LOverlappingView to create a View class that supports
//	overlapping Views of other subclasses of LOverlapping. That is,
//	all Views that can overlap must be descendents of LOverlappingView.
//
//	Or, if your View class needs to inherit from some other view class,
//	override FocusDraw() in your class, copying the code from
//	LOverlappingView::FocusDraw().

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LOverlappingView.h>
#include <TArrayIterator.h>
#include <URegions.h>

PP_Begin_Namespace_PowerPlant


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LOverlappingView						Default Constructor		  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LOverlappingView::LOverlappingView()
{
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LOverlappingView						Copy Constructor		  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LOverlappingView::LOverlappingView(
	const LOverlappingView&	inOriginal)

	: LView(inOriginal)
{
}


// ---------------------------------------------------------------------------
//	¥ LOverlappingView						Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LOverlappingView::LOverlappingView(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo)

	: LView(inPaneInfo, inViewInfo)
{
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LOverlappingView						Stream Constructor		  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LOverlappingView::LOverlappingView(
	LStream*	inStream)

	: LView(inStream)
{
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ ~LOverlappingView						Destructor				  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LOverlappingView::~LOverlappingView()
{
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ FocusDraw														  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Set up coordinates system and clipping region
//
//	An OverlappingView sets the clipping region to the revealed portion
//	of its Frame minus the Frames of any sibling Views that are in
//	front of it.

Boolean
LOverlappingView::FocusDraw(
	LPane*	/* inSubPane */)
{
	Boolean		revealed = true;

	if (this != sInFocusView) {		// Skip if already in focus
		if (EstablishPort()) {		// Set current Mac Port
									// Set up local coordinate system
			::SetOrigin(mPortOrigin.h, mPortOrigin.v);

									// Build clipping region

									// Start with revealed rect of this View
			StRegion	clipR(mRevealedRect);

				// Subtract Frames of all sibling Panes that are in front
				// of this View. Panes draw in the order in which they
				// appear in the SubPane list, so all the Panes after
				// us in out SuperView's SubPane list are in front of us.

									// Find our position in our SuperView's
									//   SubPane list. This is our birth
									//   order.
			TArray<LPane*>	&siblingPanes = mSuperView->GetSubPanes();
			ArrayIndexT	myIndex = siblingPanes.FetchIndexOf(this);

			StRegion	siblingR;
			LPane		*thePane;	// Start iterating with the sibling
									//   Pane after this View
			TArrayIterator<LPane*>	iterator(siblingPanes, myIndex);
			while (iterator.Next(thePane)) {
				Rect	siblingFrame;
				if (thePane->CalcPortFrameRect(siblingFrame)) {
									// Subtract sibling's Frame from
									//   the clipping region
					siblingR = siblingFrame;
					clipR -= siblingR;
				}
			}

									// Convert Clip region from Port to
									//   Local coords and set it
			::MacOffsetRgn(clipR, mPortOrigin.h, mPortOrigin.v);
			::SetClip(clipR);

			revealed = !clipR.IsEmpty();

			ApplyForeAndBackColors();	// Establish colors for View

			sInFocusView = this;		// Cache current Focus

		} else {
			SignalStringLiteral_("Focus View with no GrafPort");
			revealed = false;
		}
	}

	return revealed;
}


PP_End_Namespace_PowerPlant
