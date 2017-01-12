// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFileChooser.i				PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//
//	Declarations of functions which each implementation of LFileChooser
//	must define. Do NOT #include this file in client sources.
//
//	The header file for an implementation of LFileChooser should
//	have the following format:
//
//		namespace UImplementationName {
//
//			class LFileChooser {
//			public:
//				#include <LFileChooser.i>
//
//				// ... Declarations specific to this implementation
//		}
//
//	Then, the source file should define each the functions in this file
//	within its namespace.

#ifdef __MWERKS__
#pragma notonce
#endif

						LFileChooser();

						~LFileChooser();

	NavDialogOptions*	GetDialogOptions();

	void				SetDefaultLocation(
								const FSSpec&			inFileSpec,
								bool					inSelectIt);

	void				SetObjectFilterProc(
								NavObjectFilterProcPtr	inFilterProc);

	void				SetPreviewProc(
								NavPreviewProcPtr		inPreviewProc);

	bool				AskOpenFile(
								const LFileTypeList&	inFileTypes);

	bool				AskChooseOneFile(
								const LFileTypeList&	inFileTypes,
								FSSpec					&outFileSpec);

	bool				AskChooseFolder(
								FSSpec&					outFileSpec,
								SInt32&					outFolderDirID);

	bool				AskChooseVolume(
								FSSpec&					outFileSpec);

	bool				IsValid() const;

	SInt32				GetNumberOfFiles() const;

	void				GetFileSpec(
								SInt32		inIndex,
								FSSpec&		outFileSpec) const;

	void				GetFileDescList(
								AEDescList&	outDescList) const;

	ScriptCode			GetScriptCode() const;
