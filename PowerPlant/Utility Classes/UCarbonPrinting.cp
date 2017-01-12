// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UCarbonPrinting.cp			PowerPlant 2.2.2	©1999-2005 Metrowerks Inc.
// ===========================================================================
//
//	Implementation of UPrinting module using Carbon non-Session Print APIs
//
//	This implementation works with all versions of CarbonLib

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UPrinting.h>

#if PP_Target_Classic

		// You must use UClassicPrinting for Classic targets.
		// Remove this file from Classic targets.

	#error "Classic target does not support UCarbonPrinting"

#elif PM_USE_SESSION_APIS

		// You must turn off the Toolbox flag PM_USE_SESSION_APIS
		// to use UCarbonPrinting.
		
	#error "Turn off the PM_USE_SESSION_APIS flag to use UCarbonPrinting"

#endif


#if TARGET_API_MAC_CARBON && !PM_USE_SESSION_APIS

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
//		With the non-session Print API, PMPageFormat and PMPrintSettings
//		references are valid only when within a PMBegin/PMEnd block. This
//		differs from the session API, where such references are valid after
//		the session is closed.
//
//		PMPageFormat and PMPrintSettings are flattened at the end of each
//		session, and unflattened at the beginning of the next session. For
//		compatibility, a PrintRecord is also created at the end of each
//		session. You can get a copy of any of the 3 saved data Handles by
//		calling the appropriate "get" function at any time. The Handles
//		may be nil if no print operation has occurred.
//
//		When restoring specs, you should use either PageFormat/PrintSettings
//		or PrintRecords, but not both. If you do restore both, the last one
//		"set" will be used. For example, if you call SetPrintRecord() and
//		then call SetPageFormat(), the PrintRecord will be deleted and the
//		PageFormat will be used.

// ---------------------------------------------------------------------------
//	Local constants

const PMPrintSession	printSession_Open	= (PMPrintSession) (-1);
const PMPrintSession	printSession_Closed	= nil;


// ---------------------------------------------------------------------------
//	¥ LPrintSpec							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LPrintSpec::LPrintSpec()
{
	mPageFormat		= nil;
	mPrintSettings	= nil;
	mPrintSession	= printSession_Closed;
}


// ---------------------------------------------------------------------------
//	¥ ~LPrintSpec							Destructor				  [public]
// ---------------------------------------------------------------------------

LPrintSpec::~LPrintSpec()
{
}


// ---------------------------------------------------------------------------
//	¥ BeginSession
// ---------------------------------------------------------------------------
//	Start a print session
//
//	When a session is open, LPrintSpec members have the following state:
//
//		mPrintSession		printSession_Open
//		mPageFormat			Validated Reference
//		mPrintSettings		Validated Reference
//		mFlatPageFormat		nil
//		mFlatPrintSettings	nil
//		mPrintRecordH		nil

void
LPrintSpec::BeginSession(
	PMPrintSession	/* inSession */)
{
	SignalIf_( IsInSession() );		// Session already open

	OSStatus	status;
	
		// The goal here is to obtain valid PageFormat and PrintSettings
		// objects. There are 3 possibilities, which we consider in order:
		//
		//		(1) Flattened version exists. Unflatten and validate.
		//		(2) Convert a Classic PrintRecord
		//		(3) Create a new one with default values
		//
		// Note that we use a Classic PrintRecord only if both the PageFormat
		// and PrintSettings are not present.
	
	if (mFlatPageFormat.IsValid()) {
									// Restore PageFormat from flattened data
		status = ::PMUnflattenPageFormat(mFlatPageFormat, &mPageFormat);
	}
	
	mFlatPageFormat.Adopt(nil);		// Clear flattened data. PageFormat may
									//   change and we don't want stale data.
	
	if (mFlatPrintSettings.IsValid()) {
									// Restore PrintSettings from flattened
									//   data
		status = ::PMUnflattenPrintSettings(mFlatPrintSettings, &mPrintSettings);
	}
	
	mFlatPrintSettings.Adopt(nil);	// Clear flattened data
	
	if ( (mPageFormat == nil) &&
		 (mPrintSettings == nil) &&
		 mPrintRecordH.IsValid() ) {
		 							// Both PageFormat and PrintSettings are
		 							//   undefined. Create them from a saved
		 							//   Classic PrintRecord.
		status = ::PMConvertOldPrintRecord(
									mPrintRecordH,
									&mPrintSettings,
									&mPageFormat );
	}
	
	mPrintRecordH.Adopt(nil);		// Clear saved PrintRecord
	
	if (mPageFormat == nil) {		// After checking all saved data, we
									//   still don't have a PageFormat.
									//   Create a new one with default
									//   values.
		status = ::PMNewPageFormat(&mPageFormat);
		
		SignalIf_( status != noErr );
		
		status = ::PMDefaultPageFormat(mPageFormat);
		
		SignalIf_( status != noErr );
	}
	
	if (mPrintSettings == nil) {	// Similarly, create a new PrintSettings
									//   with default values
		status = ::PMNewPrintSettings(&mPrintSettings);
		status = ::PMDefaultPrintSettings(mPrintSettings);
	}
	
	mPrintSession = printSession_Open;
}


