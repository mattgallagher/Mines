// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAGroupBoxImp.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGAGroupBoxImp
#define _H_LGAGroupBoxImp
#pragma once

#include <LGAControlImp.h>
#include <LControlPane.h>
#include <LString.h>
#include <LView.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGAGroupBoxImp : public LGAControlImp {
public:
						LGAGroupBoxImp(
								LControlPane*		inControlPane);

						LGAGroupBoxImp(
								LStream*			inStream);

	virtual				~LGAGroupBoxImp();

	virtual	StringPtr	GetDescriptor(
								Str255				outTitle) const;

	LControlPane*		GetHeaderObject() const		{ return mHeaderObject; }

	bool				HasPrimaryBorder() const	{ return mPrimaryBorder; }

	virtual	RgnHandle	GetBoxBorderRegion(
								const Rect&			inRevealed);

	virtual	RgnHandle	GetBoxContentRegion(
								const Rect&			inRevealed);

	virtual	void		CalcHeaderRect(
								Rect&				outRect);

	virtual	void		CalcContentRect(
								Rect&				outRect);

	virtual	void		CalcBorderRect(
								Rect&				outRect);

	bool				HasHeader() const	{ return mHeaderObject != nil; }

	virtual	void		SetDescriptor(
								ConstStringPtr		inDescriptor);

	virtual void		SetTextTraitsID(
								ResIDT				inTextTraitsID);


	virtual	void		DrawSelf();

protected:
	bool			mPrimaryBorder;		// Primary or secondary border?

	LControlPane*	mHeaderObject;		//	Reference to actual object that represents the
										//		header for the group box
	SInt16			mBaseline;			//	This is the distance from the top of the group
										//		box to the baseline for any text that would
										//		be rendered in the header, it is used to
										//		correctly position the border in relation
										//		to the header object

	virtual	void		DrawPrimaryBoxBorder();

	virtual	void		DrawSecondaryBoxBorder();

	virtual	void		ResizeToBestRect(
								SInt16&		outBaselineOffset,
								Boolean		inRefresh);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
