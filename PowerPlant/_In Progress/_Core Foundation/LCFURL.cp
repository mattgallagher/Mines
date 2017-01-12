// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCFURL.cp					PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================

#include <LCFURL.h>
#include <Folders.h>

// ---------------------------------------------------------------------------
//	¥ LCFURL								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Contruct from a base URL and a path component

LCFURL::LCFURL(
	CFURLRef		inBaseURL,
	CFStringRef		inPathComponent,
	bool			inIsDirectory,
	CFAllocatorRef	inAllocator)
{
	CFURLRef	ref = ::CFURLCreateCopyAppendingPathComponent(
							inAllocator, inBaseURL,
							inPathComponent, inIsDirectory);
	
	if (ref == nil) {
		throw cf_bad_create();
	}
	
	SetTypeRef(ref);
}


// ---------------------------------------------------------------------------
//	¥ LCFURL								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from a FSRef

LCFURL::LCFURL(
	const FSRef&	inFSRef,
	CFAllocatorRef	inAllocator)
{
	CFURLRef	ref = nil;
	
		// CFURLCreateFromFSRef requires Carbon 1.1 or later,
		// so it might not be present

	if (CFURLCreateFromFSRef != nil) {
		ref = ::CFURLCreateFromFSRef(inAllocator, &inFSRef);
	}
	
	if (ref == nil) {
		throw cf_bad_create();
	}
	
	SetTypeRef(ref);
}


// ---------------------------------------------------------------------------
//	¥ LCFURL								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from a Folder specification

LCFURL::LCFURL(
	SInt16			inVRefNum,
	OSType			inFolderType,
	bool			inCreateFolder,
	CFAllocatorRef	inAllocator)
{
	FSRef 		folderRef;
	OSStatus	status = fnfErr;
	
		// FSFindFolder (and CFURLCreateFromFSRef) require
		// Carbon 1.1 or later, so they might not be present

	if (FSFindFolder != nil) {
		status = ::FSFindFolder( inVRefNum,
								 inFolderType,
								 inCreateFolder,
								 &folderRef );
	}
										 
	CFURLRef	ref = nil;
										 
	if (status == noErr) {
		ref = ::CFURLCreateFromFSRef(inAllocator, &folderRef);
	}
	
	if (ref == nil) {
		throw cf_bad_create();
	}
		
	SetTypeRef(ref);
}


// ---------------------------------------------------------------------------
//	LCFURL									Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from an existing CFURLRef
//
//	We retain the CFURLRef. Caller should release the CFURLRef if it
//	no longer needs it.

LCFURL::LCFURL(
	CFURLRef		inURLRef)

	: LCFObject(inURLRef)
{
}


// ---------------------------------------------------------------------------
//	¥ LCFURL								Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LCFURL::LCFURL(
	const LCFURL&	inSource)
{
	#pragma unused(inSource)		// $$$ Not yet implemented
}


// ---------------------------------------------------------------------------
//	¥ operator =							Assignment Operator		  [public]
// ---------------------------------------------------------------------------

LCFURL&
LCFURL::operator = (
	const LCFURL&	inSource)
{
	#pragma unused(inSource)		// $$$ Not yet implemented
	
	return *this;
}


// ---------------------------------------------------------------------------
//	¥ ~LCFURL								Destructor				  [public]
// ---------------------------------------------------------------------------

LCFURL::~LCFURL()
{
}
