// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFileDesignator.i			PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//
//	Declarations of functions which each implementation of LFileDesignator
//	must define. Do NOT #include this file in client sources.
//
//	The header file for an implementation of LFileDesignator should
//	have the following format:
//
//		namespace UImplementationName {
//
//			class LFileDesignator {
//			public:
//				#include <LFileDesignator.i>
//
//				// ... Declarations specific to this implementation
//		}
//
//	Then, the source file should define each the functions in this file
//	within its namespace.

#ifdef __MWERKS__
#pragma notonce
#endif

						LFileDesignator();

						~LFileDesignator();

	void				SetFileType(
								OSType				inFileType);

	void				SetFileCreator(
								OSType				inFileCreator);

	NavDialogOptions*	GetDialogOptions();

	void				SetDefaultLocation(
								const FSSpec&		inFileSpec,
								bool				inSelectIt);

	bool				AskDesignateFile(
								ConstStringPtr		inDefaultName);

	bool				IsValid() const;

	void				GetFileSpec(
								FSSpec&				outFileSpec) const;

	bool				IsReplacing() const;

	bool				IsStationery() const;

	ScriptCode			GetScriptCode() const;

	void				CompleteSave(
								NavTranslationOptions	inOption = kNavTranslateInPlace);
