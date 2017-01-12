// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGACheckBoxGroupBoxImp.h	PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGACheckBoxGroupBoxImp
#define _H_LGACheckBoxGroupBoxImp
#pragma once

#include <LGAGroupBoxImp.h>
#include <LControlPane.h>
#include <LListener.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGACheckBoxGroupBoxImp : public LGAGroupBoxImp,
							   public LListener {
public:
						LGACheckBoxGroupBoxImp(
								LControlPane*	inControlPane);

						LGACheckBoxGroupBoxImp(
								LStream*		inStream);

	virtual				~LGACheckBoxGroupBoxImp();

	virtual	void		Init(	LControlPane*	inControlPane,
								LStream*		inStream);

	virtual	void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);

	virtual	void		SetValue(
								SInt32			inValue);

	virtual	void		PostSetValue();

	virtual SInt16		FindHotSpot(
								Point			inPoint) const;

	virtual	void		ListenToMessage(
								MessageT		inMessage,
								void*			ioParam);

protected:
	virtual	void		BuildCheckBoxHeader(
								ConstStringPtr	inLabel,
								ResIDT			inTextTraitID,
								Boolean			inChecked);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
