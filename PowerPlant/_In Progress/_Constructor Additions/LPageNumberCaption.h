// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPageNumberCaption.h		PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	A caption that changes its contents based on the panel number.
//	Used to build strings of the form "Page #" for use in headers
//	of printed documents.

#ifndef _H_LPageNumberCaption
#define _H_LPageNumberCaption
#pragma once

#include <LCaption.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LPageNumberCaption : public LCaption {

public:
	enum { class_ID = FOUR_CHAR_CODE('page') };

						LPageNumberCaption( LStream* inStream );

	virtual				~LPageNumberCaption();

	virtual void		PrintPanel(
								const PanelSpec&	inPanel,
								RgnHandle			inSuperPrintRgnH);

protected:
	LStr255				mOriginalCaption;

};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
