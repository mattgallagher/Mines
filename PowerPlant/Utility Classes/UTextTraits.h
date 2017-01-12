// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UTextTraits.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UTextTraits
#define _H_UTextTraits
#pragma once

#include <PP_Prefix.h>
#include <TextEdit.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

#pragma options align=mac68k

typedef struct TextTraitsRecord {
	SInt16		size;
	SInt16		style;
	SInt16		justification;
	SInt16		mode;
	RGBColor	color;
	SInt16		fontNumber;
	Str255		fontName;
} TextTraitsRecord, *TextTraitsPtr, **TextTraitsH;

#pragma options align=reset

typedef const TextTraitsRecord	*ConstTextTraitsPtr;

// ---------------------------------------------------------------------------

class	UTextTraits {
public:
	enum { fontNumber_Unknown = -1 };

	static void			LoadSystemTraits( TextTraitsRecord& outTextTraits );

	static void			LoadTextTraits( TextTraitsPtr ioTextTraits );
	
	static SInt16		SetPortTextTraits( ConstTextTraitsPtr inTextTraits );
	
	static void			SetTETextTraits(
								ConstTextTraitsPtr	inTextTraits,
								TEHandle			inMacTEH,
								RGBColor			*ioColor = nil);

	static TextTraitsH	LoadTextTraits( ResIDT inTextTraitsID );
								
	static SInt16		SetPortTextTraits( ResIDT inTextTraitsID );
								
	static void			SetTETextTraits(
								ResIDT				inTextTraitsID,
								TEHandle			inMacTEH,
								RGBColor			*ioColor = nil);

	static void			LoadTextTraits(
								ResIDT				inTextTraitsID,
								TextTraitsRecord	&outTextTraits);
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
