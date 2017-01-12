// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableDropZone.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	Base class for simple drop zone which highlights the entire table
//  subclasses implement the receive action
//
//	In the simpler implementations, being an Attachment is just so the pane
//	owns us and will delete us
//
//	More complex versions could use messages received to vary the
//	highlighting, change the cursor etc. to reflect being over a drop area

#ifndef _H_LTableDropZone
#define _H_LTableDropZone
#pragma once

#include <LAttachment.h>
#include <LDragAndDrop.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant
	class LTableView;

class LTableDropZone : public LAttachment, public LDragAndDrop {
public:
	LTableDropZone(LTableView*, FlavorType);

	virtual	~LTableDropZone() {};

	virtual Boolean		ItemIsAcceptable( DragReference inDragRef,
							ItemReference inItemRef );

protected:
	LTableView	*mTargetTable;
	FlavorType	mSingleFlavor;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
