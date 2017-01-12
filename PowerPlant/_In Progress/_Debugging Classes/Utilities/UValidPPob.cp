// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UValidPPob.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: MW IDE Team. Heavy modifications by John C. Daub
//
//	Validates PPobs, comparing what is within the PPobs vs. what is registered
//	(in URegistrar). Helps to ensure you have everything registered that you
//	should.

#include <UValidPPob.h>
#include <LWindow.h>
#include <UMemoryMgr.h>
#include <LDataStream.h>
#include <URegistrar.h>
#include <LDebugStream.h>
#include <UModalDialogs.h>
#include <PP_Resources.h>
#include <LComparator.h>
#include <TArrayIterator.h>
#include <PP_DebugMacros.h>
#include <UCursor.h>


PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ ValidatePPob
// ---------------------------------------------------------------------------
//	Presents a dialog, asking for the ResIDT of the desired 'PPob' to
//	validate, then validates it. If a class is not registered, a signal
//	is raised to report the unregistered classes.
//
//	Like UModalDialogs::AskForOneNumber(), the ResIDT argument specifies
//	the ID of the dialog to present to ask for the PPob to validate. The
//	PaneIDT argument specifies the ID of the edit field. The edit field
//	can be of almost any type (LEditField, LEditText, etc.). Also like
//	AskForOneNumber(), the OK and Cancel buttons must broadcast msg_OK
//	and msg_Cancel, respectively.

