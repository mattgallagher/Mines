// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSingleDoc.cp				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	Document which associates one file with one window

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LSingleDoc.h>
#include <LWindow.h>
#include <LFile.h>

#include <LString.h>
#include <PP_Messages.h>

#include <AEInteraction.h>
#include <AERegistry.h>
#include <AEObjects.h>
#include <AEPackObject.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LSingleDoc							Default Constructor		  [public]
// ---------------------------------------------------------------------------
//

LSingleDoc::LSingleDoc()
{
	mWindow = nil;
	mFile   = nil;
}


// ---------------------------------------------------------------------------
//	¥ LSingleDoc(LCommander*)				Constructor				  [public]
// ---------------------------------------------------------------------------

LSingleDoc::LSingleDoc(
	LCommander*		inSuper)

	: LDocument(inSuper)
{
	mWindow = nil;
	mFile   = nil;
}


// ---------------------------------------------------------------------------
//	¥ ~LSingleDoc							Destructor				  [public]
// ---------------------------------------------------------------------------

LSingleDoc::~LSingleDoc()
{
	delete mWindow;
	delete mFile;
}


// ---------------------------------------------------------------------------
//	¥ AllowSubRemoval												  [public]
// ---------------------------------------------------------------------------

Boolean
LSingleDoc::AllowSubRemoval(
	LCommander*		inSub)
{
	if (inSub == mWindow) {

			// Check if the current AppleEvent is a "close" event
			// sent to the Window. If so, we handle it as if the
			// "close" event were sent to the Document

		AppleEvent	currentEvent;
		DescType	theType;
		DescType	theAttr = typeNull;
		Size		theSize;
		::AEGetTheCurrentEvent(&currentEvent);
		if (currentEvent.descriptorType != typeNull) {
			::AEGetAttributePtr(&currentEvent, keyEventClassAttr,
				typeType, &theType, &theAttr, sizeof(DescType),
				&theSize);
			if (theAttr == kAECoreSuite) {
				::AEGetAttributePtr(&currentEvent, keyEventIDAttr,
					typeType, &theType, &theAttr, sizeof(DescType),
					&theSize);
				if (theAttr == kAEClose) {
					DoAEClose(currentEvent);
					return false;
				}
			}
		}

		AttemptClose(true);			// A non-AppleEvent close
		return false;

	} else {
		return true;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------
//	Pass back the name of a Document

StringPtr
LSingleDoc::GetDescriptor(
	Str255	outDescriptor) const
{
	if ((mFile != nil) && mIsSpecified) {
		FSSpec	fileSpec;			// Document name is same as its File
		mFile->GetSpecifier(fileSpec);
		LString::CopyPStr(fileSpec.name, outDescriptor);

	} else if (mWindow != nil) {	// No File, use name of its Window
		mWindow->GetDescriptor(outDescriptor);

	} else {						// No File and No Window
		outDescriptor[0] = 0;		//   Document name is empty string
	}

	return outDescriptor;
}


// ---------------------------------------------------------------------------
//	¥ UsesFileSpec													  [public]
// ---------------------------------------------------------------------------
//	Returns whether the Document's File has the given FSSpec

Boolean
LSingleDoc::UsesFileSpec(
	const FSSpec&	inFileSpec) const
{
	Boolean usesFS = false;

	if (mFile != nil) {
		usesFS = mFile->UsesSpecifier(inFileSpec);
	}

	return usesFS;
}


// ---------------------------------------------------------------------------
//	¥ MakeCurrent													  [public]
// ---------------------------------------------------------------------------
//	Make this Document the current one by selecting its Window

void
LSingleDoc::MakeCurrent()
{
	if (mWindow != nil) {
		mWindow->Select();
	}
}

PP_End_Namespace_PowerPlant
