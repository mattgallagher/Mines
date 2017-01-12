// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInternetMessageList.h		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	An ordered collection of items. Item positions in the list are
//	one-based--the first item is at index 1.
//
//	This List is a DynamicArray which can be associated with a ListIterator.

#ifndef _H_LInternetMessageList
#define _H_LInternetMessageList
#pragma once

#include <LArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LInternetMessageList : public LArray {
public:
					LInternetMessageList();
	virtual			~LInternetMessageList();

	virtual void	ResetList();

	virtual void	SetItemAt(
							ArrayIndexT		inAtIndex,
							const void*		inItem);

	virtual void	AppendItem( const void* inItem );

	virtual void	RemoveItem( ArrayIndexT inAtIndex );
};

class	LInternetMessageListIterator;

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
