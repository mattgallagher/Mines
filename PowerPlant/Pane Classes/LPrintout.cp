// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPrintout.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A View for printing
//
//	A Printout object is top level container of a view hierarchy that is
//	being printed. It deals with the Mac Print Manager and coordinates
//	its subpanes during the printing process.
//
//	To print, you must create a Printout object in which you install all
//	the Panes that you want to print. Usually, your Printout has the
//	dimensions of the paper size (as displayed in the Page Setup dialog).
//	Then, the location of a Pane in the Printout is the same as the location
//	on the Paper where the Pane will print.
//
//	To print a Pane that is inside a Window, you should use a LPlaceHolder
//	view in the Printout to specify the size and location of the Pane when
//	it prints. You then install the Pane inside the PlaceHolder. When
//	printing completes, the PlaceHolder returns the Pane to its original
//	place in the Window.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LPrintout.h>
#include <LStream.h>
#include <LCommander.h>
#include <PP_Resources.h>
#include <TArrayIterator.h>
#include <UReanimator.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UTBAccessors.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ CreatePrintout												  [static]
// ---------------------------------------------------------------------------
//	Return a new Printout object (and its SubPanes) from the data in
//	a 'PPob' resource

LPrintout*
LPrintout::CreatePrintout(
	ResIDT		inPrintoutID)
{
	LCommander::SetDefaultCommander(nil);
						
	LPrintout*	thePrintout =
		UReanimator::ReanimateObjects<LPrintout>(ResType_PPob, inPrintoutID);
				
	ThrowIfNil_(thePrintout);
	
	try {		
		thePrintout->FinishCreate();
	}
	
	catch (...) {						// FinishCreate failed. Printout is
		delete thePrintout;				//   in an inconsistent state.
		throw;							//   Delete it and rethrow.
	}

	return thePrintout;
}


// ---------------------------------------------------------------------------
//	¥ LPrintout								Default Constructor		  [public]
// ---------------------------------------------------------------------------

LPrintout::LPrintout()
{
	InitPrintout();
}


// ---------------------------------------------------------------------------
//	¥ LPrintout								Constructor				  [public]
// ---------------------------------------------------------------------------
//	 Construct from a LPrintSpec

LPrintout::LPrintout(
	LPrintSpec&		inPrintSpec)
{
	InitPrintout();
	SetPrintSpec(inPrintSpec);
}


// ---------------------------------------------------------------------------
//	¥ LPrintout								Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LPrintout::LPrintout(
	LStream*	inStream)
{
	InitPrintout();

	SPrintoutInfo	thePrintoutInfo;
	*inStream >> thePrintoutInfo.width;
	*inStream >> thePrintoutInfo.height;
	*inStream >> thePrintoutInfo.active;
	*inStream >> thePrintoutInfo.enabled;
	*inStream >> thePrintoutInfo.userCon;
	*inStream >> thePrintoutInfo.attributes;

	mFrameSize.width  = thePrintoutInfo.width;
	mFrameSize.height = thePrintoutInfo.height;

	mActive = triState_Off;
	if (thePrintoutInfo.active) {
		mActive = triState_On;
	}

	mEnabled = triState_Off;
	if (thePrintoutInfo.enabled) {
		mEnabled = triState_On;
	}

	mUserCon    = thePrintoutInfo.userCon;
	mAttributes = thePrintoutInfo.attributes;

	SetDefaultView(this);
}


// ---------------------------------------------------------------------------
//	¥ InitPrintout							Initializer				 [private]
// ---------------------------------------------------------------------------

void
LPrintout::InitPrintout()
{
	mAttributes		 = 0;
	mHorizPanelCount = 1;
	mVertPanelCount  = 1;
	mForeColor		 = Color_Black;
	mBackColor		 = Color_White;
	mPrintSpec		 = nil;
	mPrinterPort	 = nil;

									// Create an empty, invisible window
									//   for installing Toolbox Controls
	Rect	bounds = Rect_0000;

	mWindowPort = ::NewCWindow(nil, &bounds, Str_Empty, false, documentProc,
							nil, false, 0);
	FocusDraw();

	mVisible = triState_On;			// A Prinout is always visible
}


// ---------------------------------------------------------------------------
//	¥ ~LPrintout							Destructor				  [public]
// ---------------------------------------------------------------------------

