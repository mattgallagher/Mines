// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAPopupGroupBoxImp.h		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGAPopupGroupBoxImp
#define _H_LGAPopupGroupBoxImp
#pragma once

#include <LGAGroupBoxImp.h>
#include <LControlPane.h>
#include <LListener.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGAPopupGroupBoxImp : public LGAGroupBoxImp,
							public LListener {
public:
						LGAPopupGroupBoxImp( LControlPane* inControlPane );

						LGAPopupGroupBoxImp( LStream* inStream );

	virtual				~LGAPopupGroupBoxImp();

	virtual	void		Init(	LControlPane*	inControlPane,
								LStream*		inStream);

	virtual	void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);

	virtual	void		GetAllValues(
								SInt32			&outValue,
								SInt32			&outMinValue,
								SInt32			&outMaxValue);

	virtual	void		SetValue( SInt32 inValue );

	virtual	void		PostSetValue();

	virtual void		SetMinValue( SInt32 inMinValue );

	virtual void		SetMaxValue( SInt32 inMaxValue );

	virtual	void		ListenToMessage(
								MessageT		inMessage,
								void*			ioParam);

	virtual	void		SetDataTag(
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inDataSize,
								void*			inDataPtr);

protected:
	virtual void		FinishCreateSelf();

	virtual	void		BuildPopupHeader(
								ConstStringPtr	inLabel,
								SInt32			inTitleOptions,
								SInt16			inLabelWidth,
								ResIDT			inTextTraitID);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
