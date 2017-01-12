// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableDropText.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author:  Andy Dent
//
//	A simple drop zone which appends to the table and highlights the
//	entire table

#ifndef _H_LTableDropText
#define _H_LTableDropText
#pragma once

#include <LTableDropZone.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LTableDropText : public LTableDropZone {
public:
						LTableDropText(
								LTableView*		inTableView);

	virtual				~LTableDropText();

	virtual void		ReceiveDragItem(
								DragReference	inDragRef,
								DragAttributes	inDragAttrs,
								ItemReference	inItemRef,
								Rect&			inItemBounds);

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
