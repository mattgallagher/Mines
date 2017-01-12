// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LComparator.cp				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Comparators are objects that know how to compare to other objects
//	or structures.  Subclasses will need to implement the Compare()
//	method. The compare Compare() result should be one of the following:
//
//		< 0		object/data item 1 is less than object/data item 2
//		0		object/data item 1 is equal to object/data item 2
//		> 0		object/data item 1 is greater than object/data item 2

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LComparator.h>
#include <UMemoryMgr.h>

PP_Begin_Namespace_PowerPlant


// ===========================================================================
//	¥ LComparator
// ===========================================================================
//	Compares items byte by byte

LComparator*	LComparator::sComparator = nil;		// static class variable

LComparator::LComparator()
{
}


LComparator::~LComparator()
{
	if (sComparator == this) {
		sComparator = nil;
	}
}


SInt32
LComparator::Compare(
	const void*		inItemOne,
	const void*		inItemTwo,
	UInt32			inSizeOne,
	UInt32			inSizeTwo) const
{
	return BlockCompare(inItemOne, inItemTwo, inSizeOne, inSizeTwo);
}


Boolean
LComparator::IsEqualTo(
	const void*		inItemOne,
	const void*		inItemTwo,
	UInt32			inSizeOne,
	UInt32			inSizeTwo) const
{
	return (Compare(inItemOne, inItemTwo, inSizeOne, inSizeTwo) == 0);
}


SInt32
LComparator::CompareToKey(
	const void*		/* inItem */,
	UInt32			/* inSize */,
	const void*		/* inKey */) const
{
	SignalStringLiteral_("CompareToKey not implemented");

	return 1;
}


Boolean
LComparator::IsEqualToKey(
	const void*		inItem,
	UInt32			inSize,
	const void*		inKey) const
{
	return (CompareToKey(inItem, inSize, inKey) == 0);
}


LComparator*
LComparator::GetComparator()
{
	if (sComparator == nil) {
		sComparator = new LComparator;
	}

	return sComparator;
}


LComparator*
LComparator::Clone()
{
	return new LComparator;
}

#pragma mark -

// ===========================================================================
//	¥ LLongComparator
// ===========================================================================
//	Compares items as long integer values

LLongComparator*	LLongComparator::sLongComparator = nil;

LLongComparator::LLongComparator()
{
}


LLongComparator::~LLongComparator()
{
	if (sLongComparator == this) {
		sLongComparator = nil;
	}
}


SInt32
LLongComparator::Compare(
	const void*		inItemOne,
	const void*		inItemTwo,
	UInt32			/* inSizeOne */,
	UInt32			/* inSizeTwo */) const
{
	return ( (*(long*) inItemOne) - (*(long*) inItemTwo) );
}


Boolean
LLongComparator::IsEqualTo(
	const void*		inItemOne,
	const void*		inItemTwo,
	UInt32			/* inSizeOne */,
	UInt32			/* inSizeTwo */) const
{
	return ( (*(long*) inItemOne) == (*(long*) inItemTwo) );
}


LLongComparator*
LLongComparator::GetComparator()
{
	if (sLongComparator == nil) {
		sLongComparator = new LLongComparator;
	}

	return sLongComparator;
}


LComparator*
LLongComparator::Clone()
{
	return new LLongComparator;
}

PP_End_Namespace_PowerPlant
