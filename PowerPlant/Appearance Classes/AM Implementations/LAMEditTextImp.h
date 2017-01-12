// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMEditTextImp.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LAMEditTextImp
#define _H_LAMEditTextImp
#pragma once

#include <LControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LAMEditTextImp : public LControlImp {
public:
						LAMEditTextImp( LControlPane* inControlPane );

						LAMEditTextImp( LStream* inStream );

	virtual				~LAMEditTextImp();

	virtual void		DrawSelf();

protected:
	void				DrawFocusBorder(
								const Rect&	inFrame,
								bool		inHasFocus);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