void
UValidPPob::ValidatePPob(
	ResIDT		inDialogID,
	PaneIDT		inEditFieldID)
{
	UCursor::SetWatch();

		// Create the dialog.

	StDialogHandler	theHandler(inDialogID, LCommander::GetTopCommander());
	LWindow*		theDialog = theHandler.GetDialog();
	Assert_(theDialog != nil);

	LCommander*	editAsCommander = FindPaneByIDNoThrow_(theDialog, inEditFieldID, LCommander);
	if (editAsCommander == nil) {
		return;
	}

	theDialog->SetLatentSub(editAsCommander);
	theDialog->Show();

	MessageT theMessage;

	do {
		theMessage = theHandler.DoDialog();
	} while ((theMessage != msg_OK) && (theMessage != msg_Cancel));

	if (theMessage == msg_OK) {

		LPane*	editAsPane = FindPaneByIDNoThrow_(theDialog, inEditFieldID, LPane);
		if (editAsPane == nil) {
			return;
		}
			// Make sure it's a legal ResIDT
		SInt32 ppobID = editAsPane->GetValue();
		if ((ppobID >= min_Int16) && (ppobID <= max_Int16)) {

				// Validate the PPob

				// We must allocate the array to hold the list of unregistered classes.
			TArray<ClassIDT> theBadClassArray;
			theBadClassArray.SetComparator(LLongComparator::GetComparator(), false);

			LStr255 errorStr(StringLiteral_("PPob Validation for ID: "));
			errorStr += static_cast<SInt32>(ppobID);

			if (UValidPPob::ValidateOnePPob(static_cast<ResIDT>(ppobID), theBadClassArray)) {

					// All is well
				errorStr += StringLiteral_(" PASSED.");
				SignalString_(errorStr);

			} else {
					// Not valid, so report
				errorStr += StringLiteral_(" failed on class IDs:");

					// We'll just list 'em
				TArrayIterator<ClassIDT> iterate(theBadClassArray);
				ClassIDT theID;
				while (iterate.Next(theID)) {
					errorStr += StringLiteral_(" '");
					errorStr += LStr255(static_cast<FourCharCode>(theID));
					errorStr += StringLiteral_("',");
				}

				SignalString_(errorStr);
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ ValidateAllPPobs
// ---------------------------------------------------------------------------
//	Validates every PPob resource that it can find. Searches the entire
//	resource chain; you can manipulate the search chain with StResourceContext
//	prior to validating.
//
//	Will use LDebugStream to output the results. Will report on a per PPob
//	basis. Also provides a final "summary" of the unregistered classes (for
//	ease of reference).

void
UValidPPob::ValidateAllPPobs()
{
	UCursor::SetWatch();

		// We will report unregistered ID's on a per-PPob basis, but that
		// could easily have duplicates across PPob's. We'll maintain a master
		// list of the unregistered classes (no duplicates) for ease of overall
		// reference.

	TArray<ClassIDT>	masterBadClassArray;
	masterBadClassArray.SetComparator(LLongComparator::GetComparator(), false);

		// Open up a debug stream. We will write to file, and overwrite any
		// existing files.

	LDebugStream debugStream(flushLocation_File, false, false);

		// Check all the PPob's we can find.
	SInt16 ppobCount = ::CountResources(ResType_PPob);
	for (SInt16 index = 1; index <= ppobCount; ++index) {

		ResIDT	ppobID;
		ResType	resType;
		Str255	resName;

			// See if we can get a resource, and get it's info.
		{
			StResource	resH;
			resH.GetIndResource(ResType_PPob, index);

			::GetResInfo(resH, &ppobID, &resType, resName);
			ThrowIfResError_();
		}

			// Allocate our array to hold the bad classes list
		TArray<ClassIDT> theBadClassArray;
		theBadClassArray.SetComparator(LLongComparator::GetComparator(), false);

			// Write out the ResIDT of the PPob
		debugStream	<< StringLiteral_("PPob ID: ")
					<< ppobID
					<< '\r';

			// The name of the resource, if there is one
		if (resName[0] > 0) {
			debugStream << StringLiteral_("PPob Name: ")
						<< resName
						<< '\r';
		}

			// Validate it.
		if (UValidPPob::ValidateOnePPob(ppobID, theBadClassArray)) {
			// no problems

			debugStream << StringLiteral_("PASSED Validation\r\r");
		} else {
				// Problems....

				// Write the class IDs that are not registered
			TArrayIterator<ClassIDT> iterate(theBadClassArray);
			ClassIDT theID;

			debugStream << StringLiteral_("Unregistered class IDs: ");

			bool	first = true;
			while (iterate.Next(theID)) {
				if (!first) {
					debugStream << ',';
				} else {
					first = false;
				}

					// Append to our stream
				debugStream << StringLiteral_(" '")
							<< StringPtr(LStr255(static_cast<FourCharCode>(theID)))
							<< '\'';

					// And append to our master list, if not already present
				ArrayIndexT index = masterBadClassArray.FetchIndexOf(theID);
				if (index == LArray::index_Bad) {
					masterBadClassArray.AddItem(theID);
				}
			}

			debugStream	<< StringLiteral_(".\r\r");
		}
	}

		// Append the master bad class list to the stream
	if (masterBadClassArray.GetCount() > 0) {

		debugStream << StringLiteral_("Master unregistered class ID list:");

		TArrayIterator<ClassIDT> masterIterate(masterBadClassArray);

		ClassIDT theID;
		bool	first = true;
		while (masterIterate.Next(theID)) {
			if (!first) {
				debugStream << ',';
			} else {
				first = false;
			}

			debugStream << StringLiteral_(" '")
						<< StringPtr(LStr255(static_cast<FourCharCode>(theID)))
						<< '\'';
		}

		debugStream << '.';

	} else {
		debugStream << StringLiteral_("No unregistered classes.\r");
	}

		// We'll only flush if there is something to report (so if
		// you find no file, there's no problems).
	if (debugStream.GetLength() > 0) {

			// We won't append to the existing debug stream output file,
			// if any. Just to keep things a little clearer.
		debugStream.SetFilename(StringLiteral_("PPob Validation"));
		debugStream.Flush();
	}
}


// ---------------------------------------------------------------------------
//	¥ ValidateOnePPob
// ---------------------------------------------------------------------------
//	Performs a validation on the given PPob. Takes a reference to an
//	array into which the bad class ID's are stored. The above method stops
//	at the first bad ID it finds. This will find them all.
//
//	Caller is responsible for allocating (and disposing of) the TArray.

bool
UValidPPob::ValidateOnePPob(
	ResIDT				inPPobID,
	TArray<ClassIDT>&	outBadClassIDArray)
{
	bool isValid = true;

	try {
		StResource objectRes(ResType_PPob, inPPobID);
		::HLockHi(objectRes.mResourceH);

		LDataStream objectStream(*objectRes.mResourceH, ::GetHandleSize(objectRes.mResourceH));

		SInt16 ppobVersion;
		objectStream >> ppobVersion;
		Assert_(ppobVersion == 2);

		ClassIDT theBadClassID = 0;

		bool tempValid = isValid;

		while (objectStream.GetMarker() < objectStream.GetLength()) {

			tempValid = UValidPPob::ValidateObjectStream(objectStream, theBadClassID);

			if (tempValid == false) {
				isValid = false;
					// Only add it if it's not there already
				ArrayIndexT theIndex = outBadClassIDArray.FetchIndexOf(theBadClassID);
				if (theIndex == LArray::index_Bad) {
					outBadClassIDArray.AddItem(theBadClassID);
				}
				continue;
			}
		}

	} catch (...) {
		isValid = false;
	}

	return isValid;
}


// typedefs and constants borrowed from UReanimator
typedef	SInt32	TagID;

enum {
	tag_ObjectData		= FOUR_CHAR_CODE('objd'),
	tag_BeginSubs		= FOUR_CHAR_CODE('begs'),
	tag_EndSubs			= FOUR_CHAR_CODE('ends'),
	tag_Include			= FOUR_CHAR_CODE('incl'),
	tag_UserObject		= FOUR_CHAR_CODE('user'),
	tag_ClassAlias		= FOUR_CHAR_CODE('dopl'),
	tag_Comment			= FOUR_CHAR_CODE('comm'),
	tag_End				= FOUR_CHAR_CODE('end.'),
	object_Null			= FOUR_CHAR_CODE('null')
};



// ---------------------------------------------------------------------------
//	¥ ValidateObjectStream
// ---------------------------------------------------------------------------
//	Performs the actual validation of the PPob stream

bool
UValidPPob::ValidateObjectStream(
	LStream&	ioStream,
	ClassIDT&	outBadClassID)
{
	bool isValid = true;

	bool readingTags		= true;
	ClassIDT aliasClassID	= FOUR_CHAR_CODE('null');

	const TArray<SClassTableEntry>&	classTable = *URegistrar::GetClassTable();

	if (URegistrar::GetClassTable() == nil) {	// Rare circumstance, but it can happen!
		isValid = true;							// Make true to short circuit the search.
		ioStream.SetMarker(0, streamFrom_End);	// Short-circut the search.
		outBadClassID = FOUR_CHAR_CODE('\?\?\?\?');
		SignalStringLiteral_( "No class table! Nothing to validate" );
		return isValid;
	}

	do {
		TagID theTag = tag_End;
		ioStream >> theTag;

		switch (theTag) {

			case tag_ObjectData: {
				SInt32 dataLength;
				ioStream >> dataLength;
				SInt32 dataStart = ioStream.GetMarker();
				ClassIDT classID;
				ioStream >> classID;

				if (aliasClassID != FOUR_CHAR_CODE('null')) {
					classID = aliasClassID;
				}

				SClassTableEntry entry = {classID, NULL};
				ArrayIndexT entryIndex = classTable.FetchIndexOf(entry);
				if (entryIndex == LArray::index_Bad) {
					outBadClassID = classID;
					isValid = false;
				}

				ioStream.SetMarker(dataStart + dataLength, streamFrom_Start);
				aliasClassID = FOUR_CHAR_CODE('null');
				break;
			}

			case tag_BeginSubs:
				isValid = UValidPPob::ValidateObjectStream(ioStream, outBadClassID);
				break;

			case tag_EndSubs:
			case tag_End:
				readingTags = false;
				break;

			case tag_UserObject:
				Assert_(false);
				break;

			case tag_ClassAlias:
				ioStream >> aliasClassID;
				break;

			case tag_Comment:
			{
				SInt32 commentLength;
				ioStream >> commentLength;
				ioStream.SetMarker(commentLength, streamFrom_Marker);
				break;
			}

			default:
				 // Unrecognized Tag
				Assert_(false);
				readingTags = false;
				break;
		}
	} while (readingTags && isValid);

	return isValid;
}

PP_End_Namespace_PowerPlant
