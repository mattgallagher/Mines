// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCleanupTask.cp				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	Abstract base class for any operation which needs to be performed at
//	application shutdown time.
//
//	On Classic, patches ExitToShell.
//
//	On Carbon, things are more difficult because you can't patch. See the
//	use note below for details.
//
//	This class may be used as a mix-in to ensure that an object cleans up
//	after itself when the application quits.

#include <LCleanupTask.h>
#include <LInterruptSafeList.h>

#if !TARGET_API_MAC_OSX
	#include <Traps.h>
#endif

#include <LowMem.h>
#include <Processes.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Important use note message
//
//		Put
//
//			#define PP_Suppress_Notes_221 0
//
//		in your prefix file (or edit this file) to turn off this warning

#if !PP_Suppress_Notes_221 && TARGET_RT_MAC_CFM && PP_Target_Carbon
#pragma ANSI_strict off			// Enable support for #warning

#warning Call LCleanupTask::CleanUpAtExit() before exiting the program

	/*
		You must call LCleanupTask::CleanUpAtExit() when the program ends.
		
		You can call the function before exiting main().
		
		However, if you want to clean up even if the user force quits
		the program, you can specify LCleanupTask_Terminator as the
		CFM termination entry point. You can set this in the PPC Linker
		preferences panel in the CW IDE.
		
		If you already have another termination routine, you can call
		LCleanupTask_Terminator from that routine.
	*/

#pragma ANSI_strict reset
#endif


// ---------------------------------------------------------------------------
//	Prototype for CFM termination routine

pascal void LCleanupTask_Terminator();


// ---------------------------------------------------------------------------
//	ExitToShell UPP information

enum { kExitToShellProcInfo = kPascalStackBased };


// ---------------------------------------------------------------------------
//	Class Variables

TInterruptSafeList<LCleanupTask*>*	LCleanupTask::sCleanupTaskList = nil;

#if PP_Target_Classic
	UniversalProcPtr	LCleanupTask::sOldETSRoutine = nil;
	UniversalProcPtr	LCleanupTask::sNewETSRoutine = nil;
#endif


// ---------------------------------------------------------------------------
//	¥ LCleanupTask						Default Constructor
// ---------------------------------------------------------------------------

LCleanupTask::LCleanupTask()
{
	if (sCleanupTaskList == nil) {

		// No cleanup tasks have been created yet, so create
		// a global list of cleanup tasks that will be performed when
		// the application is closed down. We patch ExitToShell so
		// that we can catch these shutdown events.

		sCleanupTaskList = new TInterruptSafeList<LCleanupTask*>;

		#if PP_Target_Classic

			// Patching traps only works for Classic targets

			sOldETSRoutine = ::GetToolTrapAddress (_ExitToShell);
			sNewETSRoutine = NewRoutineDescriptor ((ProcPtr)&ETSPatch,
							kExitToShellProcInfo, GetCurrentArchitecture());
			::SetToolTrapAddress (sNewETSRoutine, _ExitToShell);

		#endif
	}

	// Add this task to the list of cleanup items.

	sCleanupTaskList->Append(this);

}


// ---------------------------------------------------------------------------
//	¥ ~LCleanupTask							Destructor
// ---------------------------------------------------------------------------

LCleanupTask::~LCleanupTask()
{
	if (sCleanupTaskList != nil) {
		sCleanupTaskList->Remove(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ CleanUpAtExit
// ---------------------------------------------------------------------------
//	Should be called only once whenever the application quits, either by
//	the normal Quit command exit or by a user abort.

void
LCleanupTask::CleanUpAtExit()
{
	if (sCleanupTaskList != nil) {

		TInterruptSafeListIterator<LCleanupTask*>	iter(*sCleanupTaskList);
		LCleanupTask*	item;

		while (iter.Next(item)) {
			item->DoCleanup();
		}

		delete sCleanupTaskList;
		sCleanupTaskList = nil;
	}

}


// ---------------------------------------------------------------------------
//	¥ DoCleanup
// ---------------------------------------------------------------------------
//	Override to perform any task that must execute before the app quits.
//
// void LCleanupTask::DoCleanup()			Pure Virtual function

#pragma mark LCleanupTask::DoCleanup



// ---------------------------------------------------------------------------
//	¥ ETSPatch
// ---------------------------------------------------------------------------
//	We patch ExitToShell to ensure that the application cleans up even
//	when the user aborts via Command-Option-Escape or the "es" command
//	of MacsBug.

#if PP_Target_Classic			// No patches under Carbon

#pragma profile off

void
LCleanupTask::ETSPatch()
{

	SetCurrentA5();      // Restore our context

    // Remove ExitToShell trap patch in case something goes
    // wrong while cleaning up.

	if (sOldETSRoutine)
		::SetToolTrapAddress (sOldETSRoutine, _ExitToShell);

    // Do last-minute clean-up activities.

    CleanUpAtExit();

    // Call original ExitToShell().

	::ExitToShell();
}

#endif


// ---------------------------------------------------------------------------
//	¥ LCleanupTask_Terminator
// ---------------------------------------------------------------------------

pascal void LCleanupTask_Terminator()
{
	LCleanupTask::CleanUpAtExit();
}


PP_End_Namespace_PowerPlant
