// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAStaticTextImp.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGAStaticTextImp
#define _H_LGAStaticTextImp
#pragma once

#include <LGATitleMixin.h>
#include <LGAControlImp.h>
#include <UGraphicUtils.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LGAStaticTextImp : 	public LGAControlImp,
							public LGATitleMixin {
public:
						LGAStaticTextImp(
								LControlPane*		inControlPane);

						LGAStaticTextImp(
								LStream*			inStream);

	virtual				~LGAStaticTextImp();

	virtual void		Init(	LControlPane*		inControlPane,
								LStream*			inStream);

	virtual	void		Init(	LControlPane*		inControlPane,
								SInt16				inControlKind,
								ConstStringPtr		inTitle = Str_Empty,
								ResIDT				inTextTraitsID = 0,
								SInt32				inRefCon = 0 );

	virtual StringPtr	GetDescriptor(
								Str255				outDescriptor) const;

	virtual	void		CalcTitleRect(
								Rect&				outRect);

	virtual	ResIDT		GetTextTraitsID() const
							{
								return LGATitleMixin::GetTextTraitsID();
							}

	virtual void		SetDescriptor(
								ConstStringPtr		inDescriptor);

	virtual void		SetTextTraitsID(
								ResIDT				inTextTraitsID);

	virtual	void		SetValue(
								SInt32				inValue);

	virtual	bool		GetValue(
								SInt32&				outValue) const;

	virtual	Boolean		SupportsCalcBestRect() const;

	virtual	void		CalcBestControlRect	(
								SInt16&				outWidth,
								SInt16&				outHeight,
								SInt16&				outBaselineOffset) const;

	virtual	void		SetDataTag(
								SInt16				inPartCode,
								FourCharCode		inTag,
								Size				inDataSize,
								void*				inDataPtr );

	virtual	void		GetDataTag(
								SInt16				inPartCode,
								FourCharCode		inTag,
								Size				inBufferSize,
								void*				inBuffer,
								Size*				outDataSize = nil) const;

	virtual	void		DrawSelf();

	virtual Boolean		TrackHotSpot(
								SInt16			inHotSpot,
								Point			inPoint,
								SInt16			inModifiers);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif


