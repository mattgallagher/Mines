// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTextMultiArray.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author:  Andy Dent
//
//	An ordered collection of LTextArrays. Positions in the Array are
//	one-based--the first item is at index 1.

// The Append methods read tab-delimited text into multiple cells

#ifndef _H_LTextMultiArray
#define _H_LTextMultiArray
#pragma once

#include <LVariableArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LTextMultiArray : public LVariableArray {
public:
						LTextMultiArray(
								LComparator		*inComparator = nil,
								Boolean			inKeepSorted = false);

						LTextMultiArray(
								Handle			inItemsHandle,
								ArrayOffsetH	inOffsetsHandle,
								LComparator		*inComparator = nil,
								Boolean			inIsSorted = false,
								Boolean			inKeepSorted = false);

	virtual			~LTextMultiArray();

	virtual void	AppendItem(
								const char		*inValue,	// C string
								UInt32			inItemSize);

	virtual void	AppendArray(
								const char		**inValues);	// array of C strings, last item empty

	void	      operator+=(
								const char		*inString);
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
