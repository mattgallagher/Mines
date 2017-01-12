// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTextColumn.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LTextColumn
#define _H_LTextColumn
#pragma once

#include <LColumnView.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LTextColumn : public LColumnView {
public:
	enum				{ class_ID = FOUR_CHAR_CODE('txcl') };

						LTextColumn( LStream* inStream );

	virtual				~LTextColumn();


protected:
	virtual void		DrawCell(
								const STableCell&	inCell,
								const Rect&			inLocalRect);

protected:
	ResIDT			mTxtrID;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
