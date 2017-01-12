// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UMenus.i					PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	Declarations of functions which each implementation of UMenus
//	must define. Do NOT #include this file in client sources.
//
//	The header file for an implementation of UMenus should
//	have the following format:
//
//		namespace UImplementationName {
//			#include <UMenus.i>
//		}
//
//	Then, the source file should define each the functions in this file
//	within its namespace.


	// Menu Enable/Disable

	void			EnableItem(		MenuHandle		inMacMenuH,
									UInt16			inMenuItem );
	void			DisableItem(	MenuHandle		inMacMenuH,
									UInt16			inMenuItem );
	bool			IsItemEnabled(	MenuHandle		inMacMenuH,
									UInt16			inMenuItem );


	// Hierarchical menus

	void			SetItemHierID(	MenuHandle		inMacMenuH,
									UInt16			inMenuItem,
									ResIDT			inMenuID );
	SInt16			GetItemHierID(	MenuHandle		inMacMenuH,
									UInt16			inMenuItem );


	// Text encoding

	void			SetItemTextEncoding(
									MenuHandle		inMacMenuH,
									UInt16			inMenuItem,
									TextEncoding	inEncoding );
	TextEncoding	GetItemTextEncoding(
									MenuHandle		inMacMenuH,
									UInt16			inMenuItem );


	// Keyboard equivalents

	void			SetItemKeyEquivalent(
									MenuHandle		inMacMenuH,
									UInt16			inMenuItem,
									SInt16			inKey,
									UInt8			inModifiers = kMenuNoModifiers,
									SInt16			inGlyph = 0 );
	void			GetItemKeyEquivalent(
									MenuHandle		inMacMenuH,
									UInt16			inMenuItem,
									SInt16			&outKey,
									UInt8			&outModifiers,
									SInt16			&outGlyph );
