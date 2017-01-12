// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UStandardFile.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	GetDirectory - Uses a custom Standard File dialog which allows the
//		user to select a directory (folder or volume)

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UStandardFile.h>

#if TARGET_API_MAC_CARBON

		// StandardFile not supported. Remove this file from Carbon targets.
		// Use UNavServicesDialogs instead.

	#error "Carbon does not support StandardFile"

#endif

#include <UClassicDialogs.h>
#include <UMemoryMgr.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ GetDirectory											 [static] [public]
// ---------------------------------------------------------------------------
//	Prompts the user to select a Directory using a modified version of
//	the Standard Get File dialog box. An extra "Select Directory" button
//	is added, which must be item_SelectDirButton (defined above).
//
//	Returns true if the user selected a Directory, in which case the
//	StandardFileReply will contain info about the selected Directory.
//
//	Returns false if the user cancelled the action.

Boolean
UStandardFile::GetDirectory(
	StandardFileReply&	outReply,
	ResIDT				inDLOGid)
{
	OSType		fileTypes;
	Point		where = {-1, -1};		// Let Toolbox center the dialog


	StRoutineDescriptor<FileFilterYDUPP>	// File filter function
			fileFilter(NewFileFilterYDProc(
					UClassicDialogs::FileFilter_Directories));

	StRoutineDescriptor<DlgHookYDUPP>		// Dialog hook function
			dlgHookUPP(NewDlgHookYDProc(
					UClassicDialogs::DialogHook_SelectDirectory));


	UClassicDialogs::SHookUserData	userData;	// User data for hook function
	FSSpec		defaultSpec;
	defaultSpec.name[0] = 0;

	userData.reply			= &outReply;
	userData.defaultSpec	= &defaultSpec;
	userData.selectDefault	= false;

	::CustomGetFile(
			fileFilter,
			-1,							// Number of types (-1 = all types)
			&fileTypes,
			&outReply,
			inDLOGid,
			where,
			dlgHookUPP,
			nil,						// Modal Filter Proc
			nil,						// Activation List
			nil,						// Activation Proc
			&userData);

	return outReply.sfGood;
}


PP_End_Namespace_PowerPlant
