// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UVolume.cp			   		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	A collection of utility routines for manipulation and information
//	gathering with volumes.
//
//	Optionally utilizes Jim Luther/Apple DTS's MoreFiles package. Freely
//	available on the net.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UVolume.h>

#if PP_MoreFiles_Support
	#include "Search.h"
	#include "MoreDesktopMgr.h"
#endif

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Launching applications
//
//	What follows are some routines to facilitate the programatic
//	launching of other applications. These routines are utilized
//	by UProcess.
//
//	Code based upon SignatureToApp, an Apple DTS code snippet originally
//	authored by Jens Alfke in 1991. Snippet from the documentation:
//
//		To find an appropriate application on disk, SignatureToApp asks
//		each mounted volume's desktop database to find an application
//		with the desired signature. It first checks the volume containing
//		the active System file, then all other mounted volumes.
//
//		SignatureToApp cannot find applications on floppy disks since
//		volumes less than 2MB in size do not have a desktop database.
//
//		Occasionally SignatureToApp may not be able to find an appliaction
//		on a file server. This turns ou to be the fault of the desktop
//		database on the server. A lot of server volumes seem to have
//		messed-up desktop databases which may not have entries for all the
//		applications on the volume. The solution is for the administrator
//		to rebuild the desktops on the file server volumes.
//
//	Also, some code optionally uses Jim Luther/Apple DTS's fabulous
//	MoreFiles package (available freely on the net).
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//	¥ IsApplication
// ---------------------------------------------------------------------------
//	Determine if a given file is an application or not

bool
UVolume::IsApplication(
	const FSSpec&	inFileSpec)
{
	bool	isApp = false;

	FInfo	fileInfo;

	OSErr err = ::FSpGetFInfo(&inFileSpec, &fileInfo);
	ThrowIfOSErr_(err);

	if ((fileInfo.fdType == FOUR_CHAR_CODE('APPL'))			// Regular MacOS app
		|| (fileInfo.fdType == FOUR_CHAR_CODE('appe'))		// Background app
		|| (fileInfo.fdType == FOUR_CHAR_CODE('APPC'))		// App which if dropped on the System
															//	folder auto-routes to Control Panels
		|| (fileInfo.fdType == FOUR_CHAR_CODE('APPD')) ) {	// App auto-routes to Apple Menu Items
		isApp = true;
	}

	return isApp;
}


// ---------------------------------------------------------------------------
//	¥ GetIndVolume
// ---------------------------------------------------------------------------
//	Get the vRefNum of an indexed on-line volume

SInt16
UVolume::GetIndVolume(
	SInt16	inIndex)
{
	Assert_(inIndex > 0);	// See IM:Files 2-144 in PBHGetVInfo's Description

	HParamBlockRec pb;
	pb.volumeParam.ioCompletion	= nil;
	pb.volumeParam.ioNamePtr	= nil;
	pb.volumeParam.ioVolIndex	= inIndex;

	OSErr err = ::PBHGetVInfoSync(&pb);
	ThrowIfOSErr_(err);

	return pb.volumeParam.ioVRefNum;
}


// ---------------------------------------------------------------------------
//	¥ IsSearchable
// ---------------------------------------------------------------------------
//	Given the index of a volume, determine if it is a searchable volume.

bool
UVolume::IsSearchable(
	SInt16	inIndex)
{
		// In the future this might contain more checks on "is searchable"
		// criteria (like if the volume support PBCatSearch, etc.), but for
		// purposes right now, we just need to make sure it's an HFS volume.
		// (Thanx to Richard Atwell for this code)

	HParamBlockRec	pb;
	Str31			volName;

	pb.volumeParam.ioCompletion	= nil;
	pb.volumeParam.ioNamePtr	= volName;
	pb.volumeParam.ioVRefNum	= 0;
	pb.volumeParam.ioVolIndex	= inIndex;

	OSErr err = ::PBHGetVInfoSync(&pb);
	ThrowIfOSErr_(err);

	bool isSearchable = true;	// Most of the time it should be

	if (pb.volumeParam.ioVFSID != 0x0000) {

			// In the ioVFSID field, some values are:
			//
			// 0x0000 for MFS, HFS, and HFS+ (here to tell apart by signature words)
			// 0x0100 ProDOS file system
			// 0x0101 PowerTalk Mail Enclosures
			// 0x4147 ISO 9660 File Access (through Foreign File Access)
			// 0x4242 High Sierra File Access (through FFA)
			// 0x464D QuickTake File System (through FFA)
			// 0x4953 Macintosh PC Exchange (MS-DOS)
			// 0x4A48 Audio CD Access (through FFA)
			// 0x4D4B Apple Photo Access (through FFA)
			//
			// This information taken from the "Guide - File System Manager" PDF
			// file found on the Developer CD Series January 1998 Mac OS SDK
			// CD Disk 1 (and HFS+ confirmed by Pete Gontier at Apple DTS).

		isSearchable = false;
	}

	return isSearchable;
}


