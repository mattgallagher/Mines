// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTextArray.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	An ordered collection of variable-size text strings. Positions in the
//	Array are one-based--the first item is at index 1.

#ifndef _H_LTextArray
#define _H_LTextArray
#pragma once

#include <LVariableArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


class LTextArray : public LVariableArray {
public:
						LTextArray(
								LComparator		*inComparator = nil,
								Boolean			inKeepSorted = false);

						LTextArray(
								Handle			inItemsHandle,
								ArrayOffsetH	inOffsetsHandle,
								LComparator		*inComparator = nil,
								Boolean			inIsSorted = false,
								Boolean			inKeepSorted = false);

	virtual				~LTextArray();

	virtual void	AppendItem(
								const char		*inValue,	// C string
								UInt32			inItemSize);

	virtual void	AppendArray(
								const char		**inValues);	// array of C strings, last item empty

	virtual void	SetItemsDelimited(
								const char		*inStr,	// string of delimited strings, null terminated
								char				inDelimiter,
								UInt32		inAppendFrom=0);	// can replace strings from certain point

	virtual void DeleteAllContents();

	void	      operator+=(
								const char		*inString);

private:
	Boolean	mOwnsContents;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