// ---------------------------------------------------------------------------
//	¥ EndSession
// ---------------------------------------------------------------------------
//	End a print session
//
//	After a session ends, the LPrintSpec members have the following state:
//
//		mPrintSession		printSession_Closed
//		mPageFormat			nil
//		mPrintSettings		nil
//		mFlatPageFormat		Handle with saved PageFormat data
//		mFlatPrintSettings	Handle with saved PrintSettings data
//		mPrintRecordH		Handle with converted Format and Settings data

void
LPrintSpec::EndSession()
{
	SignalIfNot_( IsInSession() );		// Session should be open
	
	mPrintSession = printSession_Closed;
	
		// When a session ends, mPageFormat and mPrintSettings are no
		// longer valid. Since we want to restore the state the next
		// time we print, we need to save the state as flattened data.
		// For compatibility, we also save an old-style PrintRecord.

	OSStatus	status;

	if ( (mPrintSettings != nil)  &&	// Save PrintRecord using current
		 (mPageFormat != nil) ) {		//   PageFormat and PrintSettings
		 
		 Handle		printRecH = nil;

		status = ::PMMakeOldPrintRecord(mPrintSettings, mPageFormat, &printRecH);
		
		mPrintRecordH.Adopt(printRecH);
	}

	Handle	flatFormatH   = nil;		// Save flattened PageFormat

	if (mPageFormat != nil) {
		::PMFlattenPageFormat(mPageFormat, &flatFormatH);
		::PMDisposePageFormat(mPageFormat);
		mPageFormat = nil;
	}

	mFlatPageFormat.Adopt(flatFormatH);

	Handle	flatSettingsH = nil;		// Save flattened PrintSettings

	if (mPrintSettings != nil) {
		::PMFlattenPrintSettings(mPrintSettings, &flatSettingsH);
		::PMDisposePrintSettings(mPrintSettings);
		mPrintSettings = nil;
	}

	mFlatPrintSettings.Adopt(flatSettingsH);
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
	OSStatus	status = kPMNoError;
	
	if (IsInSession()) {
		status = ::PMError();
	}
	
	return status;
}


// ---------------------------------------------------------------------------
//	¥ GetPrintRecord												  [public]
// ---------------------------------------------------------------------------
//	Return classic Print Record Handle
//
//	Handle will be nil if printing isn't in progress, and no previous
//	printing operation occurred.
//
//	Caller takes ownership of the Handle