// ---------------------------------------------------------------------------
//	¥ GetNumVolumes
// ---------------------------------------------------------------------------
//	Returns the number of mounted volumes

SInt16
UVolume::GetNumVolumes()
{
	HParamBlockRec	pb;
	Str31			volName;

	pb.volumeParam.ioCompletion	= nil;
	pb.volumeParam.ioNamePtr	= volName;
	pb.volumeParam.ioVolIndex	= 1;

	OSErr	err		= noErr;
	SInt16	numVols	= 0;

	while (err == noErr) {
		err = ::PBHGetVInfoSync(&pb);
		if (err == noErr) {
			numVols = pb.volumeParam.ioVolIndex;
		}
		++pb.volumeParam.ioVolIndex;
	}

	return numVols;
}


// ---------------------------------------------------------------------------
//	¥ VolHasDesktopDB
// ---------------------------------------------------------------------------
//	Check if a volume supports desktop DB calls.

bool
UVolume::VolHasDesktopDB(
	SInt16	inVRefNum)
{
	const UInt32 bHasNewDesk = 1L << bHasDesktopMgr;	// Flag mask for vMAttrib field

	HParamBlockRec			pb;
	GetVolParmsInfoBuffer	info;

	pb.ioParam.ioCompletion	= nil;
	pb.ioParam.ioNamePtr	= nil;
	pb.ioParam.ioVRefNum	= inVRefNum;
	pb.ioParam.ioBuffer		= (Ptr)&info;
	pb.ioParam.ioReqCount	= sizeof(GetVolParmsInfoBuffer);

	OSErr err = ::PBHGetVolParmsSync(&pb);
	ThrowIfOSErr_(err);

	bool hasDB = ((info.vMAttrib & bHasNewDesk) != 0);

	return hasDB;
}


// ---------------------------------------------------------------------------
//	¥ FindAppOnVolume
// ---------------------------------------------------------------------------
//	Search for an application's location on a given volume. Returns
//	true (false) if the application was found (not found) on the
//	given volume.
//
//	Searches by signature (for speed and ease of querying the desktop
//	database, and should also be more accurate than searching by filename).
//	MoreFiles' FSpDTGetAPPL() and related functions perform a similar
//	type of query. The implementation here could be easily changed to:
//
//		OSErr err = FSpDTGetAPPL(nil, inVRefNum, inSignature, &outFSSpec);
//
//	MoreFiles' search is a more complete and comprehensive search. But it is
//	also more time consuming to perform.

bool
UVolume::FindAppOnVolume(
	OSType		inSignature,
	SInt16		inVRefNum,
	FSSpec&		outFSSpec)
{
	DTPBRec	pb;
	pb.ioCompletion	= nil;
	pb.ioVRefNum	= inVRefNum;
	pb.ioNamePtr	= nil;

	OSErr err = ::PBDTGetPath(&pb);		// Puts DT refNum into pb.ioDTRefNum
	ThrowIfOSErr_(err);

	SInt16 dtRefNum = pb.ioDTRefNum;

	pb.ioCompletion		= nil;
	pb.ioDTRefNum		= dtRefNum;
	pb.ioIndex			= 0;
	pb.ioFileCreator	= inSignature;
	pb.ioNamePtr		= (StringPtr)outFSSpec.name;

	err = ::PBDTGetAPPLSync(&pb);

	outFSSpec.vRefNum	= inVRefNum;
	outFSSpec.parID		= pb.ioAPPLParID;

	bool	foundIt = true;
	if ((err == fnfErr) || (err == afpItemNotFound)) {
		foundIt = false;
		err = noErr;
	}

	ThrowIfOSErr_(err);

	return foundIt;
}


