// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UProfiler.cp				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	CodeWarrior profiler utility functions

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UProfiler.h>
#include <LString.h>

#include <Profiler.h>

PP_Begin_Namespace_PowerPlant

#pragma profile off


// ===========================================================================
//	¥ StProfileSection Class
// ===========================================================================
//	Constructor tells the profiler to start recording
//	Destructor turns off the profiler & dumps information to disk

StProfileSection::StProfileSection(
	Str255	inDumpFileName,			// filename to receive profile logs
	SInt16	inNumFunctions,			// number of functions
	SInt16	inStackDepth) 			// stack depth

	: mNumFunctions(inNumFunctions),
  	  mStackDepth(inStackDepth)
{
	ProfilerInit(collectDetailed, bestTimeBase, inNumFunctions, inStackDepth);
	ProfilerSetStatus(false);

	LString::CopyPStr(inDumpFileName, mProfilerDumpFile);

	ProfilerSetStatus(true);
}


StProfileSection::~StProfileSection()
{
	ProfilerSetStatus(false);

	long functionSize;				// diagnostic tool: check data sizes
	long stackSize;
	ProfilerGetDataSizes(&functionSize, &stackSize);

	Assert_(functionSize < mNumFunctions);
	Assert_(stackSize < mStackDepth);

	ProfilerDump(mProfilerDumpFile);
	ProfilerTerm();
}


PP_End_Namespace_PowerPlant
