// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMTrackActionImp.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LAMTrackActionImp
#define _H_LAMTrackActionImp
#pragma once

#include <LAMControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LAMTrackActionImp : public LAMControlImp {
public:
						LAMTrackActionImp( LStream* inStream = nil );

	virtual void		Init(	LControlPane	*inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);

	virtual				~LAMTrackActionImp();

	virtual void		SetValue( SInt32 inValue );
	
	virtual void		SetMinValue( SInt32 inMinValue );
	
	virtual void		SetMaxValue( SInt32 inMaxValue );

	virtual Boolean		TrackHotSpot(
								SInt16			inHotSpot,
								Point			inPoint,
								SInt16			inModifiers);

	void				DoTrackAction( SInt16 inHotSpot );

	static pascal void	TrackAction(
								ControlHandle	inMacControl,
								SInt16			inPart);

protected:
	bool				mUsingBigValues;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
