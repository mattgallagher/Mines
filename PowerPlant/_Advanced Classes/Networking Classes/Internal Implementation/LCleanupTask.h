// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCleanupTask.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	Abstract base class for any operation which needs to be performed at
//	application shutdown time. Patches ExitToShell to ensure that tasks
//	are performed even if user force-quits application.
//
//	This class may be used as a mix-in to ensure that an object cleans up
//	after itself when the application quits.

#ifndef _H_LCleanupTask
#define _H_LCleanupTask
#pragma once

#include <PP_Prefix.h>
#include <LInterruptSafeList.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LCleanupTask : public LInterruptSafeListMember {

public:
	static void					CleanUpAtExit();

								LCleanupTask();
	virtual						~LCleanupTask();
	
protected:

	virtual void				DoCleanup() = 0;

#if PP_Target_Classic
	static void					ETSPatch();

	static UniversalProcPtr		sOldETSRoutine;
	static UniversalProcPtr		sNewETSRoutine;
#endif

	static TInterruptSafeList<LCleanupTask*>*	sCleanupTaskList;

private:
								LCleanupTask(const LCleanupTask&);	// do not use
	LCleanupTask&				operator = (const LCleanupTask&);
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
