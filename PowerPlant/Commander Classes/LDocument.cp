// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDocument.cp				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	A Document associates one or more Windows with one or more Files.
//	LDocument is an abstract class which provides a framework for
//	supporting standard document operations via AppleEvents.
//
//	Standard Document operations are:
//		Save, SaveAs, Revert, and Print
//	You can also check whether or not a Document is modified, meaning
//	that it differs from its last saved version.
//
//	Usage Note:
//		LDocument is an abstract class. The GetDescriptor() function
//		is pure virtual. Subclasses must override this function and
//		provide a definition. For example:
//
//		class MyDocument : public LDocument {
//		public:
//			virtual StringPtr	GetDescriptor(Str255 outDescriptor);
//		};
//
//		StringPtr MyDocument::GetDescriptor(Str255 outDescriptor)
//		{
//			::BlockMoveData("\pMyName", outDescriptor, 7);	// 6 + length byte
//			return outDescriptor;
//		}

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LDocument.h>
#include <LModelProperty.h>
#include <LString.h>
#include <PP_Messages.h>
#include <PP_Resources.h>
#include <TArrayIterator.h>
#include <UDesktop.h>
#include <UAppleEventsMgr.h>
#include <UStandardDialogs.h>

#include <AERegistry.h>
#include <AEObjects.h>
#include <AEPackObject.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Class Variables

TArray<LDocument*>	LDocument::sDocumentList;


// ---------------------------------------------------------------------------
//	¥ LDocument								Default Constructor		  [public]
// ---------------------------------------------------------------------------

LDocument::LDocument()
{
	mIsModified		= false;
	mIsSpecified	= false;
	mModelKind		= cDocument;

	sDocumentList.AddItem(this);
}


// ---------------------------------------------------------------------------
//	¥ LDocument								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Constructor, specifying SuperCommander

LDocument::LDocument(
	LCommander*		inSuper)

	: LCommander(inSuper)
{
	mIsModified		= false;
	mIsSpecified	= false;
	mModelKind		= cDocument;

	sDocumentList.AddItem(this);
}


// ---------------------------------------------------------------------------
//	¥ ~LDocument							Destructor				  [public]
// ---------------------------------------------------------------------------

