// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UTextEdit.h					PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Common declarations for text editing features

#ifndef _H_UTextEdit
#define _H_UTextEdit
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

enum {
	textAttr_MultiStyle			= 0x8000,
	textAttr_Editable			= 0x4000,
	textAttr_Selectable			= 0x2000,
	textAttr_WordWrap			= 0x1000,
	textAttr_AutoScroll			= 0x0800,
	textAttr_OutlineHilite		= 0x0400
};

typedef struct	STextEditUndo {
	Handle	textH;
	SInt16	selStart;
	SInt16	selEnd;
} STextEditUndo, **STextEditUndoH;

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
