// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGATitleMixin.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGATitleMixin.h>
#include <UDrawingState.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
// 	¥ LGATitleMixin							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LGATitleMixin::LGATitleMixin()
{
	mTitleHiliteColor = Color_Black;
	mTextTraitsID	  = 0;
	mHiliteTitle	  = false;
}


// ---------------------------------------------------------------------------
// 	¥ LGATitleMixin							Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LGATitleMixin::LGATitleMixin(
	const LGATitleMixin&	inOriginal)

	: mTitle(inOriginal.mTitle)
{
	mTitleHiliteColor = inOriginal.mTitleHiliteColor;
	mTextTraitsID     = inOriginal.mTextTraitsID;
	mHiliteTitle	  = inOriginal.mHiliteTitle;
}


// ---------------------------------------------------------------------------
// 	¥ ~LGATitleMixin						Destructor				  [public]
// ---------------------------------------------------------------------------

LGATitleMixin::~LGATitleMixin()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
// 	¥ SetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

void
LGATitleMixin::SetTextTraitsID(
	ResIDT	inTextTraitID)
{
	mTextTraitsID = inTextTraitID;
}


// ---------------------------------------------------------------------------
// 	¥ SetHiliteTitle												  [public]
// ---------------------------------------------------------------------------

void
LGATitleMixin::SetHiliteTitle(
	bool		inHiliteTitle)
{
	mHiliteTitle = inHiliteTitle;
}


// ---------------------------------------------------------------------------
// 	¥ SetTitleHiliteColor											  [public]
// ---------------------------------------------------------------------------

void
LGATitleMixin::SetTitleHiliteColor(
	RGBColor	inHiliteColor)
{
	mTitleHiliteColor = inHiliteColor;
}


PP_End_Namespace_PowerPlant
