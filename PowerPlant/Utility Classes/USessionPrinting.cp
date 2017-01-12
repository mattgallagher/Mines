// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	USessionPrinting.cp			PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================
//
//	Implementation of UPrinting module using the Carbon Print Session APIs
//
//	NOTE: This implmenetation requires Carbon 1.1 or later. It will NOT
//	work with CarbonLib 1.0.x.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UPrinting.h>

#if PP_Target_Classic

		// You must use UClassicPrinting for Classic targets.
		// Remove this file from Classic targets.

	#error "Classic target does not support USessionPrinting"

#elif !PM_USE_SESSION_APIS

		// You must turn on the Toolbox flag PM_USE_SESSION_APIS
		// to use USessionPrinting. However, the session APIs reguire
		// Carbon 1.1 or later. Use UCarbonPrinting if you wish to
		// support Carbon 1.0.x.

	#error "Turn on the PM_USE_SESSION_APIS flag to use USessionPrinting"
	
#endif

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//	LPrintSpec Class
// ===========================================================================
//	Wrapper class for Toolbox print information data structures:
//		THPrint			- Classic printing info
//		PMPageFormat	- Info from Page Setup [Carbon]
//		PMPrintSettings	- Info from Print Dialog [Carbon]
//
//	Data Persistence Strategy:
//		In the Session API, PMPageFormat and PMPrintSettings references
//		remain valid after a session ends. This differs from the non-session
//		API, where such references are invalidated. Thus, there is no need
//		to flatten/unflatten the PageFormat and PrintSettings to save/restore
//		information.
//
//		Flattened PageFormat and PrintSettings are never stored. They are
//		generated on demand by the "get" functions, and converted immediately
//		to references by the "set" functions.
//
//		However, converting to/from a PrintRecord does require an open
//		session. Thus, we create and store a PrintRecord when a session ends
//		so that it is available via the "get" function. A PrintRecord
//		passed to the "set" function is stored and converted to
//		PageFormat and PrintSettings at the start of the next session.
//
//		When restoring specs, you should use either PageFormat/PrintSettings
//		or PrintRecords, but not both. If you do restore both, the last one
//		"set" will be used. For example, if you call SetPrintRecord() and
//		then call SetPageFormat(), the PrintRecord will be deleted and the
//		PageFormat will be used.


// ---------------------------------------------------------------------------
//	¥ LPrintSpec							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LPrintSpec::LPrintSpec()
{
	mPageFormat		= nil;
	mPrintSettings	= nil;
	mPrintSession	= nil;
	mUsePrintRecord	= false;
}


// ---------------------------------------------------------------------------
//	¥ ~LPrintSpec							Destructor				  [public]
// ---------------------------------------------------------------------------

LPrintSpec::~LPrintSpec()
{
	if (mPageFormat != nil) {
		::PMRelease(mPageFormat);
	}
	
	if (mPrintSettings != nil) {
		::PMRelease(mPrintSettings);
	}
	
		// Print Session should be nil since client is supposed to
		// balance calls to BegingSession() and EndSession(). But
		// it doesn't do any harm to clean up here just in case.
	
	if (mPrintSession != nil) {
		::PMRelease(mPrintSession);
	}
}


// ---------------------------------------------------------------------------
//	¥ BeginSession
// ---------------------------------------------------------------------------
//	Start a print session
//
//	When a session is open, LPrintSpec members have the following state:
//
//		mPrintSession		Validated Reference
//		mPageFormat			Validated Reference
//		mPrintSettings		Validated Reference
//		mPrintRecordH		nil

void
LPrintSpec::BeginSession(
	PMPrintSession	inSession)
{
	SignalIf_( IsInSession() );		// Session already open
	
	OSStatus	status;
	
	if (mUsePrintRecord and mPrintRecordH.IsValid()) {
	
									// Create PageFormat and PrintSettings
									//   from a saved PrintRecord
		status = ::PMSessionConvertOldPrintRecord(
									inSession,
									mPrintRecordH,
									&mPrintSettings,
									&mPageFormat );
									
	} else {
		if (mPageFormat != nil) {		// Validate existing PageFormat.
			status = ::PMSessionValidatePageFormat(inSession, mPageFormat, nil);
		}
			
		if (mPrintSettings != nil) {	// Validate existing PrintSettings
			status = ::PMSessionValidatePrintSettings(inSession, mPrintSettings, nil);
		}
	}
	
	mPrintRecordH.Adopt(nil);
	mUsePrintRecord = false;
	
	if (mPageFormat == nil) {		// Create a new PageFormat with default
									//   values.
		status = ::PMCreatePageFormat(&mPageFormat);
		status = ::PMSessionDefaultPageFormat(inSession, mPageFormat);
	}
	
	if (mPrintSettings == nil) {	// Create a new PrintSettings with default
									//   values
		status = ::PMCreatePrintSettings(&mPrintSettings);
		status = ::PMSessionDefaultPrintSettings(inSession, mPrintSettings);
	}

	::PMRetain(inSession);			// Retain and store Session reference
	mPrintSession = inSession;
}