LPrintout::~LPrintout()
{
	DeleteAllSubPanes();

	if (mWindowPort != nil) {
		::DisposeWindow(mWindowPort);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetPrintSpec													  [public]
// ---------------------------------------------------------------------------

LPrintSpec*
LPrintout::GetPrintSpec() const
{
	return mPrintSpec;
}


// ---------------------------------------------------------------------------
//	¥ SetPrintSpec													  [public]
// ---------------------------------------------------------------------------

void
LPrintout::SetPrintSpec(
	LPrintSpec&		inPrintSpec)
{
	SignalIfNot_( inPrintSpec.IsInSession() );

	mPrintSpec = &inPrintSpec;

	AdjustToPaperSize();
}


// ---------------------------------------------------------------------------
//	¥ HasAttribute
// ---------------------------------------------------------------------------
//	Return whether a Printout has the specified attribute

Boolean
LPrintout::HasAttribute(
	EPrintAttr	inAttribute) const
{
	return ((mAttributes & inAttribute) != 0);
}


// ---------------------------------------------------------------------------
//	¥ SetAttribute
// ---------------------------------------------------------------------------
//	Specify an attribute for a Printout

void
LPrintout::SetAttribute(
	EPrintAttr	inAttribute)
{
	mAttributes |= inAttribute;
}


// ---------------------------------------------------------------------------
//	¥ ClearAttribute
// ---------------------------------------------------------------------------
//	Remove an attribute from a Printout

void
LPrintout::ClearAttribute(
	EPrintAttr	inAttribute)
{
	mAttributes &= ~inAttribute;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DoPrintJob
// ---------------------------------------------------------------------------

void
LPrintout::DoPrintJob()
{
	PanelSpec	firstPanel;
	PanelSpec	lastPanel;
	UInt16		copyCount;

	SignalIf_( mPrintSpec == nil );

	GetPrintJobSpecs(firstPanel, lastPanel, copyCount);
	PrintPanelRange(firstPanel, lastPanel, copyCount);
}


// ---------------------------------------------------------------------------
//	¥ PrintPanelRange
// ---------------------------------------------------------------------------

void
LPrintout::PrintPanelRange(
	const PanelSpec&	inFirstPanel,
	const PanelSpec&	inLastPanel,
	UInt16				inCopyCount)
{
		// Most modern print drivers display their own status dialog.
		// If you want to display your own status dialog, you should
		// override this function.

			// +++ Show print status dialog

	PrintCopiesOfPages(inFirstPanel, inLastPanel, inCopyCount);

	OSStatus	err = mPrintSpec->GetError();

									// +++ Put error message in print status
									//   dialog instead of throwing

	if ( (err != noErr)  &&
		 (err != userCanceledErr)  &&		// Canceling print job is OK
		 (err != -userCanceledErr) ) {		// Some print drivers flip the sign

		Throw_(err);
	}

			// +++ Remove print status dialog
}


// ---------------------------------------------------------------------------
//	¥ CountPanels
// ---------------------------------------------------------------------------
//	Count the number of Panels in a Printout

void
LPrintout::CountPanels(
	UInt32&		outHorizPanels,
	UInt32&		outVertPanels)
{
		// The number of Panels in a Printout is the maximum number
		// (both horizonal and vertical) of Panels in its SubPanes

	UInt32	maxHorizPanels = 1;
	UInt32	maxVertPanels  = 1;

	TArrayIterator<LPane*>	iterator(GetSubPanes());
	LPane	*subPane;
	while (iterator.Next(subPane)) {
		UInt32	subHorizPanels;
		UInt32	subVertPanels;
		subPane->CountPanels(subHorizPanels, subVertPanels);
		if (subHorizPanels > maxHorizPanels) {
			maxHorizPanels = subHorizPanels;
		}
		if (subVertPanels > maxVertPanels) {
			maxVertPanels = subVertPanels;
		}
	}
									// Save counts in member variables
									//   and set output parameters
	mHorizPanelCount = outHorizPanels = maxHorizPanels;
	mVertPanelCount  = outVertPanels = maxVertPanels;
}


// ---------------------------------------------------------------------------
//	¥ PageToPanel
// ---------------------------------------------------------------------------
//	Fill in PanelSpec for a given page number

void
LPrintout::PageToPanel(
	UInt32		inPageNumber,
	PanelSpec&	outPanel) const
{
	outPanel.pageNumber = inPageNumber;

	if (HasAttribute(printAttr_NumberDown)) {
		outPanel.horizIndex = ((inPageNumber - 1) / mVertPanelCount) + 1;
		outPanel.vertIndex  = ((inPageNumber - 1) % mVertPanelCount) + 1;

	} else {
		outPanel.horizIndex = ((inPageNumber - 1) % mHorizPanelCount) + 1;
		outPanel.vertIndex  = ((inPageNumber - 1) / mHorizPanelCount) + 1;
	}
}

// ---------------------------------------------------------------------------
//	¥ AdjustToPaperSize
// ---------------------------------------------------------------------------


void
LPrintout::AdjustToPaperSize()
{
		// Size and coordinate system of a Printout depend on the Page
		// and Paper rectangles of the PrintRecord. The Print Manager sets
		// the port rectangle of the Printer Port to the Page rectangle.
		//
		// The Page rectangle is usually smaller than the Paper rectangle
		// since most printers have mechanical limitations which prevent
		// them from printing to the very edge of the paper.
		//
		// For Printouts, it seems more natural to have (0,0) be the top left
		// of the Paper rectangle, so that coordinates are the absolute
		// location on the paper. This simplifies setting margins and
		// otherwise placing Panes for printing.

	Rect	paperRect;
	mPrintSpec->GetPaperRect(paperRect);

		// Printout Frame and Image match the Paper Rectangle

	ResizeFrameTo( (SInt16) (paperRect.right - paperRect.left),
				   (SInt16) (paperRect.bottom - paperRect.top), Refresh_No );

	ResizeImageTo( paperRect.right - paperRect.left,
				   paperRect.bottom - paperRect.top, Refresh_No );

	PlaceInSuperImageAt(paperRect.left, paperRect.top, Refresh_No);
}


// ---------------------------------------------------------------------------
//	¥ GetPrintJobSpecs
// ---------------------------------------------------------------------------
//	Extract information about the print job from the Print Specifications
//		- First Page to print (mapped to a PanelSpec)
//		- Last Page to print (mapped to a PanelSpec)
//		- Number of copies to print

void
LPrintout::GetPrintJobSpecs(
	PanelSpec&	outFirstPanel,
	PanelSpec&	outLastPanel,
	UInt16&		outCopyCount)
{
	UInt32		firstPage;
	UInt32		lastPage;
	UInt32		numCopies;

	mPrintSpec->GetPrintJobParams(firstPage, lastPage, numCopies);

		// The Print Manager doesn't care about our page numbers. It
		// counts the number of calls to PrOpenPage. So setting the
		// page range to 1 to max_Pages will print all pages that we draw.

	mPrintSpec->SetPrintJobParams(1, UPrinting::max_Pages, numCopies);

		// Enforce bounds on page range. Last page can't be greater
		// than number of pages in Printout, and first page can't
		// be greater than the last page.

	UInt32	horizPanelCount;
	UInt32	vertPanelCount;
	CountPanels(horizPanelCount, vertPanelCount);
	UInt32	pageCount = horizPanelCount * vertPanelCount;

	if (lastPage > pageCount) {
		lastPage = (SInt16) pageCount;
	}

	if (firstPage > lastPage) {
		firstPage = lastPage;
	}

	PageToPanel(firstPage, outFirstPanel);
	PageToPanel(lastPage, outLastPanel);
	outCopyCount = (UInt16) numCopies;
}


// ---------------------------------------------------------------------------
//	¥ PrintCopiesOfPages
// ---------------------------------------------------------------------------
//	Print copies of the specified range of Panels
//
//	This is the main "print loop"

void
LPrintout::PrintCopiesOfPages(
	const PanelSpec&	inFirstPanel,
	const PanelSpec&	inLastPanel,
	UInt16				inCopyCount)
{
	SignalIf_( mPrintSpec == nil );

	#if TARGET_API_MAC_CARBON		// Carbon Print Manager automatically
									//   handles the number of copies
		inCopyCount = 1;			//   so we don't have to make multiple
									//   passes over the print loop
	#endif

	SInt16	printMgrResFile = ::CurResFile();

	for (UInt16 copy = 1; copy <= inCopyCount; copy++) {

		// +++ Install pIdle proc

		::UseResFile(printMgrResFile);

		StPrintContext	printContext(*mPrintSpec);


									// Loop over Panels
		PanelSpec	thePanel = inFirstPanel;
		while ( (thePanel.pageNumber <= inLastPanel.pageNumber) &&
				(mPrintSpec->GetError() == noErr) ) {

			printContext.BeginPage();

			OutOfFocus(nil);		// Printer is the current port
			mPrinterPort = printContext.GetGrafPtr();

									// Set bit depth for drawing
			sBitDepth = UDrawingUtils::GetPortPixelDepth(mPrinterPort);
			sDeviceHasColor = (sBitDepth > 1);

			if (mPrintSpec->GetError() == noErr) {

				try {

					// BeginPage resets the portRect, but it doesn't
					// reset the visRgn. Since PP calls SetOrigin, the
					// visRgn gets out of synch unless we reset it here.

					Rect		portRect;
					::GetPortBounds(mPrinterPort, &portRect);

					StRegion	visRgn(portRect);
					::SetPortVisibleRegion(mPrinterPort, visRgn);

					PrintPanel(thePanel, nil);
				}

				catch (...) { }		// If we fail to print this page, move
			}						//   on to the next page

			printContext.EndPage();

			PageToPanel(++thePanel.pageNumber, thePanel);
		}
	}

	mPrinterPort = nil;
	sBitDepth = 0;

	OutOfFocus(nil);				// Printer port is gone

#if PP_Target_Classic
									// Spooling not supported under Carbon.
									// Most print drivers do their own
									//   spooling.

	THPrint	printRecordH = mPrintSpec->GetTHPrint();

	if ( ((**printRecordH).prJob.bJDocLoop == bSpoolLoop) &&
		 (::PrError() == noErr) ) {

		TPrStatus	printStatus;
		::PrPicFile(printRecordH, nil, nil, nil, &printStatus);
	}

#endif
}

// ---------------------------------------------------------------------------
//	¥ PrintPanel
// ---------------------------------------------------------------------------
//	Print the specified Panel

void
LPrintout::PrintPanel(
	const PanelSpec&	inPanel,
	RgnHandle			inSuperPrintRgnH)
{
	FocusDraw();
	PrintPanelSelf(inPanel);		// Print this Printout

									// Print SubPanes
	TArrayIterator<LPane*>	iterator(GetSubPanes());
	LPane	*subPane;
	while (iterator.Next(subPane)) {
		subPane->PrintPanel(inPanel, inSuperPrintRgnH);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetMacPort
// ---------------------------------------------------------------------------
//	Return the GrafPort associated with a Printout

GrafPtr
LPrintout::GetMacPort() const
{
		// A Printer Port only exists during the print loop. At other
		// times, we have to use the window port.

	GrafPtr	printoutPort = mPrinterPort;
	if (mPrinterPort == nil) {
		printoutPort = (GrafPtr) GetWindowPort(mWindowPort);
	}
	return printoutPort;
}


// ---------------------------------------------------------------------------
//	¥ EstablishPort
// ---------------------------------------------------------------------------
//	Make Printout the current Port

Boolean
LPrintout::EstablishPort()
{
	GrafPtr	thisPort = GetMacPort();
	Boolean	portSet = (thisPort != nil);
	if (portSet && (UQDGlobals::GetCurrentPort() != thisPort)) {
		::MacSetPort(thisPort);
	}
	return portSet;
}


// ---------------------------------------------------------------------------
//	¥ ApplyForeAndBackColors
// ---------------------------------------------------------------------------
//	Set the foreground and background colors of the current port
//
//	The Printout or one of its SubPanes must already be focused

void
LPrintout::ApplyForeAndBackColors() const
{
	::RGBForeColor(&mForeColor);
	::RGBBackColor(&mBackColor);
}


// ---------------------------------------------------------------------------
//	¥ SetForeAndBackColors
// ---------------------------------------------------------------------------
//	Sprecify the foreground and/or background colors of a Printout
//
//	Specify nil for inForeColor and/or inBackColor to leave that
//	color trait unchanged

void
LPrintout::SetForeAndBackColors(
	const RGBColor*		inForeColor,
	const RGBColor*		inBackColor)
{
	if (inForeColor != nil) {
		mForeColor = *inForeColor;
	}

	if (inBackColor != nil) {
		mBackColor = *inBackColor;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetForeAndBackColors
// ---------------------------------------------------------------------------
//	Pass back the foreground and/or background colors
//
//	Pass nil for outForeColor and/or outBackColor to not retrieve that color

void
LPrintout::GetForeAndBackColors(
	RGBColor*	outForeColor,
	RGBColor*	outBackColor) const
{
	if (outForeColor != nil) {
		*outForeColor = mForeColor;
	}

	if (outBackColor != nil) {
		*outBackColor = mBackColor;
	}
}


PP_End_Namespace_PowerPlant
