// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UTables.h				PowerPlant 2.2.2		©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UTables
#define _H_UTables
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

typedef	UInt32	TableIndexT;			// Row/Column index


// ---------------------------------------------------------------------------
//	¥ STableCell												  STableCell ¥
// ---------------------------------------------------------------------------
//	A cell, specified by row and column index, in a two-dimensional table

class STableCell {
public:
	TableIndexT	row;
	TableIndexT	col;

					STableCell(
							TableIndexT			inRow = 0,
							TableIndexT			inCol = 0)
					{
						row = inRow;
						col = inCol;
					}

					STableCell(
							Point				inCellPoint)
					{
						row = inCellPoint.v;
						col = inCellPoint.h;
					}

	void			SetCell(
							TableIndexT			inRow,
							TableIndexT			inCol)
					{
						row = inRow;
						col = inCol;
					}

	Boolean			IsNullCell() const
					{
						return ((row == 0) && (col == 0));
					}

	void			ToPoint(
							Point				&outPoint) const
					{
						outPoint.h = (SInt16) col;
						outPoint.v = (SInt16) row;
					}

	Boolean			operator ==(
							const STableCell	&inCell) const
					{
						return ((row == inCell.row) && (col == inCell.col));
					}

	Boolean			operator !=(
							const STableCell	&inCell) const
					{
						return ((row != inCell.row) || (col != inCell.col));
					}

	Boolean			operator >(
							const STableCell	&inCell) const
					{
						return ( (row > inCell.row) ||
								((row == inCell.row) && (col > inCell.col)) );
					}

	Boolean			operator >=(
							const STableCell	&inCell) const
					{
						return ( (row > inCell.row) ||
								((row == inCell.row) && (col >= inCell.col)) );
					}

	Boolean			operator <(
							const STableCell	&inCell) const
					{
						return ( (row < inCell.row) ||
								((row == inCell.row) && (col < inCell.col)) );
					}

	Boolean			operator <=(
							const STableCell	&inCell) const
					{
						return ( (row < inCell.row) ||
								((row == inCell.row) && (col <= inCell.col)) );
					}
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
