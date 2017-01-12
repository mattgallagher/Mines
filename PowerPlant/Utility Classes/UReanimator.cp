// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UReanimator.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Functions for creating objects using data from resources

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UReanimator.h>

#include <LCommander.h>
#include <LControl.h>
#include <LDataStream.h>
#include <LString.h>
#include <LView.h>
#include <PP_Resources.h>
#include <UMemoryMgr.h>
#include <URegistrar.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Types and Constants

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

#pragma options align=mac68k

typedef struct {						// Counted list of Pane ID's
	SInt16	numberOfItems;
	PaneIDT	itemID[1];
} SResList, *SResListP;

#pragma options align=reset

// ---------------------------------------------------------------------------
//	Class Variables

bool	UReanimator::sConstructionFailed = false;


// ---------------------------------------------------------------------------
//	¥ ReadObjects
// ---------------------------------------------------------------------------
//	Create new objects from the data read from a resource. Return a pointer
//	to the first object created.
//
//	Before calling, be sure to properly set the default Commander, View,
//	and Attachable.

void*
UReanimator::ReadObjects(
	OSType	inResType,
	ResIDT	inResID)
{
	StResource	objectRes(inResType, inResID);
	::HLockHi((Handle)objectRes);

	LDataStream		objectStream(*objectRes.mResourceH,
									GetHandleSize(objectRes.mResourceH));

	SInt16	ppobVersion;
	objectStream >> ppobVersion;

	SignalIf_(ppobVersion != 2);

	void	*theObject = ObjectsFromStream(&objectStream);

	return theObject;
}


// ---------------------------------------------------------------------------
//	¥ ObjectsFromStream
// ---------------------------------------------------------------------------
//	Create new objects from the data in a Stream. Return a pointer
//	to the first object created.

void*
UReanimator::ObjectsFromStream(
	LStream*	inStream)
{
	void		*firstObject = nil;
	ClassIDT	aliasClassID = 'null';

								// Save current defaults
	LCommander	*defaultCommander  = LCommander::GetDefaultCommander();
	LView		*defaultView	   = LPane::GetDefaultView();
	LAttachable	*defaultAttachable = LAttachable::GetDefaultAttachable();

	bool		readingTags = true;
	
	sConstructionFailed = false;		// No construction failure yet
	
	do {
		void*	currentObject = nil;	// Object created by current tag

		TagID	theTag = tag_End;
		*inStream >> theTag;

		switch (theTag) {

			case tag_ObjectData: {
					// Restore default Commander, View, and Attachable
				LCommander::SetDefaultCommander(defaultCommander);
				LPane::SetDefaultView(defaultView);
				LAttachable::SetDefaultAttachable(defaultAttachable);

					// Object data consists of a byte length, class ID,
					// and then the data for the object. We use the
					// byte length to manually set the stream marker
					// after creating the object just in case the
					// object constructor doesn't read all the data.

				SInt32		dataLength;
				*inStream >> dataLength;

				SInt32		dataStart = inStream->GetMarker();

				ClassIDT	classID;
				*inStream >> classID;

				if (aliasClassID != FOUR_CHAR_CODE('null')) {
						// The previous tag specified an Alias for
						// the ID of this Class
					classID = aliasClassID;
				}

				try {								 	// Allocate and Construct Object
					currentObject = URegistrar::CreateObject(classID, inStream);
				}

				catch (...) {	
				
					#ifdef Debug_Signal
						LStr255 msg(StringLiteral_("Object Construction Failed. ClassID: "));
						msg.Append(&classID, sizeof(classID));
						SignalString_(msg);
					#endif
					
					sConstructionFailed = true;			// Object construction failed
					
					classID = FOUR_CHAR_CODE('null');	// Prevent signal below
				}

				inStream->SetMarker(dataStart + dataLength, streamFrom_Start);

				aliasClassID = FOUR_CHAR_CODE('null');	// Alias is no longer in effect

				#ifdef Debug_Signal
					if (currentObject == nil  &&  classID != FOUR_CHAR_CODE('null')) {
						LStr255	msg(StringLiteral_("Unregistered ClassID: "));
						msg.Append(&classID, sizeof(classID));
						SignalString_(msg);
					}
				#endif
				break;
			}

			case tag_BeginSubs:				// Recursive call to create subobjects
				currentObject = ObjectsFromStream(inStream);
				break;

			case tag_EndSubs:
			case tag_End:
				readingTags = false;
				break;

			case tag_UserObject: {

					// The UserObject tag is only needed for the Constructor
					// view editing program. It tells Constructor to treat
					// the following object as if it were an object of the
					// specified superclass (which must be a PowerPlant
					// class that Constructor knows about). We don't need
					// this information here, so we just read and ignore
					// the superclass ID.

				ClassIDT	superClassID;
				*inStream >> superClassID;
				break;
			}

			case tag_ClassAlias:

					// The ClassAlias tag defines the ClassID the for
					// the next object in the Stream. This allows you
					// to define an object which belongs to a subclass
					// of another class, but has the same data as that
					// other class.

				*inStream >> aliasClassID;
				break;

			case tag_Comment: {

					// The Comment tag denotes data used by PPob editors
					// that PowerPlant ignores. Format is a long word
					// byte count followed by arbitrary hex data.

				SInt32	commentLength;
				*inStream >> commentLength;

				inStream->SetMarker(commentLength, streamFrom_Marker);
				break;
			}

			default: {
				LStr255	msg(StringLiteral_("Unrecognized Tag: "));
				msg.Append(&theTag, sizeof(theTag));
				SignalString_(msg);
				readingTags = false;
				break;
			}
		}

		if (firstObject == nil) {
			firstObject = currentObject;
		}

	} while (readingTags and not sConstructionFailed);

	return firstObject;
}