// ---------------------------------------------------------------------------
//	¥ EndSession
// ---------------------------------------------------------------------------
//	End a print session
//
//	After a session ends, the LPrintSpec members have the following state:
//
//		mPrintSession		nil
//		mPageFormat			Reference
//		mPrintSettings		Reference
//		mPrintRecordH		Handle with converted Format and Settings data

void
LPrintSpec::EndSession()
{
	SignalIfNot_( IsInSession() );		// Session should be open
	
	if ( (mPrintSettings != nil)  &&	// Save PrintRecord using current
		 (mPageFormat != nil) ) {		//   PageFormat and PrintSettings
		 
		 Handle		printRecH = nil;

		::PMSessionMakeOldPrintRecord(
										mPrintSession,
										mPrintSettings,
										mPageFormat,
										&printRecH);
		
		mUsePrintRecord = false;
		mPrintRecordH.Adopt(printRecH);
	}

	::PMRelease(mPrintSession);
	mPrintSession = nil;
}


// ---------------------------------------------------------------------------
//	¥ IsInSession													  [public]
// ---------------------------------------------------------------------------

bool
LPrintSpec::IsInSession() const
{
	return (mPrintSession != nil);
}


// ---------------------------------------------------------------------------
//	¥ GetError														  [public]
// ---------------------------------------------------------------------------

OSStatus
LPrintSpec::GetError() const
{
	OSStatus	status = kPMNoError;
	
	if (IsInSession()) {
		status = ::PMSessionError(mPrintSession);
	}
	
	return status;
}


// ---------------------------------------------------------------------------
//	¥ GetPrintRecord												  [public]
// ---------------------------------------------------------------------------
//	Return classic Print Record Handle
//
//	Handle may be nil if printing isn't in progress, and no previous
//	printing operation occurred.
//
//	Caller takes ownership of the Handle

Handle
LPrintSpec::GetPrintRecord() const
{
	Handle	printRecH = nil;

	if ( (mPrintSession != nil) &&
		 (mPrintSettings != nil) &&
		 (mPageFormat != nil) ) {
	
			// Create a new PrintRecord based on the current
			// printing information
	
		ThrowIfOSStatus_( ::PMSessionMakeOldPrintRecord(
										mPrintSession,
										mPrintSettings,
										mPageFormat,
										&printRecH) );
										
	} else {
			
		printRecH = mPrintRecordH.GetCopy();
	
	}

	return printRecH;
}


// ---------------------------------------------------------------------------
//	¥ SetPrintRecord												  [public]
// ---------------------------------------------------------------------------
//	Set the old-style PrintRecord
//
//	The input Handle must be a handle to a PrintRecord (TPrint struct), and
//	will be used to create the PMPageFormat and PrintSettings information the
//	next time a session is started. The Handle may be nil, which means to
//	use default values.
//
//	Do not call this function while a session is open.
//
//	Caller retains ownership of the Handle.

