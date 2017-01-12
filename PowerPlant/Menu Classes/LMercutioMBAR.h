// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMercutioMBAR.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LMercutioMBAR
#define _H_LMercutioMBAR
#pragma once

#include <LMenuBar.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LMercutioMBAR : public LMenuBar {
public:
						LMercutioMBAR( ResIDT inMBARid );

	virtual				~LMercutioMBAR();

	virtual CommandT	FindKeyCommand(
								const EventRecord&	inKeyEvent,
								SInt32&				outMenuChoice) const;

protected:
	MenuHandle		mMercutioMenu;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
