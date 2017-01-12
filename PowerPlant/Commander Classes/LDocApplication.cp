// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDocApplication.cp			PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LDocApplication.h>
#include <LDocument.h>
#include <PP_Messages.h>
#include <UAppleEventsMgr.h>
#include <UDesktop.h>
#include <UPrinting.h>

#include <AEObjects.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LDocApplication						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LDocApplication::LDocApplication()
{
}


// ---------------------------------------------------------------------------
//	¥ ~LDocApplication						Destructor				  [public]
// ---------------------------------------------------------------------------

LDocApplication::~LDocApplication()
{
}


// ---------------------------------------------------------------------------
//	¥ ObeyCommand													  [public]
// ---------------------------------------------------------------------------
//	Respond to commands

Boolean
LDocApplication::ObeyCommand(
	CommandT	inCommand,
	void*		ioParam)
{
	Boolean		cmdHandled = true;

	switch (inCommand) {

		case cmd_New:
			SendAECreateDocument();
			break;

		case cmd_Open:
			ChooseDocument();
			break;

		case cmd_PageSetup:
			SetupPage();
			break;

		default:
			cmdHandled = LApplication::ObeyCommand(inCommand, ioParam);
			break;
	}

	return cmdHandled;
}


// ---------------------------------------------------------------------------
//	¥ FindCommandStatus												  [public]
// ---------------------------------------------------------------------------
//	Pass back the status of a Command

