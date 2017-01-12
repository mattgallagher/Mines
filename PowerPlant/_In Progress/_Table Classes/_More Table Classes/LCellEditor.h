// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCellEditor.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	A simple class for editing any textual data in a table, used by
//	LTableEditableSelector

#ifndef _H_LCellEditor
#define _H_LCellEditor
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LInPlaceEditField;

// ---------------------------------------------------------------------------

class LCellEditor {
public:
						LCellEditor();
	virtual				~LCellEditor();

protected:
	LInPlaceEditField*		mEditField;	// self-destroying
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
