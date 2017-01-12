// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	URegistrar.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Maintains a table of (Class ID, Creator Function Pointer) pairs that
//	is used for dynamically creating objects from external data at runtime

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <URegistrar.h>
#include <LComparator.h>
#include <LString.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Class Variables

TArray<SClassTableEntry>*	URegistrar::sClassTable;


// ---------------------------------------------------------------------------
//	¥ RegisterClass											 [static] [public]
// ---------------------------------------------------------------------------
//	Register a ClassID and associated ClassCreatorFunc
//
//	If ClassID is already registered, the existing ClassCreatorFunc is
//	replaced.

void
URegistrar::RegisterClass(
	ClassIDT			inClassID,
	ClassCreatorFunc	inCreatorFunc)
{
	if (sClassTable == nil) {			// Create new class table
		sClassTable = new TArray<SClassTableEntry>();

			// Use LLongComparator since the ClassID field determines
			// the identity of an item (For comparing items, we don't
			// care about the creator function pointer). Also, we set
			// the comparator after construction so the Array doesn't
			// own the global LLongComparator (and delete it later).

		sClassTable->SetComparator(LLongComparator::GetComparator(), false);
	}

										// Fill in entry for this class
	SClassTableEntry	theEntry;
	theEntry.classID	 = inClassID;
	theEntry.creatorFunc = inCreatorFunc;

		// If class isn't already in the table, add it to the end.
		// If it is already in the table, replace the existing entry
		// with this one.

	ArrayIndexT	index = sClassTable->FetchIndexOf(theEntry);
	if (index == LArray::index_Bad) {
		sClassTable->AddItem(theEntry);

	} else {
		sClassTable->AssignItemsAt(1, index, theEntry);
		
			// Optional warning for duplicate class ID
		
		#if PP_Warn_Duplicate_Class_ID
			LStr255	msg(StringLiteral_("Duplicate Class ID: "));
			msg.Append(&inClassID, sizeof(ClassIDT));
			SignalString_(msg);
		#endif
	}
}


// ---------------------------------------------------------------------------
//	¥ UnregisterClass										 [static] [public]
// ---------------------------------------------------------------------------
//	Unregister a class by removing its ClassID from class table

void
URegistrar::UnregisterClass(
	ClassIDT	inClassID)
{
	if (sClassTable != nil) {

			// The creator function pointer doesn't matter, since
			// we identify class table entries by ClassID only

		SClassTableEntry	theEntry;
		theEntry.classID	 = inClassID;

		sClassTable->Remove(theEntry);
	}
}


// ---------------------------------------------------------------------------
//	¥ CreateObject											 [static] [public]
// ---------------------------------------------------------------------------
//	Return a newly created object of the specified class using data from
//	a Stream

void*
URegistrar::CreateObject(
	ClassIDT	inClassID,
	LStream		*inStream)
{
	void*	theObject = nil;			// In case we fail

	if (sClassTable != nil) {
										// Search for ClassID in table
		SClassTableEntry	theEntry;
		theEntry.classID = inClassID;

		ArrayIndexT	index = sClassTable->FetchIndexOf(theEntry);
		if (index != LArray::index_Bad) {
										// Found it. Call creator function
			theObject = (*(*sClassTable)[index].creatorFunc)(inStream);
		}
	}

	return theObject;
}


// ---------------------------------------------------------------------------
//	¥ DisposeClassTable										 [static] [public]
// ---------------------------------------------------------------------------
//	Dispose of the class table associated with URegistrar.
//
//	You will need to call this function to free the memory used by
//	URegistrar if you use URegistrar for a non-application program,
//	such as a code resource or shared library. URegistrar allocates a
//	Handle in the heap that you need to free when terminating. You
//	don't need to call this function from an application, since the
//	application's is destroyed upon exiting.

void
URegistrar::DisposeClassTable()
{
	delete sClassTable;
	sClassTable = nil;
}


PP_End_Namespace_PowerPlant
