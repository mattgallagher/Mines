// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableDropMsg.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	A simple drop zone which triggers a command when dropped

#ifndef _H_LTableDropMsg
#define _H_LTableDropMsg
#pragma once

#include <LTableDropZone.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant
	class	LListener;

class LTableDropMsg : public LTableDropZone {
public:
	LTableDropMsg(LTableView* target, CommandT dropMsg, LListener* tellDropped);

	virtual	~LTableDropMsg() {};

	virtual Boolean		ItemIsAcceptable( DragReference inDragRef,
							ItemReference inItemRef );

	virtual void		ReceiveDragItem( DragReference inDragRef,
							DragAttributes inDragAttrs, ItemReference inItemRef,
							Rect &inItemBounds );

private:
	CommandT	mDropOKmsg;
	LListener*	mTellDropped;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
