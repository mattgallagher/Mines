// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LLink.h						PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde

#ifndef _H_LLink
#define _H_LLink
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif


PP_Begin_Namespace_PowerPlant


// ===========================================================================
// ¥ Forward declarations
// ===========================================================================


// forward class declarations
class	LLink;


// ===========================================================================
// ¥ class LLink
// ===========================================================================


class LLink
{
public:

	// constructors / destructors
				LLink();
				LLink(LLink* inLinkP);
	virtual		~LLink();

	// link management
	void		SetLink(LLink* inLinkP);
	LLink*		GetLink() const;

protected:

	LLink*		mLink;		// pointer to next element
};


// ===========================================================================
// ¥ Inline Functions
// ===========================================================================


// ---------------------------------------------------------------------------
//	¥ LLink()
// ---------------------------------------------------------------------------
//	Default constructor.

inline
LLink::LLink()
	: mLink(0)
{
	// there's nothing else to do
}


// ---------------------------------------------------------------------------
//	¥ LLink(LLink *inLinkP)
// ---------------------------------------------------------------------------
//	Constructor which initialises the link with the given pointer.

inline
LLink::LLink(LLink* inLinkP)
	: mLink(inLinkP)
{
	// there's nothing else to do
}


// ---------------------------------------------------------------------------
//	¥ SetLink
// ---------------------------------------------------------------------------
//	Changes the link field.

inline void
LLink::SetLink(LLink* inLinkP)
{
	mLink = inLinkP;
}


// ---------------------------------------------------------------------------
//	¥ GetLink
// ---------------------------------------------------------------------------
//	Returns the contents of the link field.

inline LLink*
LLink::GetLink() const
{
	return (mLink);
}

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_LLink
