// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LProgressBar.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LProgressBar
#define _H_LProgressBar
#pragma once

#include <LControlPane.h>
#include <LPeriodical.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LProgressBar : public LControlPane,
					   public LPeriodical {
public:
	enum { class_ID		= FOUR_CHAR_CODE('pbar'),
		   imp_class_ID	= FOUR_CHAR_CODE('ipbr') };

						LProgressBar(
								LStream*			inStream,
								ClassIDT			inImpID = imp_class_ID);

						LProgressBar(
								const SPaneInfo& 	inPaneInfo,
								MessageT			inValueMessage,
								SInt32				inValue,
								SInt32				inMinValue,
								SInt32				inMaxValue,
								Boolean				inIsIndeterminate = false,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LProgressBar();

	void				SetIndeterminateFlag(
								Boolean				inIsIndeterminate,
								Boolean				inStartNow = true);

	bool				IsIndeterminate() const;

	void				Start();
	void				Stop();

	virtual void		SpendTime( const EventRecord& inMacEvent );

protected:
	UInt32				mNextIdleTick;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
