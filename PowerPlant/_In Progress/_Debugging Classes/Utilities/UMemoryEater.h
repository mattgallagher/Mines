// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UMemoryEater.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub.

#ifndef _H_UMemoryEater
#define _H_UMemoryEater
#pragma once

#include <PP_Debug.h>
#include <PP_DebugConstants.h>

#include <TArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LWindow;

class LCommander;

class UMemoryEater {
public:
				enum EMemoryType {
					memoryType_Handle		=	1,
					memoryType_Pointer		=	2
				};

	static	void			Eat();

	static	void			EatPointer(
									Size 				inBytesToEat);
	static	void			EatHandle(
									Size				inBytesToEat);

	static	void			DeleteMemoryLists();

	static	UInt32			GetPointerPoolSize();
	static	UInt32			GetHandlePoolSize();
	static	UInt32			GetTotalPoolSize();

	static	bool			SetParameters(
									LCommander*			inSuper,
									bool				inEatNow,
									ResIDT				inPPobID,
									PaneIDT				inEditID = EatMemoryDialog_EditAmount,
									PaneIDT				inRadioGroupID = EatMemoryDialog_RadioGroup,
									PaneIDT				inRadioHandleID = EatMemoryDialog_RadioHandle,
									PaneIDT				inRadioPtrID = EatMemoryDialog_RadioPtr);

	static	Size			GetMemorySize()
								{
									return sMemorySize;
								}
	static	EMemoryType		GetMemoryType()
								{
									return sMemoryType;
								}

	static	void			FillBlock(
									void*				inPtr,
									register Size		inPtrSize,
									register long		inValue);

protected:

	static	LWindow*		BuildWindow(
									const LCommander*	inSuper);


	static	TArray<Handle>*	sHandleList;
	static	TArray<Ptr>*	sPointerList;

	static	EMemoryType		sMemoryType;
	static	Size			sMemorySize;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_UMemoryEater
