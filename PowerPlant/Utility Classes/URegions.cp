// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	URegions.cp					PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <URegions.h>
#include <UDrawingState.h>

PP_Begin_Namespace_PowerPlant


// ===========================================================================
//	¥ StRegion
// ===========================================================================
//	Wrapper class for a QuickDraw Region
//
//	Constructors create a region handle and the destructor disposes of it

// ---------------------------------------------------------------------------
//	¥ StRegion								Default Constructor		  [public]
// ---------------------------------------------------------------------------

StRegion::StRegion()
{
	mRegionH = ::NewRgn();				// Create a new empty region
	ThrowIfNil_(mRegionH);
	mIsOwner = true;
}


// ---------------------------------------------------------------------------
//	¥ StRegion								Constructor				  [public]
// ---------------------------------------------------------------------------

StRegion::StRegion(
	const Rect	&inRect)
{
	mRegionH = ::NewRgn();
	ThrowIfNil_(mRegionH);
	mIsOwner = true;

	::RectRgn(mRegionH, &inRect);		// Create a rectangular region
}


// ---------------------------------------------------------------------------
//	¥ StRegion								Constructor				  [public]
// ---------------------------------------------------------------------------

StRegion::StRegion(
	SInt16		inLeft,
	SInt16		inTop,
	SInt16		inRight,
	SInt16		inBottom)
{
	mRegionH = ::NewRgn();
	ThrowIfNil_(mRegionH);
	mIsOwner = true;
										// Create rectangular region
	::MacSetRectRgn(mRegionH, inLeft, inTop, inRight, inBottom);
}


// ---------------------------------------------------------------------------
//	¥ StRegion								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from an existing RgnHandle
//
//	If inMakeCopy is true (default), object makes a copy of the input region
//	If false, object takes ownership of the input region.

StRegion::StRegion(
	const RgnHandle	inRegionH,
	bool			inMakeCopy)
{
	if (inMakeCopy || (inRegionH == nil)) {

		mRegionH = ::NewRgn();
		ThrowIfNil_(mRegionH);

		if (inRegionH != nil) {
			::MacCopyRgn(inRegionH, mRegionH);

			OSErr	err = ::QDError();
			if (err != noErr) {				// Copy failed. Dispose our
				::DisposeRgn(mRegionH);		//   half-baked region and
				Throw_(err);				//   throw an exception
			}
		}

	} else {
		mRegionH = inRegionH;
	}

	mIsOwner = true;
}


// ---------------------------------------------------------------------------
//	¥ StRegion								Copy Constructor		  [public]
// ---------------------------------------------------------------------------

StRegion::StRegion(
	const StRegion	&inStRegion)
{
	mRegionH = ::NewRgn();
	ThrowIfNil_(mRegionH);
	mIsOwner = true;

	::MacCopyRgn(inStRegion.mRegionH, mRegionH);

	OSErr	err = ::QDError();

	if (err != noErr) {						// Copy failed. Dispose our
		::DisposeRgn(mRegionH);				//   half-baked region and
		Throw_(err);						//   throw and exception
	}
}


// ---------------------------------------------------------------------------
//	¥ ~StRegion								Destructor				  [public]
// ---------------------------------------------------------------------------

StRegion::~StRegion()
{
	if (mIsOwner && (mRegionH != nil)) {	// Dispose region if we own it
		::DisposeRgn(mRegionH);
	}
}


// ---------------------------------------------------------------------------
//	¥ StRegion								Assignment Operator		  [public]
// ---------------------------------------------------------------------------

