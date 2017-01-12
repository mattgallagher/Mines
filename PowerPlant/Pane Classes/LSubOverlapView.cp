// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSubOverlapView.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Container View for Panes which might overlap.

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LSubOverlapView.h>
#include <TArrayIterator.h>
#include <URegions.h>

PP_Begin_Namespace_PowerPlant


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LSubOverlapView						Default Constructor		  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LSubOverlapView::LSubOverlapView()
{
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LSubOverlapView						Copy Constructor		  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LSubOverlapView::LSubOverlapView(
	const LSubOverlapView&	inOriginal)

	: LView(inOriginal)
{
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LSubOverlapView						Parameterized Constructor [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LSubOverlapView::LSubOverlapView(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo)

	: LView(inPaneInfo, inViewInfo)
{
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LSubOverlapView						Stream Constructor		  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LSubOverlapView::LSubOverlapView(
	LStream*	inStream)

	: LView(inStream)
{
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ ~LSubOverlapView						Destructor				  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LSubOverlapView::~LSubOverlapView()
{
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ FocusDraw														  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Set up coordinates system and clipping region
//
//	Panes rely on their superview to focus them. When a Pane requests
//	focus for drawing, a SubOverlapView sets the clipping region to
//	revealed portion of that Pane's Frame minus the Frames of all
//	sibling Panes that are in front of that Pane.

Boolean
LSubOverlapView::FocusDraw(
	LPane	*inSubPane)
{
	Boolean		revealed = true;

	if (inSubPane == nil) {			// Focus this view
		revealed = LView::FocusDraw(nil);

	} else {						// Focus a SubPane

		if (EstablishPort()) {		// Set current Mac Port
			sInFocusView = nil;		// Saved focus is now invalid

									// Set up local coordinate system
			::SetOrigin(mPortOrigin.h, mPortOrigin.v);

									// Build clipping region

									// Start with the intersection of the
									//   revealed rect of this View with the
									//   Frame of the SubPane
			Rect	subRect;
			inSubPane->CalcPortFrameRect(subRect);

			if (!::SectRect(&subRect, &mRevealedRect, &subRect)) {
									// No intersection, so subpane is
									//   not revealed. Set empty clip.
				::ClipRect(&subRect);
				return false;
			}

									// SubPane is revealed. Make region
									//   from the intersection.
			StRegion	clipR(subRect);

				// Subtract Frames of all sibling Panes that are in front
				// of this SubPane (i.e., come after the SubPane in
				// this View's list of SubPanes.

									// Find positon of the SubPane
			ArrayIndexT	subIndex = mSubPanes.FetchIndexOf(inSubPane);

			StRegion	siblingR;
			LPane	*thePane;		// Start iterating with the next SubPane
			TArrayIterator<LPane*>	iterator(mSubPanes, subIndex);
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

		} else {
			SignalStringLiteral_("Focus View with no GrafPort");
			revealed = false;
		}

	}

	return revealed;
}

PP_End_Namespace_PowerPlant
