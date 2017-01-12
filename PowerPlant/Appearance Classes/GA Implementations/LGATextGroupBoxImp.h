// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGATextGroupBoxImp.h		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGATextGroupBoxImp
#define _H_LGATextGroupBoxImp
#pragma once

#include <LGAGroupBoxImp.h>
#include <LView.h>
#include <LPane.h>
#include <LControlImp.h>
#include <LControlPane.h>
#include <LListener.h>
#include <LString.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGATextGroupBoxImp : 	public LGAGroupBoxImp {
public:
						LGATextGroupBoxImp(
								LControlPane*		inControlPane);

						LGATextGroupBoxImp(
								LStream*			inStream);

	virtual				~LGATextGroupBoxImp();

	virtual	void		Init (	LControlPane*		inControlPane,
								LStream*			inStream);
																									// Override
	virtual	void		Init (	LControlPane*		inControlPane,
								SInt16				inControlKind,
								ConstStringPtr		inTitle = Str_Empty,
								ResIDT				inTextTraitsID = 0,
								SInt32				inRefCon = 0);
																									//	Override
protected:
	virtual	void		BuildTextHeader(
								ConstStringPtr		inDescriptor,
								ResIDT				inTextTraitID,
								PaneIDT				inHeaderID);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
