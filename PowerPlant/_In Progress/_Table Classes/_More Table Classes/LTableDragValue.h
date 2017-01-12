// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableDragValue.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	A simple dragger which creates a task dragging the text of the
//	current selection

#ifndef _H_LTableDragValue
#define _H_LTableDragValue
#pragma once

#include <UTextTableHelpers.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LTableDragValue : public LTableDragger {
public:
						LTableDragValue();

	virtual				~LTableDragValue();

	virtual void		CreateDragTask(
								LTableView*				fromTable,
								const SMouseDownEvent&	inMouseDown);
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