#if PP_MoreFiles_Support

// ---------------------------------------------------------------------------
//	¥ FindAppOnVolume
// ---------------------------------------------------------------------------
//	Search for an application's location on a given volume. Returns
//	true (false) if the application was found (not found) on the
//	given volume.
//
//	Searches by filename. To take advantage of code reuse, this method
//	requires the use of Jim Luther/Apple DTS's MoreFiles. This technique
//	can be slower than by-signature/destkop approach above, but sometimes
//	you just gotta use a name.
//
//	Allows one to specify a partial name for the file, e.g. "CodeWarrior IDE"
//	instead of "CodeWarrior IDE 2.1".

bool
UVolume::FindAppOnVolume(
	ConstStr255Param	inName,
	SInt16				inVRefNum,
	FSSpec&				outFSSpec,
	bool				inPartial)
{
		// We'll use the NameFileSearch from MoreFile's Search.c to
		// accomplish this.

		// NameFileSearch just searches for any file with the given
		// string. We have to add in an extra check to ensure it's
		// an application

	FSSpec	theFSSpec;
	SInt32	numMatches = 0;
	OSErr	err = noErr;
	bool	newSearch = true;

	do {
		err = NameFileSearch(	nil,
								inVRefNum,
								inName,
								&theFSSpec,
								1,
								&numMatches,
								newSearch,
								inPartial );
		newSearch = false;
	} while ((err == noErr) && (UVolume::IsApplication(theFSSpec) == false));

	if (err != eofErr)	{	// A normal error for NameFileSearch
		ThrowIfOSErr_(err);
	}

	bool foundIt = false;
	if (numMatches >= 1) {
		foundIt = true;
		outFSSpec = theFSSpec;
	}

	return foundIt;
}


#endif // PP_MoreFiles_Support


// ---------------------------------------------------------------------------
//	¥ FindApp
// ---------------------------------------------------------------------------
//	Find an application's location. Returns true if found (false if not).
//	Will automatically iterate through all mounted volumes to search for
//	the application.
//
//	Takes the application by signature

bool
UVolume::FindApp(
	OSType		inSignature,
	FSSpec&		outFSSpec)
{
	SInt16 numVolumes	= UVolume::GetNumVolumes();
	SInt16 vRefNum		= 0;

		// Walk the mounted volumes....
	bool foundIt = false;
	for (SInt16 index = 1; index <= numVolumes; ++index) {

			// Make sure the volume is searchable
		if (UVolume::IsSearchable(index) == false) {
			continue;
		}

			// Get the volume
		vRefNum = UVolume::GetIndVolume(index);

			// See if the volume has a desktop database for us to query
		if (UVolume::VolHasDesktopDB(vRefNum)) {

				// Let's see if the app is on this volume...
			if (UVolume::FindAppOnVolume(inSignature, vRefNum, outFSSpec)) {
					// It is, let's bail out
				foundIt = true;
				break;
			}
		}
	}

	return foundIt;
}


#if PP_MoreFiles_Support

// ---------------------------------------------------------------------------
//	¥ FindApp
// ---------------------------------------------------------------------------
//	Find an application's location. Returns true if found (false if not).
//	Will automatically iterate through all mounted volumes to search for
//	the application.
//
//	Takes the application by name, optionally partial.

bool
UVolume::FindApp(
	ConstStr255Param	inName,
	FSSpec&				outFSSpec,
	bool				inPartial)
{
	SInt16 numVolumes	= UVolume::GetNumVolumes();
	SInt16 vRefNum		= 0;

		// Walk the mounted volumes....
	bool foundIt = false;
	for (SInt16 index = 1; index <= numVolumes; ++index) {

			// Make sure the volume is searchable
		if (UVolume::IsSearchable(index) == false) {
			continue;
		}

			// Get the volume
		vRefNum = UVolume::GetIndVolume(index);

			// See if the volume has a desktop database for us to query
		if (UVolume::VolHasDesktopDB(vRefNum)) {

				// Let's see if the app is on this volume
			if (UVolume::FindAppOnVolume(inName, vRefNum, outFSSpec, inPartial)) {
					// it is, let's bail out
				foundIt = true;
				break;
			}
		}
	}

	return foundIt;
}

#endif // PP_MoreFiles_Support


PP_End_Namespace_PowerPlant