LDocument::~LDocument()
{
	sDocumentList.Remove(this);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ObeyCommand													  [public]
// ---------------------------------------------------------------------------
//	Respond to commands

Boolean
LDocument::ObeyCommand(
	CommandT	inCommand,
	void*		ioParam)
{
	Boolean		cmdHandled = true;

	switch (inCommand) {

		case cmd_Close:
			AttemptClose(RecordAE_Yes);
			break;

		case cmd_Save:
			if (IsSpecified()) {
				SendSelfAE(kAECoreSuite, kAESave, ExecuteAE_No);
				DoSave();
				break;
			}
			// else *fall thru*, saving an unspecified document is the
			// same as "SaveAs"

		case cmd_SaveAs: {
			FSSpec	fileSpec;
			AskSaveAs(fileSpec, RecordAE_Yes);
			break;
		}

		case cmd_Revert: {
			if (AskConfirmRevert()) {
				SendSelfAE(kAEMiscStandards, kAERevert, ExecuteAE_No);
				DoRevert();
			}
			break;
		}

		case cmd_PageSetup:
			HandlePageSetup();
			break;

		case cmd_Print:
			HandlePrint();
			break;

		case cmd_PrintOne:
			HandlePrintOne();
			break;

		default:
			cmdHandled = LCommander::ObeyCommand(inCommand, ioParam);
			break;
	}

	return cmdHandled;
}


// ---------------------------------------------------------------------------
//	¥ FindCommandStatus												  [public]
// ---------------------------------------------------------------------------
//	Return whether a Command is enabled and/or marked (in a Menu)

void
LDocument::FindCommandStatus(
	CommandT	inCommand,
	Boolean&	outEnabled,
	Boolean&	outUsesMark,
	UInt16&		outMark,
	Str255		outName)
{
	switch (inCommand) {

		case cmd_Close:
		case cmd_SaveAs:
		case cmd_PageSetup:
		case cmd_Print:
		case cmd_PrintOne:
			outEnabled = true;
			break;

		case cmd_Save:
			outEnabled = IsModified() or  not IsSpecified();
			break;

		case cmd_Revert:
			outEnabled = IsModified() and IsSpecified();
			break;

		default:
			LCommander::FindCommandStatus(inCommand, outEnabled,
									outUsesMark, outMark, outName);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ Close															  [public]
// ---------------------------------------------------------------------------
//	Close a Document

void
LDocument::Close()
{
	if ((mSuperCommander == nil) || mSuperCommander->AllowSubRemoval(this)) {
		delete this;
	}
}


// ---------------------------------------------------------------------------
//	¥ IsModified													  [public]
// ---------------------------------------------------------------------------
//	Return whether a Document has been modified since it was last saved

Boolean
LDocument::IsModified()
{
	return mIsModified;
}


// ---------------------------------------------------------------------------
//	¥ SetModified													  [public]
// ---------------------------------------------------------------------------
//	Specify whether a Document has been modified since it was last saved

void
LDocument::SetModified(
	Boolean	inModified)
{
	if (inModified != mIsModified) {	// Modifed state is changing
		mIsModified = inModified;
		SetUpdateCommandStatus(true);	// "Save" command will enable/disable
	}
}


// ---------------------------------------------------------------------------
//	¥ IsSpecified													  [public]
// ---------------------------------------------------------------------------
//	Return whether a file is specified for this Document

Boolean
LDocument::IsSpecified() const
{
	return mIsSpecified;
}


// ---------------------------------------------------------------------------
//	¥ SetSpecified													  [public]
// ---------------------------------------------------------------------------
//	Set whether a Document has a file specified

void
LDocument::SetSpecified(
	Boolean	inSpecified)
{
	mIsSpecified = inSpecified;
}


// ---------------------------------------------------------------------------
//	¥ UsesFileSpec													  [public]
// ---------------------------------------------------------------------------
//	Return whether the Document has a file that uses the specified FSSpec

Boolean
LDocument::UsesFileSpec(
	const FSSpec&	/* inFileSpec */) const
{
	return false;
}


// ---------------------------------------------------------------------------
//	¥ GetPrintSpec													  [public]
// ---------------------------------------------------------------------------
//	Return the Print Specifier associated with a Document

LPrintSpec&
LDocument::GetPrintSpec()
{
	return mPrintSpec;
}


// ---------------------------------------------------------------------------
//	¥ MakeCurrent													  [public]
// ---------------------------------------------------------------------------
//	Make this Document the current one.
//
//	Override to do what makes sense for a particular kind of Document. In
//	most cases, this means selecting the main window for a Document.

void
LDocument::MakeCurrent()
{
}


// ---------------------------------------------------------------------------
//	¥ AskSaveAs														  [public]
// ---------------------------------------------------------------------------
//	Ask the user to save a Document and give it a name
//
//	Returns false if the user cancels the operation

Boolean
LDocument::AskSaveAs(
	FSSpec&		outFSSpec,
	Boolean		inRecordIt)
{
	Boolean		saveOK = false;
	Str255		defaultName;
	bool		replacing;

	if ( PP_StandardDialogs::AskSaveFile(GetDescriptor(defaultName),
										 GetFileType(),
										 outFSSpec,
										 replacing) ) {

		if (replacing && UsesFileSpec(outFSSpec)) {
									// User chose to replace the file with
									//   one of the same name. This is the
									//   same thing as a regular save.
			if (inRecordIt) {
				SendSelfAE(kAECoreSuite, kAESave, ExecuteAE_No);
			}

			DoSave();
			saveOK = true;

		} else {

			if (inRecordIt) {
				try {
					SendAESaveAs(outFSSpec, fileType_Default, ExecuteAE_No);
				}

				catch (...) { }
			}

			if (replacing) {		// Delete existing file
				ThrowIfOSErr_(::FSpDelete(&outFSSpec));
			}

			DoAESave(outFSSpec, fileType_Default);
			saveOK = true;
		}
	}

	return saveOK;
}


// ---------------------------------------------------------------------------
//	¥ GetFileType													  [public]
// ---------------------------------------------------------------------------
//	Return the type (four character code) of the file used for saving
//	the Document. Subclasses should override if they support saving files.

OSType
LDocument::GetFileType() const
{
	return fileType_Default;
}


// ---------------------------------------------------------------------------
//	¥ AttemptClose													  [public]
// ---------------------------------------------------------------------------
//	Try to close a Document.
//
//	The Document might not close if it is modified and the user cancels
//	the operation when asked whether to save the changes.

void
LDocument::AttemptClose(
	Boolean		inRecordIt)
{
	Boolean		closeIt = true;
	SInt32		saveOption = kAENo;
	FSSpec		fileSpec = {0, 0, "\p"};		// Invalid specifier
	
	if (IsModified()) {

		SInt16 answer = AskSaveChanges(SaveWhen_Closing);

		if (answer == answer_Save) {
			if (IsSpecified()) {
				DoSave();
				saveOption = kAEYes;

			} else {
				closeIt = AskSaveAs(fileSpec, RecordAE_No);
				saveOption = kAEYes;
			}

		} else if (answer == answer_Cancel) {
			closeIt = false;
		}
	}

	if (closeIt) {

		if (inRecordIt) {
			try {
				SendAEClose(saveOption, fileSpec, ExecuteAE_No);
			}

			catch (...) { }
		}

		Close();
	}
}


// ---------------------------------------------------------------------------
//	¥ AttemptQuitSelf												  [public]
// ---------------------------------------------------------------------------
//	Try to close a Document when quitting the program.
//
//	Return false if the user cancels when asked whether to save changes
//	to a modified Document.

Boolean
LDocument::AttemptQuitSelf(
	SInt32	inSaveOption)
{
	Boolean	allowQuit = true;

	if (IsModified()  &&  (inSaveOption != kAENo) ) {

		SInt16	answer = answer_Save;

		if (inSaveOption == kAEAsk) {
			UAppleEventsMgr::InteractWithUser(Throw_Yes);
			answer = AskSaveChanges(SaveWhen_Quitting);
		}

		if (answer == answer_Save) {
			if (IsSpecified()) {
				DoSave();

			} else {
				FSSpec	fileSpec;
				UAppleEventsMgr::InteractWithUser(Throw_Yes);
				allowQuit = AskSaveAs(fileSpec, RecordAE_No);
			}

		} else if (answer == answer_Cancel) {
			allowQuit = false;
		}
	}

	if (allowQuit) {
		delete this;
	}

	return allowQuit;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AskConfirmRevert												  [public]
// ---------------------------------------------------------------------------
//	Ask user to confirm discarding changes and reverting to the last
//	saved version of the Document

bool
LDocument::AskConfirmRevert()
{
	Str255	docName;

	return PP_StandardDialogs::AskConfirmRevert(GetDescriptor(docName));
}


// ---------------------------------------------------------------------------
//	¥ AskSaveChanges												  [public]
// ---------------------------------------------------------------------------
//	Ask user whether to save changes before closing the Document or
//	quitting the Application

SInt16
LDocument::AskSaveChanges(
	bool	inQuitting)
{
	MakeCurrent();

	Str255	docName;
	LStr255	appName(STRx_Standards, str_ProgramName);

	return PP_StandardDialogs::AskSaveChanges(GetDescriptor(docName),
										   appName, inQuitting);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DoAEClose														  [public]
// ---------------------------------------------------------------------------
//	Close a Document in response to a "close" AppleEvent

void
LDocument::DoAEClose(
	const AppleEvent&	inCloseAE)
{
	OSErr		err;
	DescType	theType;
	Size		theSize;
	FSSpec		fileSpec;

		// Check for optional "file" parameter

	err = ::AEGetParamPtr(&inCloseAE, keyAEFile, typeFSS, &theType,
						&fileSpec, sizeof(FSSpec), &theSize);

	bool	hasFileParameter = (err == noErr);

		// Check for optional "saveOption" parameter
		//
		// Default value is "ask", unless there is a file, in which
		// case the default is "yes".

	SInt32		saveOption = kAEAsk;
	if (hasFileParameter) {
		saveOption = kAEYes;
	}

	err = ::AEGetParamPtr(&inCloseAE, keyAESaveOptions,
				typeEnumeration, &theType, &saveOption,
				sizeof(SInt32), &theSize);

		// Based on the options (and possible user responses), we
		// will save and/or close (or do nothing).

		// The default values below apply to the case where the
		// save option is kAENo, or kAEAsk and the user chooses
		// not to save.

	bool	saveIt   = false;
	bool	closeIt  = true;

	if (saveOption == kAEAsk) {

			// Normally, we only ask the user whether to save changes
			// if the document is modified. However, there is a special
			// case if the user specifies a file. For example,
			//
			//		close document "foo" saving in file "bar" saving ask
			//
			// This is similar to a "save as", so we ask about saving even
			// if "foo" is not modified.

		SInt16	saveAnswer = answer_DontSave;

		if (hasFileParameter or IsModified()) {
			UAppleEventsMgr::InteractWithUser(Throw_Yes);
			saveAnswer = AskSaveChanges(SaveWhen_Closing);
		}

		if (saveAnswer == answer_Save) {
			saveIt = true;

		} else if (saveAnswer == answer_Cancel) {
			closeIt = false;				// Abort the close
		}

	} else if (saveOption == kAEYes) {
		saveIt = true;
	}

	if (saveIt) {

			// Figure out which file to use to save the document

		if (hasFileParameter) {			// First choice:
										//   File in the Event
			DoAESave(fileSpec, fileType_Default);

		} else if (IsSpecified()) {		// Second choice:
			DoSave();					//   Save to existing file

		} else {						// Third choice:
										//   Ask the user to specify a file.
										//   Canceling aborts the close.
			UAppleEventsMgr::InteractWithUser(Throw_Yes);
			closeIt = AskSaveAs(fileSpec, RecordAE_No);
		}
	}

	if (closeIt) {						// Finally, close the document
		Close();
		
	} else {							// User canceling the close means
		ThrowOSErr_(userCanceledErr);	//   the AppleEvent failed
	}
}


// ---------------------------------------------------------------------------
//	¥ HandleAESave													  [public]
// ---------------------------------------------------------------------------
//	Respond to "Save" AppleEvent

void
LDocument::HandleAESave(
	const AppleEvent&	inSaveAE)
{
	OSErr		err;
	DescType	theType;
	Size		theSize;
	FSSpec		fileSpec;

		// Check for optional "file" parameter

	err = ::AEGetParamPtr(&inSaveAE, keyAEFile, typeFSS, &theType,
						&fileSpec, sizeof(FSSpec), &theSize);

	bool	hasFileParameter = (err == noErr);

		// Check for optional "file type" parameter

	OSType		fileType = fileType_Default;
	err = ::AEGetParamPtr(&inSaveAE, keyAEFileType, typeType,
						&theType, &fileType, sizeof(OSType), &theSize);

	if (hasFileParameter) {
		DoAESave(fileSpec, fileType);	// Save using file from event

	} else if (mIsSpecified) {
		DoSave();						// Save using existing file

	} else {							// If user interaction is allowed,
										//   ask user to specify a file
		UAppleEventsMgr::InteractWithUser(Throw_Yes);
		
		if (not AskSaveAs(fileSpec, RecordAE_No)) {
										// User canceling the save means
										//   the AppleEvent failed
			ThrowOSErr_(userCanceledErr);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DoAESave														  [public]
// ---------------------------------------------------------------------------
//	Save a Document in a particular file
//
//	This is a "Save As" operation
//
//	Subclasses should override this function to save a file and
//	should set mIsSpecified to true

void
LDocument::DoAESave(
	FSSpec&	/* inFileSpec */,
	OSType	/* inFileType */)
{
}


// ---------------------------------------------------------------------------
//	¥ DoSave														  [public]
// ---------------------------------------------------------------------------
//	Save a Document, which must already be "specified"
//
//	Subclasses should override to save a Document to an existing file

void
LDocument::DoSave()
{
}


// ---------------------------------------------------------------------------
//	¥ DoRevert														  [public]
// ---------------------------------------------------------------------------
//	Revert a Document to its last saved version

void
LDocument::DoRevert()
{
}


// ---------------------------------------------------------------------------
//	¥ HandlePageSetup												  [public]
// ---------------------------------------------------------------------------
//	Handle the "page setup" command

void
LDocument::HandlePageSetup()
{
	StPrintSession			session(mPrintSpec);
	StDesktopDeactivator	deactivator;

	UPrinting::AskPageSetup(mPrintSpec);
}


// ---------------------------------------------------------------------------
//	¥ HandlePrint													  [public]
// ---------------------------------------------------------------------------
//	Handle the "print" command

void
LDocument::HandlePrint()
{
	StPrintSession	session(mPrintSpec);

	UDesktop::Deactivate();
	bool	printIt = UPrinting::AskPrintJob(mPrintSpec);
	UDesktop::Activate();

	if (printIt) {
		SendSelfAE(kCoreEventClass, kAEPrint, ExecuteAE_No);
		DoPrint();
	}
}


// ---------------------------------------------------------------------------
//	¥ HandlePrintOne												  [public]
// ---------------------------------------------------------------------------
//	Handle the "print one" command

void
LDocument::HandlePrintOne()
{
	StPrintSession	session(mPrintSpec);

	mPrintSpec.SetPrintJobParams(1, UPrinting::max_Pages, 1);

	SendSelfAE(kCoreEventClass, kAEPrint, ExecuteAE_No);
	DoPrint();
}


// ---------------------------------------------------------------------------
//	¥ DoPrint														  [public]
// ---------------------------------------------------------------------------
//	Print a Document

void
LDocument::DoPrint()
{
}

#pragma mark -
// ===========================================================================
// ¥ AppleEvent Object Model Support		 AppleEvent Object Model Support ¥
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ MakeSelfSpecifier												  [public]
// ---------------------------------------------------------------------------

void
LDocument::MakeSelfSpecifier(
	AEDesc	&inSuperSpecifier,
	AEDesc	&outSelfSpecifier) const
{
	Str255	docName;
	GetDescriptor(docName);

	StAEDescriptor	keyData;
	OSErr	err = ::AECreateDesc(typeChar, docName + 1, docName[0],
						&keyData.mDesc);
	ThrowIfOSErr_(err);

	err = ::CreateObjSpecifier(cDocument, &inSuperSpecifier, formName,
								&keyData.mDesc, false, &outSelfSpecifier);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ HandleAppleEvent												  [public]
// ---------------------------------------------------------------------------

void
LDocument::HandleAppleEvent(
	const AppleEvent	&inAppleEvent,
	AppleEvent			&outAEReply,
	AEDesc				&outResult,
	long				inAENumber)
{
	switch (inAENumber) {

		case ae_Close:
			DoAEClose(inAppleEvent);
			break;

		case ae_Save: {
			HandleAESave(inAppleEvent);
			break;
		}

		case ae_Revert:
			DoRevert();
			break;

		case ae_Print: {
				// Print one copy of all pages of document
			StPrintSession	session(mPrintSpec);
			mPrintSpec.SetPrintJobParams(1, UPrinting::max_Pages, 1);
			DoPrint();
			break;
		}

		default:
			LModelObject::HandleAppleEvent(inAppleEvent, outAEReply,
											outResult, inAENumber);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetAEProperty													  [public]
// ---------------------------------------------------------------------------

void
LDocument::GetAEProperty(
	DescType		inProperty,
	const AEDesc	&inRequestedType,
	AEDesc			&outPropertyDesc) const
{
	OSErr	err;

	switch (inProperty) {

		case pName: {
			Str255	docName;
			GetDescriptor(docName);
			err = AECreateDesc(typeChar, docName + 1, docName[0],
								&outPropertyDesc);
			break;
		}

		case pIsModified: {			// Has Document changed since last save?
			Boolean	dirty = ((LDocument *) this)->IsModified();
			err = AECreateDesc(typeBoolean, (Ptr) &dirty,
								sizeof(Boolean), &outPropertyDesc);
			break;
		}

		default:
			LModelObject::GetAEProperty(inProperty, inRequestedType,
											outPropertyDesc);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ AEPropertyExists
// ---------------------------------------------------------------------------

bool
LDocument::AEPropertyExists(
	DescType	inProperty) const
{
	bool	exists = false;

	switch (inProperty) {

		case pName:
		case pIsModified:
			exists = true;
			break;

		default:
			exists = LModelObject::AEPropertyExists(inProperty);
			break;
	}

	return exists;
}


// ---------------------------------------------------------------------------
//	¥ SendAESaveAs													  [public]
// ---------------------------------------------------------------------------

void
LDocument::SendAESaveAs(
	FSSpec&		inFileSpec,
	OSType		inFileType,
	Boolean		inExecute)
{
	AppleEvent	theAppleEvent;
	UAppleEventsMgr::MakeAppleEvent(kAECoreSuite, kAESave, theAppleEvent);

	StAEDescriptor	docSpec;
	MakeSpecifier(docSpec.mDesc);
	OSErr err = ::AEPutParamDesc(&theAppleEvent, keyDirectObject,
								&docSpec.mDesc);
	ThrowIfOSErr_(err);

	err = ::AEPutParamPtr(&theAppleEvent, keyAEFile, typeFSS, &inFileSpec,
								sizeof(FSSpec));
	ThrowIfOSErr_(err);

	if (inFileType != fileType_Default) {
		err = ::AEPutParamPtr(&theAppleEvent, keyAEFileType, typeType,
									&inFileType, sizeof(OSType));
		ThrowIfOSErr_(err);
	}

	UAppleEventsMgr::SendAppleEvent(theAppleEvent, inExecute);
}


// ---------------------------------------------------------------------------
//	¥ SendAEClose													  [public]
// ---------------------------------------------------------------------------

void
LDocument::SendAEClose(
	SInt32		inSaveOption,
	FSSpec&		inFileSpec,
	Boolean		inExecute)
{
										// Create a "clase" AppleEvent
	AppleEvent	theAppleEvent;
	UAppleEventsMgr::MakeAppleEvent(kAECoreSuite, kAEClose, theAppleEvent);

										// Document is the direct object
	StAEDescriptor	docSpec;
	MakeSpecifier(docSpec.mDesc);
	OSErr err = ::AEPutParamDesc(&theAppleEvent, keyDirectObject,
								&docSpec.mDesc);
	ThrowIfOSErr_(err);

										// Store save option parameter
	err = ::AEPutParamPtr(&theAppleEvent, keyAESaveOptions, typeEnumeration,
								&inSaveOption, sizeof(SInt32));
	ThrowIfOSErr_(err);

										// If we are saving to a valid file,
										//   store the file specifier
	if ((inSaveOption == kAEYes) && (inFileSpec.name[0] != 0)) {
		err = ::AEPutParamPtr(&theAppleEvent, keyAEFile, typeFSS, &inFileSpec,
								sizeof(FSSpec));
		ThrowIfOSErr_(err);
	}

	UAppleEventsMgr::SendAppleEvent(theAppleEvent, inExecute);
}


// ---------------------------------------------------------------------------
//	¥ FindNamedDocument										 [static] [public]
// ---------------------------------------------------------------------------
//	Return the Document with the specified name

LDocument*
LDocument::FindNamedDocument(
	ConstStringPtr	inName)
{
	TArrayIterator<LDocument*> iterator(sDocumentList);
	LDocument*	theDoc = nil;
	while (iterator.Next(theDoc)) {
		Str255	docName;
		theDoc->GetDescriptor(docName);
		if (::IdenticalString(inName, docName, nil) == 0) {
			break;
		}
		theDoc = nil;
	}

	return theDoc;
}


// ---------------------------------------------------------------------------
//	¥ FindByFileSpec										 [static] [public]
// ---------------------------------------------------------------------------
//	Return the Document that uses the specified FSSpec

LDocument*
LDocument::FindByFileSpec(
	const FSSpec&	inFileSpec)
{
	TArrayIterator<LDocument*> iterator(sDocumentList);
	LDocument*	theDoc = nil;
	while (iterator.Next(theDoc)) {
		if (theDoc->UsesFileSpec(inFileSpec)) {
			break;
		}
		theDoc = nil;
	}

	return theDoc;
}


PP_End_Namespace_PowerPlant
