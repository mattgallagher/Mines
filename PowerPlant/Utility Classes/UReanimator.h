// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UReanimator.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UReanimator
#define _H_UReanimator
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LStream;
class	LListener;
class	LView;
class	LCommander;

// ---------------------------------------------------------------------------

class	UReanimator {
public:

	template <class T> 
	static T*		ReanimateObjects(
							OSType		inResType,
							ResIDT		inResID);

	static void*	ReadObjects(
							OSType		inResType,
							ResIDT		inResID);

	static void*	ObjectsFromStream(
							LStream*	inStream);

	static void		LinkListenerToBroadcasters(
							LListener*	inListener,
							LView*		inControlContainer,
							ResIDT		inResListID);

	static void		LinkListenerToControls(
							LListener*	inListener,
							LView*		inControlContainer,
							ResIDT		inResListID);

	static LView*	CreateView(
							ResIDT		inViewID,
							LView*		inSuperView,
							LCommander*	inSuperCommander);
							
private:
	static bool		sConstructionFailed;
};


// ---------------------------------------------------------------------------
//	¥ ReanimateObjects					Template Member Function
// ---------------------------------------------------------------------------
//	Type-safe wrapper for creating objects from resource data
//
//	Returns nil if there is a failure in creating *any* object
//
//	For "T", specify the type of the first object, usually the top-level
//	object in a hierarchy. For example, to create a LWindow:
//
//		LWindow* theWindow = UReanimator::ReanimateObjects<LWindow>(
//												ResType_PPob,
//												kMyWindowID );

template <class T> T*
UReanimator::ReanimateObjects(
	OSType		inResType,
	ResIDT		inResID)
{
	T* theObject = static_cast<T*>( ReadObjects(inResType, inResID) );
	
	if (sConstructionFailed) {		// Construction failed for some object
		delete theObject;			// Delete partial object hierarchy
		theObject = nil;
	}
	
	return theObject;
}


PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
