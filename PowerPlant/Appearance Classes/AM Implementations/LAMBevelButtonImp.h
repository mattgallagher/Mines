// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMBevelButtonImp.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LAMBevelButtonImp
#define _H_LAMBevelButtonImp
#pragma once

#include <LAMControlImp.h>
#include <UMemoryMgr.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LAMBevelButtonImp : public LAMControlImp {
public:
						LAMBevelButtonImp( LStream* inStream = nil );

	virtual				~LAMBevelButtonImp();

	virtual void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);

	virtual void		SetValue( SInt32 inValue );

	virtual void		SetDataTag(
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inDataSize,
								void*			inDataPtr);

	virtual Boolean		TrackHotSpot(
								SInt16			inHotSpot,
								Point			inPoint,
								SInt16			inModifiers);

	virtual void		DrawSelf();

protected:
	MenuHandle			GetMacMenuH() const;
	
	SInt16				GetMenuValue() const;
	
private:
	StHandleBlock		mPictHandle;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