StRegion&
StRegion::operator = (
	const StRegion	&inRhs)
{
	if (this != &inRhs) {				// Check for self assignment
		::MacCopyRgn(inRhs.mRegionH, mRegionH);
		ThrowIfQDError_();
	}

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ StRegion								Assignment to Region	  [public]
// ---------------------------------------------------------------------------
//	Object makes a copy of the input region. Use the Adopt() function if
//	you want the object to take ownership of a region (and not copy it).

StRegion&
StRegion::operator = (
	RgnHandle	inRegionH)
{
	if (inRegionH == nil) {				// Setting equal to nil makes this
		::SetEmptyRgn(mRegionH);		//   an empty region

	} else {
		::MacCopyRgn(inRegionH, mRegionH);
		ThrowIfQDError_();
	}

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ Release														  [public]
// ---------------------------------------------------------------------------
//	Return handle to the Region. Caller takes ownership of the RgnHandle,
//	but this object still stores the RgnHandle so you can continue to use
//	this object to manipulate the Region. But this object won't dispose of
//	the Region when it is deleted.

RgnHandle
StRegion::Release()
{
	mIsOwner = false;
	return mRegionH;
}


// ---------------------------------------------------------------------------
//	¥ Adopt															  [public]
// ---------------------------------------------------------------------------
//	This object takes ownership of the input region, and deletes its
//	existing region (if it owns it).
//
//	If input region is nil, object creates a new empty region.

void
StRegion::Adopt(
	RgnHandle	inRegionH)
{
	if (inRegionH != mRegionH) {
		if (mIsOwner) {					// Dispose of our existing region
			::DisposeRgn(mRegionH);
		}

		mRegionH = inRegionH;			// Take ownership of input region
		mIsOwner = true;

		if (inRegionH == nil) {			// Allocate new region if necessary
			mRegionH = ::NewRgn();
			ThrowIfNil_(mRegionH);
		}
	}
}


#pragma mark -

// ===========================================================================
//	¥ StRegionBuilder
// ===========================================================================
//	Constructor opens a region (subsequent QD calls build the region) and
//	the Destructor closes it
//
// 	Usage:
//		You can pass in an existing RgnHandle or let StRegionBuilder allocate
//	a new region by passing nil. Frequently, you will use StRegion to manage
//	the region:
//
//		StRegion			myRegion;
//		{
//			StBuilderRegion		builder(myRegion);
//			// ... Make QuickDraw calls to build region
//
//		}	// ... Build process ends upon leaving this scope
//
//		// ... Now, myRegion is ready to use and it will be deleted
//		// ... upon leaving this scope
//
//	Or, if you need the region beyond the scope of a single function,
//	you can let StBuilderRegion create a new region:
//
//		RgnHandle	myRegion = nil;
//		{
//			StBuilderRegion		builder(myRegion);
//			// ... Make QuickDraw calls to build region
//
//		}	// ... Build process ends up leaving this scope
//
//		// ... Now, myRegion is ready to use. You own the RgnHandle
//		// ... and are responsible for deleting it (or not).


StRegionBuilder::StRegionBuilder(
	StRegion	&inRegion)
{
	GrafPtr	currPort = UQDGlobals::GetCurrentPort();

	if (currPort == nil) {
		Throw_(paramErr);			// Yikes! No current port

	} else {						// Check if we are already building
									//   a region
		#if ACCESSOR_CALLS_ARE_FUNCTIONS

			if (::IsPortRegionBeingDefined((CGrafPtr) currPort)) {
				Throw_(paramErr);
			}

		#else

			if (currPort->rgnSave != nil) {
				Throw_(paramErr);
			}

		#endif
	}

	::OpenRgn();
	ThrowIfQDError_();

	mRegionH = inRegion;
}


StRegionBuilder::StRegionBuilder(
	RgnHandle	&ioRgnH)				// Region to build. If nil, this
{										//   routine will create a new region
	mRegionH = nil;

	GrafPtr	currPort = UQDGlobals::GetCurrentPort();

	if (currPort == nil) {
		Throw_(paramErr);			// Yikes! No current port

	} else {						// Check if we are already building
									//   a region
		#if ACCESSOR_CALLS_ARE_FUNCTIONS

			if (::IsPortRegionBeingDefined((CGrafPtr) currPort)) {
				Throw_(paramErr);
			}

		#else

			if (currPort->rgnSave != nil) {
				Throw_(paramErr);
			}

		#endif
	}

	if (ioRgnH == nil) {				// Caller did not allocate a region
		ioRgnH = ::NewRgn();
		ThrowIfNil_(ioRgnH);
	}

	::OpenRgn();
	ThrowIfQDError_();

	mRegionH = ioRgnH;
}


StRegionBuilder::~StRegionBuilder()
{
	if (mRegionH != nil) {
		::CloseRgn(mRegionH);
	}
}

PP_End_Namespace_PowerPlant

