// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCFBoolean.h				PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================
//
//	Wrapper class for Core Foundation Boolean data type

#ifndef _H_LCFBoolean
#define _H_LCFBoolean
#pragma once

#include <LCFObject.h>
#include <CFNumber.h>

// ---------------------------------------------------------------------------

class	LCFBoolean : public LCFObject {
public:
						LCFBoolean( Boolean inValue );
						
						LCFBoolean( bool inValue );

						LCFBoolean( CFBooleanRef inBooleanRef );

						LCFBoolean( const LCFBoolean& inSource );

	LCFBoolean&			operator = (const LCFBoolean& inSource );

	virtual				~LCFBoolean();

						operator CFBooleanRef() const;

	CFBooleanRef		GetTypeRef() const;
	
	void				Adopt( CFBooleanRef inBooleanRef );

	Boolean				GetValue() const;
	
	void				SetValue( Boolean inValue );
};


// ===========================================================================
//	Inline Functions
// ===========================================================================


// ---------------------------------------------------------------------------
//	¥ operator CFBooleanRef											  [public]
// ---------------------------------------------------------------------------

inline
LCFBoolean::operator CFBooleanRef() const
{
	return GetTypeRef();
}


// ---------------------------------------------------------------------------
//	¥ GetTypeRef													  [public]
// ---------------------------------------------------------------------------

inline
CFBooleanRef
LCFBoolean::GetTypeRef() const
{
	return reinterpret_cast<CFBooleanRef>( LCFObject::GetTypeRef() );
}


// ---------------------------------------------------------------------------
//	¥ Adopt															  [public]
// ---------------------------------------------------------------------------

inline
void
LCFBoolean::Adopt(
	CFBooleanRef	inBooleanRef)
{
	LCFObject::AdoptTypeRef(inBooleanRef);
}


// ---------------------------------------------------------------------------
//	¥ GetValue														  [public]
// ---------------------------------------------------------------------------

inline
Boolean
LCFBoolean::GetValue() const
{
	return ::CFBooleanGetValue( GetTypeRef() );
}


#endif
