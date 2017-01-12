// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMouseTracker.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	Periodical that tracks which Pane contains the mouse and calls the
//	MouseEnter, MouseWithin, and MouseLeave LPane functions
//
//	Tracking occurs for all visible Panes regardless of whether they
//	are Active or Enabled.
//
//	Install as a Repeater to track the mouse after every event
//	Install as an Idler to track the mouse only on null and mouse moved
//	If using Carbon 1.1 or later, install as a Repeater or Idler, specifying
//		an interval (in seconds) at which to track the mouse

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LMouseTracker.h>
#include <LWindow.h>

#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LMouseTracker							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LMouseTracker::LMouseTracker()
{
}


// ---------------------------------------------------------------------------
//	¥ ~LMouseTracker						Destructor				  [public]
// ---------------------------------------------------------------------------

LMouseTracker::~LMouseTracker()
{
}


// ---------------------------------------------------------------------------
//	¥ SpendTime														  [public]
// ---------------------------------------------------------------------------
//	Track the mouse

void
LMouseTracker::SpendTime(
	const EventRecord	&inMacEvent)
{
	if (inMacEvent.what == kHighLevelEvent) {	// High level events don't
		return;									// pass the mouse location
	}											// in the EventRecord

	WindowPtr	macWindowP;				// Ask Toolbox where the mouse is
	SInt16		thePart = ::MacFindWindow(inMacEvent.where, &macWindowP);

	LPane		*paneWithMouse = nil;
	Point		portMouse;

	if ((thePart == inContent) || (thePart == inGrow)) {
										// Mouse is inside a Window

		LWindow	*theWindow = LWindow::FetchWindowObject(macWindowP);
		if (theWindow != nil) {
										// Mouse is inside a PP Window
			portMouse = inMacEvent.where;
			theWindow->GlobalToPortPoint(portMouse);

										// Find Pane containing the mouse
			paneWithMouse = theWindow->FindDeepSubPaneContaining(
											portMouse.h, portMouse.v);

			if (paneWithMouse == nil) {	// Mouse is in the Window but not
										//   inside any subpane
				paneWithMouse = theWindow;
			}
		}
	}
										// Get Pane that contained mouse
										//   the last time we checked
	LPane	*lastPaneMoused = LPane::GetLastPaneMoused();

	if (paneWithMouse != lastPaneMoused) {
										// Different Pane
		if (lastPaneMoused != nil) {
			lastPaneMoused->MouseLeave();	// Mouse left last Pane
		}

		if (paneWithMouse != nil) {			// Mouse entered new Pane
			paneWithMouse->MouseEnter(portMouse, inMacEvent);
		}

		LPane::SetLastPaneMoused(paneWithMouse);

	} else if (paneWithMouse != nil) {	// Mouse is in the same Pane
		paneWithMouse->MouseWithin(portMouse, inMacEvent);
	}
}


PP_End_Namespace_PowerPlant
