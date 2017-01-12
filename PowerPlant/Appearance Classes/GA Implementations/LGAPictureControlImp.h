// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAPictureControlImp.h		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub

#ifndef _H_LGAPictureControlImp
#define _H_LGAPictureControlImp
#pragma once

#include <LGAControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LGAPictureControlImp : public LGAControlImp {
public:
						LGAPictureControlImp(
								LControlPane*	inControlPane,
								ResIDT			inPICTid);

						LGAPictureControlImp( LStream* inStream );

	virtual				~LGAPictureControlImp();


	virtual void		Init(	LControlPane*	inControlPane,
								LStream*		inStream);

	virtual void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);


	virtual void		DrawSelf();

	virtual	void		DoneTracking(
								SInt16		inHotSpot,
								Boolean		inGoodTrack);

protected:
			ResIDT		mPICTid;
			bool		mTrack;

private:
						LGAPictureControlImp();
						LGAPictureControlImp( const LGAPictureControlImp& );
	LGAPictureControlImp&	operator = ( const LGAPictureControlImp& );

};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
