// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UStandardDialogs.i			PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//
//	Declarations of functions which each implementation of UStandardDialogs
//	must define. Do NOT #include this file in client sources.
//
//	The header file for an implementation of UStandardDialogs should
//	have the following format:
//
//		namespace UImplementationName {
//			#include <UStandardDialogs.i>
//		}
//
//	Then, the source file should define each the functions in this file
//	within its namespace.

#ifdef __MWERKS__
#pragma notonce
#endif

					// Preload system facilities for managing standard dialogs

	void		Load();


					// Unload system facilties for managing standard dialogs

	void		Unload();


					// Ask to save changes to before closing document
					// or quitting application

	SInt16		AskSaveChanges(
						ConstStringPtr		inDocumentName,
						ConstStringPtr		inAppName,
						bool				inQuitting);


					// Ask to confirm discarding changes and reverting
					// to the last saved version of the document

	bool		AskConfirmRevert(
						ConstStringPtr		inDocumentName);


					// Ask to select one file to open

	bool		AskOpenOneFile(
						OSType					inFileType,
						FSSpec&					outFileSpec,
						NavDialogOptionFlags	inFlags = kNavDefaultNavDlogOptions);


					// Ask to choose one file (usually not a document)

	bool		AskChooseOneFile(
						OSType					inFileType,
						FSSpec&					outFileSpec,
						NavDialogOptionFlags	inFlags = kNavDefaultNavDlogOptions);


					// Ask to choose a folder

	bool		AskChooseFolder(
						FSSpec&					outFileSpec,
						SInt32&					outFolderDirID);


					// Ask to choose a volume

	bool		AskChooseVolume(
						FSSpec&					outFileSpec);

					// Ask to specify a name and location for saving
					// a document

	bool		AskSaveFile(
						ConstStringPtr			inDefaultName,
						OSType					inFileType,
						FSSpec&					outFileSpec,
						bool&					outReplacing,
						NavDialogOptionFlags	inFlags = kNavDefaultNavDlogOptions);
