// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSmallIconTable.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LSmallIconTable
#define _H_LSmallIconTable
#pragma once

#include <LTableView.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LSmallIconTable : public LTableView {
public:
	enum { class_ID = FOUR_CHAR_CODE('sitb') };

						LSmallIconTable( LStream* inStream );
						
	virtual				~LSmallIconTable();

protected:
	virtual void		DrawCell(
								const STableCell&	inCell,
								const Rect&			inLocalRect);
};


struct	SIconTableRec {
	Str31	name;
	ResIDT	iconID;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
