// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UClassicPrinting.cp			PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Classic implementation of UPrinting module

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UPrinting.h>

#if TARGET_API_MAC_CARBON

		// You must use UCarbonPrinting or USessionPrinting under Carbon.
		// Remove this file from Carbon targets.

	#error "Carbon does not support UClassicPrinting"

#endif

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//	LPrintSpec Class
// ===========================================================================
//	Wrapper class for Toolbox THPrint, a Handle to a TPrint record

// ---------------------------------------------------------------------------
//	Local constants

const PMPrintSession	printSession_Open	= (PMPrintSession) (-1);
const PMPrintSession	printSession_Closed	= nil;


// ---------------------------------------------------------------------------
//	¥ LPrintSpec							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LPrintSpec::LPrintSpec()
{
	mPrintSession = printSession_Closed;
}


// ---------------------------------------------------------------------------
//	¥ ~LPrintSpec							Destructor				  [public]
// ---------------------------------------------------------------------------

LPrintSpec::~LPrintSpec()
{
}


// ---------------------------------------------------------------------------
//	¥ BeginSession													  [public]
// ---------------------------------------------------------------------------

void
LPrintSpec::BeginSession(
	PMPrintSession	/* inSession */)
{
	SignalIf_( IsInSession() );			// Error: Session already open

	if (not mPrintRecordH.IsValid()) {	// Create a new PrintRecord with
										//   default values
		Handle	printRecH = ::NewHandle(sizeof(TPrint));
		ThrowIfMemFail_(printRecH);
		
		::PrintDefault((THPrint) printRecH);
		
		mPrintRecordH.Adopt(printRecH);
		
	} else {							// Make sure existing PrintRecord
										//   is still valid
		::PrValidate((THPrint) mPrintRecordH.Get());
	}
		
	mPrintSession = printSession_Open;
}


// ---------------------------------------------------------------------------
//	¥ EndSession													  [public]
// ---------------------------------------------------------------------------

void
LPrintSpec::EndSession()
{
	SignalIfNot_( IsInSession() );		// Error: Session is not open

	mPrintSession = printSession_Closed;
}


// ---------------------------------------------------------------------------
//	¥ IsInSession													  [public]
// ---------------------------------------------------------------------------

bool
LPrintSpec::IsInSession() const
{
	return (mPrintSession == printSession_Open);
}


// ---------------------------------------------------------------------------
//	¥ GetError														  [public]
// ---------------------------------------------------------------------------

OSStatus
LPrintSpec::GetError() const
{
	return ::PrError();
}


// ---------------------------------------------------------------------------
//	¥ GetPrintRecord												  [public]
// ---------------------------------------------------------------------------
//	Return classic Print Record Handle
//
//	Caller takes ownership of the Handle

Handle
LPrintSpec::GetPrintRecord() const
{
	return mPrintRecordH.GetCopy();
}


// ---------------------------------------------------------------------------
//	¥ SetPrintRecord												  [public]
// ---------------------------------------------------------------------------
//	Set the classic Print Record Handle
//
//	Do not call this function while a session is open
//
//	Caller retains ownership of the input Handle