void
LDocApplication::FindCommandStatus(
	CommandT	inCommand,
	Boolean&	outEnabled,
	Boolean&	outUsesMark,
	UInt16&		outMark,
	Str255		outName)
{
	switch (inCommand) {

		case cmd_New:
		case cmd_Open:
		case cmd_PageSetup:
			outEnabled = true;
			break;

		default:
			LApplication::FindCommandStatus(inCommand, outEnabled,
									outUsesMark, outMark, outName);
			break;
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SendAEOpenDoc													  [public]
// ---------------------------------------------------------------------------
//	Self-send an AppleEvent to open a document (so that AppleScript can
//	record the action)

void
LDocApplication::SendAEOpenDoc(
	FSSpec&		inFileSpec)
{
	try {							// Create an "open document" AppleEvent
									//   using the specified file
		AppleEvent	openEvent;
		UAppleEventsMgr::MakeAppleEvent(kCoreEventClass, kAEOpen, openEvent);

		OSErr err = ::AEPutParamPtr(&openEvent, keyDirectObject, typeFSS,
								&inFileSpec, sizeof(FSSpec));
		ThrowIfOSErr_(err);
									// Send event so it's recorded,
									//   but don't execute it
		UAppleEventsMgr::SendAppleEvent(openEvent, ExecuteAE_No);
	}

	catch (...) { }					// If AppleEvent fails, we still want
									//   to open the document
	OpenDocument(&inFileSpec);
}


// ---------------------------------------------------------------------------
//	¥ SendAEOpenDocList												  [public]
// ---------------------------------------------------------------------------
//	Self-send an AppleEvent to open a list of documents (so that AppleScript
//	can record the action)

void
LDocApplication::SendAEOpenDocList(
	const AEDescList&		inFileList)
{
	try {							// Create an "open document" AppleEvent
									//   with the specified file list
		AppleEvent	openEvent;
		UAppleEventsMgr::MakeAppleEvent(kCoreEventClass, kAEOpen, openEvent);

		OSErr err = ::AEPutParamDesc(&openEvent, keyDirectObject, &inFileList);
		ThrowIfOSErr_(err);
									// Send event so it's recorded,
									//   but don't execute it
		UAppleEventsMgr::SendAppleEvent(openEvent, ExecuteAE_No);
	}

	catch (...) { }					// If AppleEvent fails, we still want
									//   to open the document
	OpenOrPrintDocList(inFileList, ae_OpenDoc);
}


// ---------------------------------------------------------------------------
//	¥ SendAECreateDocument											  [public]
// ---------------------------------------------------------------------------
//	Self-send an AppleEvent to create a new Document (so that AppleScript can
//	record the action)

void
LDocApplication::SendAECreateDocument()
{
	try {							// Create a "make new document" AppleEvent
									//   using the specified file
		AppleEvent		createDocEvent;
		UAppleEventsMgr::MakeAppleEvent(kAECoreSuite, kAECreateElement,
											createDocEvent);

		DescType	docType = cDocument;
		OSErr err = ::AEPutParamPtr(&createDocEvent, keyAEObjectClass,
							typeType, &docType, sizeof(DescType));
		ThrowIfOSErr_(err);
									// Send event so it's recorded,
									//   but don't execute it
		UAppleEventsMgr::SendAppleEvent(createDocEvent, ExecuteAE_No);
	}

	catch (...) { }					// If AppleEvent fails, we still want
									//   to make the new document
	MakeNewDocument();
}


// ---------------------------------------------------------------------------
//	¥ DoAEOpenOrPrintDoc											  [public]
// ---------------------------------------------------------------------------
//	Respond to an AppleEvent to open or print a Document

void
LDocApplication::DoAEOpenOrPrintDoc(
	const AppleEvent&	inAppleEvent,
	AppleEvent&			/* outAEReply */,
	SInt32				inAENumber)
{
	StAEDescriptor	docList;
	OSErr		err = ::AEGetParamDesc(&inAppleEvent, keyDirectObject,
							typeAEList, docList);
	ThrowIfOSErr_(err);

	OpenOrPrintDocList(docList, inAENumber);
}


// ---------------------------------------------------------------------------
//	¥ OpenOrPrintDocList											  [public]
// ---------------------------------------------------------------------------
//	Open or print the documents specified in a AE descriptor list

void
LDocApplication::OpenOrPrintDocList(
	const AEDescList&	inDocList,
	SInt32				inAENumber)
{
	SInt32		numDocs;
	OSErr 		err = ::AECountItems(&inDocList, &numDocs);
	ThrowIfOSErr_(err);

		// Loop through all items in the list
			// Extract descriptor for the document
			// Coerce descriptor data into a FSSpec
			// Tell Program object to open or print document

	for (SInt32 i = 1; i <= numDocs; i++) {
		AEKeyword	theKey;
		DescType	theType;
		FSSpec		theFileSpec;
		Size		theSize;
		err = ::AEGetNthPtr(&inDocList, i, typeFSS, &theKey, &theType,
							(Ptr) &theFileSpec, sizeof(FSSpec), &theSize);
		ThrowIfOSErr_(err);

		if (inAENumber == ae_OpenDoc) {
			OpenDocument(&theFileSpec);
		} else {
			PrintDocument(&theFileSpec);
		}
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ OpenDocument													  [public]
// ---------------------------------------------------------------------------
//	Open a Document specified by an FSSpec

void
LDocApplication::OpenDocument(
	FSSpec*	/* inMacFSSpec */)
{
	// Subclasses should override.
	// Note that the file may already be open. One way to handle this
	// case is to make it the current document. For example:
	//
	//	LDocument*	theDoc = LDocument::FindByFileSpec(*inMacFSSpec);
	//
	//	if (theDoc != nil) {
	//		theDoc->MakeCurrent();
	//	} else {
	//		theDoc = new CMyDocument(this, inMacFSSpec);
	//	}
}


// ---------------------------------------------------------------------------
//	¥ PrintDocument													  [public]
// ---------------------------------------------------------------------------
//	Print a Document specified by an FSSpec

void
LDocApplication::PrintDocument(
	FSSpec*	/* inMacFSSpec */)
{
}		// Subclasses should override


// ---------------------------------------------------------------------------
//	¥ MakeNewDocument												  [public]
// ---------------------------------------------------------------------------
//	Create a new Document and pass back an AppleEvent Model object
//	representing that Document

LModelObject*
LDocApplication::MakeNewDocument()
{
	return nil;		// Subclasses should override
}


// ---------------------------------------------------------------------------
//	¥ ChooseDocument												  [public]
// ---------------------------------------------------------------------------
//	Allow the user to pick a Document (usually for opening)

void
LDocApplication::ChooseDocument()
{
}		// Subclasses should override


// ---------------------------------------------------------------------------
//	¥ SetupPage														  [public]
// ---------------------------------------------------------------------------
//	Handle the Page Setup command

void
LDocApplication::SetupPage()
{
	StPrintSession			session(UPrinting::GetAppPrintSpec());
	StDesktopDeactivator	deactivator;

	UPrinting::AskPageSetup(UPrinting::GetAppPrintSpec());
}

#pragma mark -
// ===========================================================================
//	AppleEvent Handlers
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ HandleAppleEvent												  [public]
// ---------------------------------------------------------------------------
//	Respond to an AppleEvent

void
LDocApplication::HandleAppleEvent(
	const AppleEvent&	inAppleEvent,
	AppleEvent&			outAEReply,
	AEDesc&				outResult,
	long				inAENumber)
{
	switch (inAENumber) {

		case ae_OpenDoc:
		case ae_PrintDoc:
			DoAEOpenOrPrintDoc(inAppleEvent, outAEReply, inAENumber);
			break;

		default:
			LApplication::HandleAppleEvent(inAppleEvent, outAEReply,
								outResult, inAENumber);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ HandleCreateElementEvent										  [public]
// ---------------------------------------------------------------------------
//	Respond to an AppleEvent to create a new item

LModelObject*
LDocApplication::HandleCreateElementEvent(
	DescType			inElemClass,
	DescType			inInsertPosition,
	LModelObject*		inTargetObject,
	const AppleEvent&	inAppleEvent,
	AppleEvent&			outAEReply)
{
	switch (inElemClass) {

		case cDocument:
		case cWindow:
			return MakeNewDocument();
			break;

		default:
			return LApplication::HandleCreateElementEvent(inElemClass,
						inInsertPosition, inTargetObject, inAppleEvent,
						outAEReply);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ CountSubModels												  [public]
// ---------------------------------------------------------------------------
//	Return the number of AppleEvent submodels of a particular type

SInt32
LDocApplication::CountSubModels(
	DescType	inModelID) const
{
	SInt32	count = 0;

	switch (inModelID) {

		case cDocument:
			count = (SInt32) LDocument::GetDocumentList().GetCount();
			break;

		default:
			count = LApplication::CountSubModels(inModelID);
			break;
	}

	return count;
}


// ---------------------------------------------------------------------------
//	¥ GetSubModelByPosition											  [public]
// ---------------------------------------------------------------------------
//	Pass back an AppleEvent token representing a AppleEvent submodel
//	identified by its numerical position

void
LDocApplication::GetSubModelByPosition(
	DescType		inModelID,
	SInt32			inPosition,
	AEDesc&			outToken) const
{
	switch (inModelID) {

		case cDocument:
			ThrowOSErr_(errAENoSuchObject);
			break;

		default:
			LApplication::GetSubModelByPosition(inModelID, inPosition,
													outToken);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetSubModelByName												  [public]
// ---------------------------------------------------------------------------
//	Pass back an AppleEvent token representing a AppleEvent submodel
//	identified by its name

void
LDocApplication::GetSubModelByName(
	DescType		inModelID,
	Str255			inName,
	AEDesc&			outToken) const
{
	switch (inModelID) {

		case cDocument: {
			LDocument	*theDoc = LDocument::FindNamedDocument(inName);
			if (theDoc == nil) {
				ThrowOSErr_(errAENoSuchObject);
			}
			PutInToken(theDoc, outToken);
			break;
		}

		default:
			LApplication::GetSubModelByName(inModelID, inName, outToken);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetPositionOfSubModel											  [public]
// ---------------------------------------------------------------------------
//	Return the position (1 = first) of a SubModel within an Application

SInt32
LDocApplication::GetPositionOfSubModel(
	DescType			inModelID,
	const LModelObject*	inSubModel) const
{
	switch (inModelID) {

		case cDocument:
			return 1;		// +++ find position
			break;

		default:
			return LApplication::GetPositionOfSubModel(inModelID, inSubModel);
			break;
	}
}


PP_End_Namespace_PowerPlant
