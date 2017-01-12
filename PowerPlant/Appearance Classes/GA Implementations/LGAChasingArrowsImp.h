// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAChasingArrowsImp.h		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub

#ifndef _H_LGAChasingArrowsImp
#define _H_LGAChasingArrowsImp
#pragma once

#include <LGAControlImp.h>
#include <UGWorld.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LGAChasingArrowsImp : public LGAControlImp {
public:
						LGAChasingArrowsImp( LControlPane* inControlPane );

						LGAChasingArrowsImp( LStream* inStream );

	virtual				~LGAChasingArrowsImp();


	virtual void		Init(	LControlPane*	inControlPane,
								LStream*		inStream);

	virtual void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);

	virtual	void		AllocateGWorlds();
	static	void		DisposeGWorlds();

	virtual void		DrawSelf();

	virtual	void		Idle();

protected:
	static	LGWorld		*sArrowGWorlds[];
			SInt16		mCurrentFrame;

	virtual	void		CalcLocalControlRect( Rect &outRect );


private:
						LGAChasingArrowsImp();
						LGAChasingArrowsImp( const LGAChasingArrowsImp& );
	LGAChasingArrowsImp& operator=( const LGAChasingArrowsImp& );

};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