Handle
LPrintSpec::GetPrintRecord() const
{
	Handle	printRecH = mPrintRecordH.GetCopy();
	
	if ( (printRecH == nil)  &&
		 (mPrintSettings != nil)  &&
		 (mPageFormat != nil) ) {

		ThrowIfOSStatus_( ::PMMakeOldPrintRecord(mPrintSettings, mPageFormat,
													&printRecH) );
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
		mPrintRecordH.CopyAndAdopt(inPrintRecordH);
		
			// Clear flattened PageFormat and PrintSettings. We will create
			// that data from the PrintRecord.
			
		mFlatPageFormat.Adopt(nil);
		mFlatPrintSettings.Adopt(nil);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetPageFormat													  [public]
// ---------------------------------------------------------------------------
//	Return PMPageFormat reference, which will be nil if a session is
//	not open

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
	Handle	flatFormatH   = mFlatPageFormat.GetCopy();
	
	if ( (flatFormatH == nil) && (mPageFormat != nil) ) {
	
		// No Flattened Handle exists, so we make a new Handle from
		// the current PageFormat data
	
		::PMFlattenPageFormat(mPageFormat, &flatFormatH);
	}
		
	return flatFormatH;
}


// ---------------------------------------------------------------------------
//	¥ SetFlatPageFormat												  [public]
// ---------------------------------------------------------------------------
//	Set the PageFormat flattened data Handle
//
//	This flattened Handle will be used to create the PMPageFormat information
//	the next time a session is started. Do not call this function while a
//	session is open.
//
//	Caller retains ownership of the Handle.

void
LPrintSpec::SetFlatPageFormat(
	Handle	inFlatPageFormat)
{
	if (IsInSession()) {
		SignalStringLiteral_("Can't set PageFormat data when session is open");
		
	} else {
		mFlatPageFormat.CopyAndAdopt(inFlatPageFormat);
		
		mPrintRecordH.Adopt(nil);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetPrintSettings												  [public]
// ---------------------------------------------------------------------------
//	Return PMPrintSettings reference, which will be nil if a session is
//	not open

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
	Handle	flatSettingsH = mFlatPrintSettings.GetCopy();
	
	if ( (flatSettingsH == nil) && (mPrintSettings != nil) ) {
	
		// No Flattened Handle exists, so we make a new Handle from
		// the current PrintSettings data
	
		::PMFlattenPrintSettings(mPrintSettings, &flatSettingsH);
	}

	return flatSettingsH;
}


// ---------------------------------------------------------------------------
//	¥ SetFlatPrintSettings											  [public]
// ---------------------------------------------------------------------------
//	Set the PrintSettings flattened data Handle
//
//	This flattened Handle will be used to create the PMPrintSettings
//	information the next time a session is started. Do not call this function
//	while a session is open.
//
//	Caller retains ownership of the Handle.

void
LPrintSpec::SetFlatPrintSettings(
	Handle	inFlatPrintSettings)
{
	if (IsInSession()) {
		SignalStringLiteral_("Can't set PrintSettings data when session is open");
		
	} else {
		mFlatPrintSettings.CopyAndAdopt(inFlatPrintSettings);
		
		mPrintRecordH.Adopt(nil);
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
{
								// Print Session must be open
	SignalIfNot_( inPrintSpec.IsInSession() );

	OSStatus	status = ::PMBeginDocument( inPrintSpec.GetPrintSettings(),
											inPrintSpec.GetPageFormat(),
					  						&mPrintContext);

	ThrowIfOSStatus_(status);
}


// ---------------------------------------------------------------------------
//	¥ ~StPrintContext						Destructor
// ---------------------------------------------------------------------------

StPrintContext::~StPrintContext()
{
	::PMEndDocument(mPrintContext);
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

	OSStatus	status = ::PMGetGrafPtr(mPrintContext, &printerPort);

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
	OSStatus	status = ::PMBeginPage(mPrintContext, nil);

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
	::PMEndPage(mPrintContext);
}


#pragma mark -
// ===========================================================================
//	StPrintSession Class
// ===========================================================================
//
//	Constructor begins a print session
//	Destructor ends a print session


StPrintSession::StPrintSession(
	LPrintSpec&		ioPrintSpec)

	: mPrintSpec(ioPrintSpec)
{
	OSStatus	status = ::PMBegin();

	ThrowIfOSStatus_(status);

	mPrintSpec.BeginSession(nil);
}


StPrintSession::~StPrintSession()
{
	mPrintSpec.EndSession();

	::PMEnd();
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
	SignalIfNot_( ioPrintSpec.IsInSession() );

	Boolean	setUpOK = false;

	::PMPageSetupDialog(ioPrintSpec.GetPageFormat(), &setUpOK);

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
	SignalIfNot_( ioPrintSpec.IsInSession() );

	Boolean		printOK = false;

	::PMPrintDialog( ioPrintSpec.GetPrintSettings(),
					 ioPrintSpec.GetPageFormat(),
					 &printOK);

	return printOK;
}


PP_End_Namespace_PowerPlant

#endif	// TARGET_API_MAC_CARBON && !PM_USE_SESSION_APIS

