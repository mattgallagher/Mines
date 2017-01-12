// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGACheckBoxImp.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGACheckBoxImp
#define _H_LGACheckBoxImp
#pragma once

#include <LGATitleMixin.h>
#include <LGAControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGACheckBoxImp : public LGAControlImp,
					   public LGATitleMixin {
public:
						LGACheckBoxImp(
								LControlPane*	inControlPane);

						LGACheckBoxImp(
								LStream*		inStream);

	virtual				~LGACheckBoxImp();

	virtual void		Init(	LControlPane*	inControlPane,
								LStream*		inStream);

	virtual	void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0 );


							// Getters

	bool				IsSelected() const
							{
								return mControlPane->GetValue() == Button_On;
							}

	virtual StringPtr	GetDescriptor(
								Str255			outDescriptor) const;

	virtual	void		CalcTitleRect(
								Rect&			outRect);

	virtual	void		CalcLocalControlRect(
								Rect&			outRect);

	bool				IsMixedState() const
							{
								return mControlPane->GetValue() == Button_Mixed;
							}

	virtual	ResIDT		GetTextTraitsID() const
							{
								return LGATitleMixin::GetTextTraitsID();
							}

							// Setters

	virtual void		SetDescriptor(
								ConstStringPtr	inDescriptor);

	virtual void		SetTextTraitsID(
								ResIDT			inTextTraitsID);

	virtual	void		PostSetValue();

	virtual	void		SetSetClassicCheckMark(
								Boolean			inClassicMark);


							// Drawing

	virtual	void		DrawSelf();

	virtual	void		DrawCheckBoxTitle();

	virtual	void		DrawCheckBoxNormal();
	virtual	void		DrawCheckBoxHilited();
	virtual	void		DrawCheckBoxDimmed();


							// Best Rectangle

	virtual	Boolean		SupportsCalcBestRect() const	{ return true; }

	virtual	void		CalcBestControlRect	(
								SInt16&			outWidth,
								SInt16&			outHeight,
								SInt16&			outBaselineOffset) const;

protected:
	Boolean		mClassicCheckMark;			//	True if the checkmark is the classic
											//		'x' style mark
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