void
LPrintSpec::SetPrintRecord(
	Handle		inPrintRecordH)
{
	if (IsInSession()) {
		SignalStringLiteral_("Can't set print record when session is open");
		
	} else {
		mPrintRecordH.CopyAndAdopt(inPrintRecordH);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetPrintJobParams												  [public]
// ---------------------------------------------------------------------------

void
LPrintSpec::GetPrintJobParams(
	UInt32&		outFirstPage,
	UInt32&		outLastPage,
	UInt32&		outNumCopies)
{
	THPrint	printRecH = (THPrint) mPrintRecordH.Get();
	
	if (printRecH != nil) {
		outFirstPage = (**printRecH).prJob.iFstPage;
		outLastPage  = (**printRecH).prJob.iLstPage;
		outNumCopies = (**printRecH).prJob.iCopies;
	}
}


// ---------------------------------------------------------------------------
//	¥ SetPrintJobParams												  [public]
// ---------------------------------------------------------------------------

void
LPrintSpec::SetPrintJobParams(
	UInt32		inFirstPage,
	UInt32		inLastPage,
	UInt32		inNumCopies)
{
	THPrint	printRecH = (THPrint) mPrintRecordH.Get();
	
	if (printRecH != nil) {
		(**printRecH).prJob.iFstPage = (SInt16) inFirstPage;
		(**printRecH).prJob.iLstPage = (SInt16) inLastPage;
		(**printRecH).prJob.iCopies  = (SInt16) inNumCopies;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetPaperRect													  [public]
// ---------------------------------------------------------------------------

void
LPrintSpec::GetPaperRect(
	Rect&	outPaperRect)
{
	THPrint	printRecH = (THPrint) mPrintRecordH.Get();

	if (printRecH != nil) {
		outPaperRect = (**printRecH).rPaper;

	} else {
		outPaperRect = Rect_0000;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetPageRect													  [public]
// ---------------------------------------------------------------------------

void
LPrintSpec::GetPageRect(
	Rect&	outPageRect)
{
	THPrint	printRecH = (THPrint) mPrintRecordH.Get();

	if (printRecH != nil) {
		outPageRect = (**printRecH).prInfo.rPage;

	} else {
		outPageRect = Rect_0000;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetTHPrint													  [public]
// ---------------------------------------------------------------------------
//	Return Handle to actual PrintRecord
//
//	Classic-only function. GetPrintRecord() returns a copy of the Print
//	Record Handle, whereas this function returns the Handle itself.
//	Clients may modify the Handle, which will affect printing operations
//	using this PrintSpec.
//
//	PrintSpec retains ownership of the Handle. Do not dispose it.

THPrint
LPrintSpec::GetTHPrint() const
{
	return (THPrint) mPrintRecordH.Get();
}


#pragma mark -
// ===========================================================================
//	StPrintContext Class
// ===========================================================================
//
//	Constructor opens printing for a document
//	Destructor closes printing for a document
//
//	A Print Session must be open before using any functions of this class.

// ---------------------------------------------------------------------------
//	¥ StPrintContext						Constructor
// ---------------------------------------------------------------------------

StPrintContext::StPrintContext(
	const LPrintSpec&	inPrintSpec)
{
								// Print Session must be open
	SignalIfNot_( inPrintSpec.IsInSession() );

	mPrinterPort = ::PrOpenDoc(inPrintSpec.GetTHPrint(), nil , nil);
}


// ---------------------------------------------------------------------------
//	¥ ~StPrintContext						Destructor
// ---------------------------------------------------------------------------

StPrintContext::~StPrintContext()
{
	::PrCloseDoc(mPrinterPort);
	mPrinterPort = nil;
}


// ---------------------------------------------------------------------------
//	¥ GetGrafPtr
// ---------------------------------------------------------------------------
//	Return GrafPtr being used for printing

GrafPtr
StPrintContext::GetGrafPtr() const
{
	return (GrafPtr) mPrinterPort;
}


// ---------------------------------------------------------------------------
//	¥ BeginPage
// ---------------------------------------------------------------------------
//	Begin printing a new page
//
//	Each BeginPage() call must be balanced by a following EndPage() call

void
StPrintContext::BeginPage()
{
	::PrOpenPage(mPrinterPort, nil);
}


// ---------------------------------------------------------------------------
//	¥ EndPage
// ---------------------------------------------------------------------------
//	End printing a new page
//
//	Each EndPage() call must be balanced by a preceding BeginPage() call

void
StPrintContext::EndPage()
{
	::PrClosePage(mPrinterPort);
}


#pragma mark -
// ===========================================================================
//	StPrintSession Class
// ===========================================================================
//
//	Constructor begins a print session
//	Destructor ends a print session


StPrintSession::StPrintSession(
	LPrintSpec&	ioPrintSpec)

	: mPrintSpec(ioPrintSpec)
{
	::PrOpen();
	ThrowIfPrError_();

	mPrintSpec.BeginSession(nil);
}


StPrintSession::~StPrintSession()
{
	mPrintSpec.EndSession();

	::PrClose();
}


#pragma mark -
// ===========================================================================
//	UPrinting Class
// ===========================================================================

// ---------------------------------------------------------------------------
//	Class Variables

LPrintSpec	UPrinting::sAppPrintSpec;


// ---------------------------------------------------------------------------
//	¥ AskPageSetup
// ---------------------------------------------------------------------------
//	Display the standard Page Setup dialog.
//
//	If the user OK's the settings, the ioPrintSpec is changed to the
//	new settings and this function returns true. If the user Cancels,
//	ioPrintSpec is unchanged and this function returns false.
//
//	You must open a Print Session before calling this function.

bool
UPrinting::AskPageSetup(
	LPrintSpec&	ioPrintSpec)
{
								// Print Session must be open
	SignalIfNot_( ioPrintSpec.IsInSession() );

	return ::PrStlDialog( ioPrintSpec.GetTHPrint() );
}


// ---------------------------------------------------------------------------
//	¥ AskPrintJob
// ---------------------------------------------------------------------------
//	Display the standard Print Job Dialog.
//
//	If the user OK's the job, the ioPrintSpec is changed to the
//	new settings and this function returns true. If the user Cancels,
//	ioPrintSpec is unchanged and this function returns false.
//
//	You must open a Print Session before calling this function.

bool
UPrinting::AskPrintJob(
	LPrintSpec&	ioPrintSpec)
{
								// Print Session must be open
	SignalIfNot_( ioPrintSpec.IsInSession() );

	return ::PrJobDialog( ioPrintSpec.GetTHPrint() );
}


PP_End_Namespace_PowerPlant
