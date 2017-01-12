// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	URegions.h					PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_URegions
#define _H_URegions
#pragma	once

#include <PP_Prefix.h>
#include <UTBAccessors.h>
#include <Quickdraw.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	StRegion {
public:
											// ¥ Constructors
					StRegion ();

					StRegion (const Rect &inRect );

					StRegion (SInt16	inLeft,
							  SInt16	inTop,
							  SInt16	inRight,
							  SInt16	inBottom);

					StRegion (const RgnHandle	inRegionH,
							  bool  inMakeCopy = true);

					StRegion ( const StRegion &inStRegion );

					~StRegion();

											// ¥ Assignment Operators
	StRegion&		operator = ( const StRegion &inRhs);

	StRegion&		operator = ( RgnHandle inRegionH );

	StRegion&		operator = ( const Rect &inRect )
						{
							::RectRgn(mRegionH, &inRect);
							return *this;
						}

											// ¥ Coercion Operators
	operator		RgnHandle() const	{ return mRegionH; }

	operator		Handle() const		{ return (Handle) mRegionH; }


											// ¥ Accessors
	bool			IsEmpty() const	{ return ::EmptyRgn(mRegionH); }

	Rect			Bounds() const
						{
							Rect	theRect;
							::GetRegionBounds(mRegionH, &theRect);
							return theRect;
						}

	void			GetBounds( Rect &outRect ) const
						{
							::GetRegionBounds(mRegionH, &outRect);
						}

	bool			Contains( Point inPoint ) const
						{
							return ::PtInRgn(inPoint, mRegionH);
						}

	bool			IsOwner() const	{ return mIsOwner; }

											// ¥ Ownership
	RgnHandle		Release();

	void			Adopt(	RgnHandle		inRegionH);

											// ¥ Manipulators
	void			Clear()			{ ::SetEmptyRgn(mRegionH); }

	void			OffsetBy(
							SInt16		inHoriz,
							SInt16		inVert)
						{
							::MacOffsetRgn(mRegionH, inHoriz, inVert);
						}

	void			InsetBy(
							SInt16		inHoriz,
							SInt16		inVert)
						{
							::InsetRgn(mRegionH, inHoriz, inVert);
						}

											// ¥ Operators with Regions
	StRegion&		operator += ( RgnHandle inRegionH )
						{
							::MacUnionRgn(mRegionH, inRegionH, mRegionH);
							return *this;
						}

	StRegion&		operator -= ( RgnHandle inRegionH )
						{
							::DiffRgn(mRegionH, inRegionH, mRegionH);
							return *this;
						}

	StRegion&		operator &= ( RgnHandle inRegionH )
						{
							::SectRgn(mRegionH, inRegionH, mRegionH);
							return *this;
						}

	StRegion&		operator |= ( RgnHandle inRegionH )
						{
							::MacUnionRgn(mRegionH, inRegionH, mRegionH);
							return *this;
						}

	StRegion&		operator ^= ( RgnHandle inRegionH )
						{
							::MacXorRgn(mRegionH, inRegionH, mRegionH);
							return *this;
						}

	bool			operator == ( RgnHandle inRegionH ) const
						{
							return ::MacEqualRgn(mRegionH, inRegionH);
						}

	bool			operator != ( RgnHandle inRegionH ) const
						{
							return !::MacEqualRgn(mRegionH, inRegionH);
						}

											// ¥ Operators with Rectangles
	StRegion&		operator += ( const Rect &inRect )
						{
							StRegion	rgnH(inRect);
							::MacUnionRgn(mRegionH, rgnH, mRegionH);
							return *this;
						}

	StRegion&		operator -= ( const Rect &inRect )
						{
							StRegion	rgnH(inRect);
							::DiffRgn(mRegionH, rgnH, mRegionH);
							return *this;
						}

	StRegion&		operator &= ( const Rect &inRect )
						{
							StRegion	rgnH(inRect);
							::SectRgn(mRegionH, rgnH, mRegionH);
							return *this;
						}

	StRegion&		operator |= ( const Rect &inRect )
						{
							StRegion	rgnH(inRect);
							::MacUnionRgn(mRegionH, rgnH, mRegionH);
							return *this;
						}

	StRegion&		operator ^= ( const Rect &inRect )
						{
							StRegion	rgnH(inRect);
							::MacXorRgn(mRegionH, rgnH, mRegionH);
							return *this;
						}

	bool			operator == ( const Rect &inRect ) const
						{
							StRegion	rgnH(inRect);
							return ::MacEqualRgn(mRegionH, rgnH);
						}

	bool			operator != ( const Rect &inRect ) const
						{
							StRegion	rgnH(inRect);
							return !::MacEqualRgn(mRegionH, rgnH);
						}

protected:
	RgnHandle		mRegionH;
	bool			mIsOwner;
};

// ---------------------------------------------------------------------------

class	StRegionBuilder {
public:
						StRegionBuilder( StRegion &inRegion );
						
						StRegionBuilder( RgnHandle &ioRgnH );
						
						~StRegionBuilder();

	operator			RgnHandle() { return mRegionH; }

	RgnHandle			mRegionH;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