// ---------------------------------------------------------------------------
//	¥ LinkListenerToBroadcasters
// ---------------------------------------------------------------------------
//	Associate a Listener with Broadcasters specified as a list of Pane ID's
//	stored in a resource.
//
//	Only Panes that are also Broadcasters are linked to the Listener. Panes
//	that are not Broadcasters are ignored.

void
UReanimator::LinkListenerToBroadcasters(
	LListener*	inListener,
	LView*		inControlContainer,
	ResIDT		inResListID)
{
	StResource	idList(ResType_IDList, inResListID);
	::HLockHi(idList.mResourceH);
	SResListP	resListP = (SResListP) *idList.mResourceH;

	for (SInt16 i = 0; i < resListP->numberOfItems; i++) {

		LBroadcaster*	theBroadcaster = dynamic_cast<LBroadcaster*>
						(inControlContainer->FindPaneByID(resListP->itemID[i]));

		if (theBroadcaster != nil) {
			theBroadcaster->AddListener(inListener);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ LinkListenerToControls
// ---------------------------------------------------------------------------
//	Associate a Listener with Controls specified as a list of Pane ID's
//	stored in a resource
//
//	Note: You should probably use LinkListenerToBroadcasters() instead of
//	this function. That function is more general since it handles all Panes
//	that are also Broadcasters rather than just Controls.

void
UReanimator::LinkListenerToControls(
	LListener*	inListener,
	LView*		inControlContainer,
	ResIDT		inResListID)
{
	StResource	idList(ResType_IDList, inResListID);
	::HLockHi(idList.mResourceH);
	SResListP	resListP = (SResListP) *idList.mResourceH;

	for (SInt16 i = 0; i < resListP->numberOfItems; i++) {

		LControl	*theControl = dynamic_cast<LControl*>
					(inControlContainer->FindPaneByID(resListP->itemID[i]));
		if (theControl != nil) {
			theControl->AddListener(inListener);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ CreateView
// ---------------------------------------------------------------------------
//	Return a newly created View object initialized from a PPob resource

LView*
UReanimator::CreateView(
	ResIDT			inViewID,
	LView*			inSuperView,
	LCommander*		inSuperCommander)
{
	LView::SetDefaultView(inSuperView);
	LCommander::SetDefaultCommander(inSuperCommander);
	LAttachable::SetDefaultAttachable(nil);

	LView*	theView =
		UReanimator::ReanimateObjects<LView>(ResType_PPob, inViewID);

	ThrowIfNil_(theView);

	try {
		theView->FinishCreate();
	}
	
	catch (...) {						// FinishCreate failed. View is
		delete theView;					//   in an inconsistent state.
		throw;							//   Delete it and rethrow.
	}
	
	if (theView->IsVisible()) {
		theView->Refresh();
	}

	return theView;
}


PP_End_Namespace_PowerPlant
