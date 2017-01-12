// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UQuickTime.h				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UQuickTime
#define _H_UQuickTime
#pragma once

#include <LPane.h>
#include <LPeriodical.h>

#if TARGET_RT_MAC_MACHO
	#include <QuickTime/Movies.h>
#else
	#include <Movies.h>
#endif

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//	¥ UQuickTime ¥
// ===========================================================================

class	UQuickTime {
public:
	static bool		QuickTimeIsPresent();
	static void		Initialize();
	static void		Finalize();

	static Movie	GetMovieFromFile();
	static void		SaveFlattenedMovie( Movie inMovie );
};


// ===========================================================================
//	¥ LMovieController ¥
// ===========================================================================

class	LMovieController : public LPane,
						   public LPeriodical {
public:
						LMovieController();

						LMovieController(
								const SPaneInfo &inPaneInfo,
								Movie inMovie);

	virtual				~LMovieController();

	virtual void		SpendTime( const EventRecord &inMacEvent );

protected:
	Movie				mMovie;
	MovieController		mMovieController;

	virtual void		DrawSelf();
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
