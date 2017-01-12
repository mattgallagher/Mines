// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCFBoolean.cp				PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================
//
//	Wrapper class for Core Foundation Boolean data type


#include <LCFBoolean.h>


// ---------------------------------------------------------------------------
//	¥ LCFBoolean							Constructor				  [public]
// ---------------------------------------------------------------------------

LCFBoolean::LCFBoolean(
	Boolean		inValue)
{
	SetTypeRef( inValue ? kCFBooleanTrue : kCFBooleanFalse );
}


// ---------------------------------------------------------------------------
//	¥ LCFBoolean							Constructor				  [public]
// ---------------------------------------------------------------------------

LCFBoolean::LCFBoolean(
	bool		inValue)
{
	SetTypeRef( inValue ? kCFBooleanTrue : kCFBooleanFalse );
}


// ---------------------------------------------------------------------------
//	LCFBoolean								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from an existing CFBooleanRef
//
//	We retain the CFBooleanRef. Caller should release the CFBooleanRef if it
//	no longer needs it.

LCFBoolean::LCFBoolean(
	CFBooleanRef	inBooleanRef)

	: LCFObject(inBooleanRef)
{
}


// ---------------------------------------------------------------------------
//	¥ LCFBoolean							Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LCFBoolean::LCFBoolean(
	const LCFBoolean&	inSource)

	: LCFObject(inSource.GetTypeRef())
{
}


// ---------------------------------------------------------------------------
//	¥ operator =							Assignment Operator		  [public]
// ---------------------------------------------------------------------------

LCFBoolean&
LCFBoolean::operator = (
	const LCFBoolean&	inSource)
{
	SetTypeRef(inSource.GetTypeRef());

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ ~LCFBoolean							Destructor				  [public]
// ---------------------------------------------------------------------------

LCFBoolean::~LCFBoolean()
{
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LCFBoolean::SetValue(
	Boolean	inValue)
{
	LCFObject::AdoptTypeRef( inValue ? kCFBooleanTrue : kCFBooleanFalse );
}
