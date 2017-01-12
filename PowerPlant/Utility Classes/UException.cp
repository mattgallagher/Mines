// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UException.cp				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	The C++ standard states that the Exception Handling mechanism
//	calls terminate() when encountering certain unrecoverable errors,
//	such as:
//		- Not finding a handler for a thrown exception
//		- Finding a corrupted stack
//		- A destructor called during stack unwinding tries to exit
//			using an exception
//
//	The default implementation of terminate() calls abort().
//
//	abort() is a standard ANSI C library function. Since PowerPlant
//	programs frequently don't include the ANSI C library, the abort()
//	function is defined below.
//
//	Do NOT include this file in your project if you use the ANSI C or
//	MSL C library. Otherwise, you will get a Linker warning about abort()
//	being multiply defined.

#include <UException.h>
#include <Processes.h>

#if PP_Uses_PowerPlant_Namespace
	using namespace PowerPlant;
#endif

// ---------------------------------------------------------------------------
//	¥ abort
// ---------------------------------------------------------------------------
//	Terminate a program

extern "C" void abort(void);
void abort(void)
{
	SignalStringLiteral_("Aborting Program. Probably a throw without a matching catch.");
	::ExitToShell();
}
