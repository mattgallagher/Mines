// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAEditTextImp.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LGAEditTextImp
#define _H_LGAEditTextImp
#pragma once

#include <LControlImp.h>
#include <TextEdit.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGAEditTextImp : public LControlImp {
public:
						LGAEditTextImp(
								LControlPane	*inControlPane);
						LGAEditTextImp(
								LStream			*inStream);

	virtual				~LGAEditTextImp();

	virtual	void		DrawSelf();
	virtual	void		DrawBorder(
								SInt16			inBitDepth);
	virtual	void		DrawFocusRing(
								bool			inHasRing,
								SInt16			inBitDepth);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
