// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAProgressBarImp.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub

#ifndef _H_LGAProgressBarImp
#define _H_LGAProgressBarImp
#pragma once

#include <LGAControlImp.h>
#include <UGWorld.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LGAProgressBarImp : public LGAControlImp {
public:
						LGAProgressBarImp( LControlPane* inControlPane );

						LGAProgressBarImp( LStream* inStream );

	virtual				~LGAProgressBarImp();

	virtual void		Init(	LControlPane*	inControlPane,
								LStream*		inStream);

	virtual void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);

	virtual void		AllocateGWorlds();
	
	static	void		DisposeGWorlds();

	virtual void		PostSetValue();
	
	virtual void		SetMinValue( SInt32 inMinValue );
	
	virtual void		SetMaxValue( SInt32 inMaxValue );

	virtual void		DrawSelf();

	virtual void		SetDataTag(
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inDataSize,
								void*			inDataPtr);

	virtual void		GetDataTag(
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inBufferSize,
								void*			inBuffer,
								Size			*outDataSize = nil) const;

	virtual	void		Idle();

protected:

	static	LGWorld*	sProgBarGWorld;
	static	LGWorld*	sDisabledProgBarGWorld;
	static	LGWorld*	sProgBarGWorldBW;
	static	LGWorld*	sDisabledProgBarGWorldBW;

	SInt16				mCurrentFrame;
	bool				mIsIndeterminant;
	SInt16				mFillWidth;
	SInt16				mLeftBarEdge;
	SInt16				mRightBarEdge;
	SInt16				mRightFillEdge;
	UInt32				mLastTick;

	virtual	void		DrawBorder( Rect& ioFrame );

	virtual	void		DrawLeftEnd( const Rect& inFrame );

	virtual	void		DrawRightEnd( const Rect& inFrame );

	virtual	void		DrawFill( const Rect& inFrame );

	virtual	void		DrawBackground( const Rect& inFrame );

	virtual	void		CalcLocalControlRect( Rect& outRect );

private:
						LGAProgressBarImp();
						LGAProgressBarImp( const LGAProgressBarImp& );
	LGAProgressBarImp&	operator=( const LGAProgressBarImp& );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
