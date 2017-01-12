// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LStdScrollBarImp.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LStdScrollBarImp
#define _H_LStdScrollBarImp
#pragma once

#include <LStdControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LStdScrollBarImp : public LStdControlImp {
public:
						LStdScrollBarImp( LStream* inStream = nil );

	virtual void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);

	virtual				~LStdScrollBarImp();

	virtual void		SetValue( SInt32 inValue );

	virtual void		SetMinValue( SInt32 inMinValue );

	virtual void		SetMaxValue( SInt32 inMaxValue );

	virtual Boolean		TrackHotSpot(
								SInt16			inHotSpot,
								Point			inPoint,
								SInt16			inModifiers);

	static pascal void	TrackAction(
								ControlHandle	inMacControlH,
								SInt16			inPart);

protected:
	bool				mUsingBigValues;
	bool				mLiveFeedback;

	void				TrackLiveThumb( Point inPoint );

	SInt16				CalcThumbEdge( bool inHorizontal ) const;

	void				DoTrackAction( SInt16 inHotSpot );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