void
LPrintSpec::SetPrintRecord(
	Handle		inPrintRecordH)
{
	if (IsInSession()) {
		SignalStringLiteral_("Can't set print record when session is open");
		
	} else {
										// Store copy of PrintRecord
		mPrintRecordH.CopyAndAdopt(inPrintRecordH);
		
			// We must set a flag to distinguish between this situation,
			// where a PrintRecord is set externally, and the case
			// where a PrintRecord is saved at the end of a session.
		
		mUsePrintRecord	= true;
		
										// Discard existing PageFormat and
										//   PrintSettings data
		if (mPageFormat != nil) {
			::PMRelease(mPageFormat);
		}
		
		if (mPrintSettings != nil) {
			::PMRelease(mPrintSettings);
		}
		
		mPageFormat = nil;
		mPrintSettings = nil;
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetPrintSession												  [public]
// ---------------------------------------------------------------------------

PMPrintSession
LPrintSpec::GetPrintSession() const
{
	return mPrintSession;
}


// ---------------------------------------------------------------------------
//	¥ GetPageFormat													  [public]
// ---------------------------------------------------------------------------

PMPageFormat
LPrintSpec::GetPageFormat() const
{
	return mPageFormat;
}


// ---------------------------------------------------------------------------
//	¥ GetPageFlatFormat												  [public]
// ---------------------------------------------------------------------------
//	Return Handle with saved PageFormat data
//
//	Handle may be nil if printing isn't in progress, and no previous
//	printing operation occurred.
//
//	Caller takes ownership of the Handle

Handle
LPrintSpec::GetFlatPageFormat() const
{
	Handle	flatFormatH = nil;

	if (mPageFormat != nil) {
	
		// Make a new Handle from the current PageFormat data
	
		::PMFlattenPageFormat(mPageFormat, &flatFormatH);
	}
		
	return flatFormatH;
}


// ---------------------------------------------------------------------------
//	¥ SetFlatPageFormat												  [public]
// ---------------------------------------------------------------------------
//	Set PageFormat from flattened data in a Handle
//
//	Caller retains ownership of the Handle.

void
LPrintSpec::SetFlatPageFormat(
	Handle	inFlatPageFormat)
{
										// Resuscitate PageFormat
	PMPageFormat	pageFormat = nil;
	OSStatus		status = ::PMUnflattenPageFormat(
									inFlatPageFormat, &pageFormat);
	ThrowIfOSStatus_(status);
		
		
	if (mPageFormat != nil) {			// Discard old PageFormat
		::PMRelease(mPageFormat);
	}			

	mPageFormat = pageFormat;
	
	mPrintRecordH.Adopt(nil);			// Clear saved PrintRecord
}


// ---------------------------------------------------------------------------
//	¥ GetPrintSettings												  [public]
// ---------------------------------------------------------------------------

PMPrintSettings
LPrintSpec::GetPrintSettings() const
{
	return mPrintSettings;
}


// ---------------------------------------------------------------------------
//	¥ GetFlatPrintSettings											  [public]
// ---------------------------------------------------------------------------
//	Return Handle with saved PrintSettings data
//
//	Handle may be nil if printing isn't in progress, and no previous
//	printing operation occurred.
//
//	Caller takes ownership of the Handle

Handle
LPrintSpec::GetFlatPrintSettings() const
{
	Handle	flatSettingsH = nil;
	
	if (mPrintSettings != nil) {
	
		// Make a new Handle from the current PrintSettings data
	
		::PMFlattenPrintSettings(mPrintSettings, &flatSettingsH);
	}

	return flatSettingsH;
}


// ---------------------------------------------------------------------------
//	¥ SetFlatPrintSettings											  [public]
// ---------------------------------------------------------------------------
//	Set PrintSettings from flattened data in a Handle
//
//	Caller retains ownership of the Handle.

void
LPrintSpec::SetFlatPrintSettings(
	Handle	inFlatPrintSettings)
{
										// Resuscitate PrintSettings
	PMPrintSettings	printSettings;
	OSStatus		status = ::PMUnflattenPrintSettings(
								inFlatPrintSettings, &printSettings);
	ThrowIfOSStatus_(status);

	if (mPrintSettings != nil) {		// Discard old PrintSettings
		::PMRelease(mPrintSettings);
	}
	
	mPrintSettings = printSettings;

	mPrintRecordH.Adopt(nil);			// Clear saved PrintRecord
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetPrintJobParams												  [public]
// ---------------------------------------------------------------------------

void
LPrintSpec::GetPrintJobParams(
	UInt32&		outFirstPage,
	UInt32&		outLastPage,
	UInt32&		outNumCopies)
{
	SignalIfNot_( IsInSession() );		// Print Session must be open

	if (mPrintSettings != nil) {
		::PMGetFirstPage(mPrintSettings, &outFirstPage);
		::PMGetLastPage (mPrintSettings, &outLastPage);
		::PMGetCopies   (mPrintSettings, &outNumCopies);
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
	SignalIfNot_( IsInSession() );		// Print Session must be open

	if (mPrintSettings != nil) {
		::PMSetFirstPage(mPrintSettings, inFirstPage, false);
		::PMSetLastPage (mPrintSettings, inLastPage, false);
		::PMSetCopies   (mPrintSettings, inNumCopies, false);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetPaperRect													  [public]
// ---------------------------------------------------------------------------
//	Get bounds of the entire sheet of Paper used for printing

void
LPrintSpec::GetPaperRect(
	Rect&	outPaperRect)
{
	SignalIfNot_( IsInSession() );		// Session should be open
	
	outPaperRect = Rect_0000;

	if (mPageFormat != nil) {
		PMRect		pmPaperRect;
		OSStatus	status =
						::PMGetAdjustedPaperRect(mPageFormat, &pmPaperRect);
		
		if (status == noErr) {
			UPrinting::PMRectToQDRect(pmPaperRect, outPaperRect);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ GetPageRect													  [public]
// ---------------------------------------------------------------------------
//	Get bounds of the printable area of the Paper used for printing

void
LPrintSpec::GetPageRect(
	Rect&	outPageRect)
{
	SignalIfNot_( IsInSession() );		// Session should be open
	
	outPageRect = Rect_0000;

	if (mPageFormat != nil) {
		PMRect		pmPageRect;
		OSStatus	status =
						::PMGetAdjustedPageRect(mPageFormat, &pmPageRect);
		
		if (status == noErr) {
			UPrinting::PMRectToQDRect(pmPageRect, outPageRect);
		}
	}
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
	
	: mPrintSpec(inPrintSpec)
{
								// Print Session must be open
	SignalIfNot_( inPrintSpec.IsInSession() );

	OSStatus	status = ::PMSessionBeginDocument(
										inPrintSpec.GetPrintSession(),
										inPrintSpec.GetPrintSettings(),
										inPrintSpec.GetPageFormat() );

	ThrowIfOSStatus_(status);
}


// ---------------------------------------------------------------------------
//	¥ ~StPrintContext						Destructor
// ---------------------------------------------------------------------------

StPrintContext::~StPrintContext()
{
	::PMSessionEndDocument(mPrintSpec.GetPrintSession());
}


// ---------------------------------------------------------------------------
//	¥ GetGrafPtr
// ---------------------------------------------------------------------------
//	Return GrafPtr being used for printing
//
//	Note: The GrafPtr is valid only while printing a page. That is, after
//	BeginPage() is called and before EndPage() is called.

GrafPtr
StPrintContext::GetGrafPtr() const
{
	GrafPtr		printerPort = nil;

#ifdef Debug_Signal
	OSStatus	status =
#endif
						::PMSessionGetGraphicsContext(
									mPrintSpec.GetPrintSession(),
									nil,
									(void**) &printerPort );

		// Signal here instead of throwing. If there's an error, it's
		// most likely programmer error (haven't called BeginPage() yet)
		// rather than a printing or memory problem.

	SignalIf_( status != noErr );

	return printerPort;
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
	OSStatus	status = ::PMSessionBeginPage(
								mPrintSpec.GetPrintSession(),
								mPrintSpec.GetPageFormat(),
								nil );

	ThrowIfOSStatus_(status);
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
	::PMSessionEndPage(mPrintSpec.GetPrintSession());
}


#pragma mark -
// ===========================================================================
//	StPrintSession Class
// ===========================================================================
//
//	Constructor begins a print session
//	Destructor ends a print session

// ---------------------------------------------------------------------------
//	¥ StPrintSession						Constructor
// ---------------------------------------------------------------------------
//	Open a Print Session
//
//	You need to open a Print Session before performing any print operations

StPrintSession::StPrintSession(
	LPrintSpec&		ioPrintSpec)

	: mPrintSpec(ioPrintSpec)
{
	PMPrintSession	session;
	
		// Check if the Print Session API exists. It won't if
		// running on Carbon 1.0.x. To support Carbon 1.0.x, you
		// should use UCarbonPrinting.cp.

	OSStatus	status = kPMNotImplemented;
	
	if (PMCreateSession != (void*) kUnresolvedCFragSymbolAddress) {

		status = ::PMCreateSession(&session);
	}

	ThrowIfOSStatus_(status);

	mPrintSpec.BeginSession(session);
	
	::PMRelease(session);
}


// ---------------------------------------------------------------------------
//	¥ ~StPrintSession						Destructor
// ---------------------------------------------------------------------------
//	Close a Print Session

StPrintSession::~StPrintSession()
{
	mPrintSpec.EndSession();
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

	Boolean	setUpOK = false;

	::PMSessionPageSetupDialog(
			ioPrintSpec.GetPrintSession(),
			ioPrintSpec.GetPageFormat(),
			&setUpOK );

	return setUpOK;
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

	Boolean		printOK = false;

	::PMSessionPrintDialog(
				ioPrintSpec.GetPrintSession(),
				ioPrintSpec.GetPrintSettings(),
		 		ioPrintSpec.GetPageFormat(),
		 		&printOK );

	return printOK;
}


PP_End_Namespace_PowerPlant
