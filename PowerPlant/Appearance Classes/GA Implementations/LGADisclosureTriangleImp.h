// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGADisclosureTriangleImp.h	PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGADisclosureTriangleImp
#define _H_LGADisclosureTriangleImp
#pragma once

#include <LGAControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGADisclosureTriangleImp : public LGAControlImp {
public:
						LGADisclosureTriangleImp(
								LControlPane*	inControlPane );

						LGADisclosureTriangleImp (
								LStream *inStream );

	virtual				~LGADisclosureTriangleImp ();


	virtual void		Init(	LControlPane*	inControlPane,
								LStream*		inStream );

	virtual	void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0 );

							// Getters

	bool				IsSelected() const
									{
										return mControlPane->GetValue () > 0;
									}

	bool				IsLeftDisclosure () const
									{
										return mLeftDisclosure;
									}

	bool				HasAutoToggle () const
									{
										return mAutoToggle;
									}

	virtual	void		CalcLocalControlRect	(
												Rect	&outRect );


							// Setters

	virtual	void		SetValue(
							SInt32	inValue );

	virtual	void		PostSetValue ();

	virtual	void		SetPushedState(
								Boolean	inPushedState );


							// Data Handling

	virtual	void		SetDataTag (
								SInt16				inPartCode,
								FourCharCode	inTag,
								Size				inDataSize,
								void*				inDataPtr );

	virtual	void		GetDataTag (
								SInt16				inPartCode,
								FourCharCode	inTag,
								Size				inBufferSize,
								void*				inBuffer,
								Size				*outDataSize = nil ) const;

							// Drawing

	virtual	void		DrawSelf();

	virtual	void		DrawIntermediateState();
	virtual	void		WipeBackground();


protected:

	bool		mLeftDisclosure;		//	Left or right facing triangle?
	bool		mAutoToggle;			//	Toggle when clicked?


	virtual	void		PlotPixelMapChoice(
							SInt16	inChoice );


							// Mouse Tracking

	virtual	void		HotSpotAction(
								SInt16		inHotSpot,
								Boolean	inCurrInside,
								Boolean	inPrevInside );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
