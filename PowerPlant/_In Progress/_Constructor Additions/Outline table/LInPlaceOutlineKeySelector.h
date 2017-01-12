// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInPlaceOutlineKeySelector.h PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LInPlaceOutlineKeySelector
#define _H_LInPlaceOutlineKeySelector
#pragma once

#include <LOutlineKeySelector.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LInPlaceOutlineKeySelector : public LOutlineKeySelector {

public:
	enum { class_ID = FOUR_CHAR_CODE('ipky') };

						LInPlaceOutlineKeySelector( LStream* inStream );
									
						LInPlaceOutlineKeySelector(
								LOutlineTable*		inOutlineTable,
								MessageT			inMessage = msg_AnyMessage,
								Boolean				inAcceptReturnKey = true);
									
	virtual				~LInPlaceOutlineKeySelector();

protected:
	virtual void		HandleKeyEvent( const EventRecord* inEvent );

	virtual void		RotateTarget( Boolean inBackward );

protected:
	Boolean			mAcceptReturnKey;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
