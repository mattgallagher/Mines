// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LChasingArrows.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LChasingArrows
#define _H_LChasingArrows
#pragma once

#include <LControlPane.h>
#include <LPeriodical.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LChasingArrows : public LControlPane,
						 public LPeriodical {
public:
	enum { class_ID		= FOUR_CHAR_CODE('carr'),
		   imp_class_ID	= FOUR_CHAR_CODE('icar') };

						LChasingArrows(
								LStream*			inStream,
								ClassIDT			inImpID = imp_class_ID);

						LChasingArrows(
								const SPaneInfo&	inPaneInfo,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LChasingArrows();

	virtual void		SpendTime( const EventRecord& inMacEvent );

protected:
	UInt32			mNextIdleTick;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
