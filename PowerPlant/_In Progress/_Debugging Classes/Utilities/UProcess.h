// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UProcess.h		   			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub

#ifndef _H_UProcess
#define _H_UProcess
#pragma once

#include <UEnvironment.h>
#include <LString.h>

#include <Processes.h>
#include <Files.h>
#include <Gestalt.h>
#include <Aliases.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Using an SProcessInfo class makes obtaining process information easier.

class SProcessInfo {
public:
	ProcessInfoRec		info;
	Str255				name;
	FSSpec				appSpec;

						SProcessInfo()
							{
								info.processInfoLength	= sizeof(ProcessInfoRec);
								info.processName		= name;
								info.processAppSpec		= &appSpec;
							}

	operator ProcessInfoRecPtr() { return &info; }

	FSSpec				GetSpec()	{ return appSpec; }

	void				GetName(Str255 outString)
							{
								LString::CopyPStr(name, outString);
							}

private:
						SProcessInfo(const SProcessInfo& inOriginal);
		SProcessInfo&	operator=(const SProcessInfo& inRhs);
};


// ---------------------------------------------------------------------------

extern const	ProcessSerialNumber		PSN_NoProcess;
extern const	ProcessSerialNumber		PSN_CurrentProcess;


// ---------------------------------------------------------------------------

namespace UProcess {

				// ProcessSerialNumber (PSN) accessors

	ProcessSerialNumber		GetMyPSN();

	ProcessSerialNumber		GetPSN(	OSType				inCreator,
									OSType				inType = FOUR_CHAR_CODE('APPL'));
	ProcessSerialNumber		GetPSN( ConstStr255Param	inName,
									OSType				inType = FOUR_CHAR_CODE('APPL'),
									bool				inCaseSensitive = false,
									bool				inDiacSens = false );
	ProcessSerialNumber		GetCurrentPSN();
	bool					GetNextPSN(
									ProcessSerialNumber&			ioPSN);
	bool					GetPreviousPSN(
									ProcessSerialNumber&			ioPSN);


				// PSN comparisons

	bool					IsSame(	const ProcessSerialNumber&		inProc1,
									const ProcessSerialNumber&		inProc2);

				// Front process

	void					SetFront(
									const ProcessSerialNumber&		inPSN);
	ProcessSerialNumber		GetFront();

	inline	bool			AmIFront()
								{
										// Is caller front process?
									return IsSame(GetFront(), GetMyPSN());
								}
	inline	void			MakeMeFront()
								{
										// Make caller front process
									SetFront(GetMyPSN());
								}

				// Waking processes

	void					Wake(	const ProcessSerialNumber&		inPSN);
	inline	void			WakeMe()
								{
										// Wake caller
									Wake(GetMyPSN());
								}
	inline	void			WakeFront()
								{
										// Wake front process
									Wake(GetFront());
								}

				// Process Information

	void					GetInfo(
									const ProcessSerialNumber&		inPSN,
									SProcessInfo&					outProcInfo);
	inline	void			GetMyInfo(
									SProcessInfo&					outProcInfo)
								{
										// Return info about caller
									GetInfo(GetMyPSN(), outProcInfo);
								}
	inline	void			GetFrontInfo(
									SProcessInfo&					outProcInfo)
								{
										// Return info about front process
									GetInfo(GetFront(), outProcInfo);
								}

				// Killing processes

	void					Kill(	const ProcessSerialNumber&		inPSN);
	inline	void			KillMe()
								{
										// Kill calling process
									Kill(GetMyPSN());
								}
	inline	void			KillFront()
								{
										// Kill front process
									Kill(GetFront());
								}
	void					KillAllButMe(
									bool				inKillFinder = true);
	inline	void			KillAll()
								{
										// Attempt to terminate all running processes
									KillAllButMe();
									KillMe();
								}

				// Utilities

	bool					IsRunning(
									OSType				inCreator,
									OSType				inType = FOUR_CHAR_CODE('APPL'));
	bool					IsRunning(
									ConstStr255Param	inName,
									OSType				inType = FOUR_CHAR_CODE('APPL'),
									bool				inCaseSensative = false,
									bool				inDiacSens = false);

	inline	bool			ProcessManagerIsPresent()
								{
									return UEnvironment::HasGestaltAttribute(
											gestaltOSAttr,
											gestaltLaunchControl);
								}

				// Launching applications

	ProcessSerialNumber		LaunchApp(
									const FSSpec&		inFileSpec,
									LaunchFlags			inFlags = launchContinue +
																	launchNoFileFlags);

	ProcessSerialNumber		LaunchApp(
									AliasHandle			inFileSpec,
									LaunchFlags			inFlags = launchContinue +
																	launchNoFileFlags);

	bool					Launch(	OSType					inSignature,
									OSType					inType = FOUR_CHAR_CODE('APPL'),
									LaunchFlags				inFlags = launchContinue +
																		launchNoFileFlags,
									FSSpec*					outFSSpec = nil,
									ProcessSerialNumber*	outPSN = nil);

#if PP_MoreFiles_Support

	bool					Launch(	ConstStr255Param		inName,
									OSType					inType = FOUR_CHAR_CODE('APPL'),
									LaunchFlags				inFlags = launchContinue +
																		launchNoFileFlags,
									FSSpec*					outFSSpec = nil,
									ProcessSerialNumber*	outPSN = nil);
#endif

}

// ---------------------------------------------------------------------------

inline
bool
operator==(
	const ProcessSerialNumber&	inLhs,
	const ProcessSerialNumber&	inRhs)
{
	return UProcess::IsSame(inLhs, inRhs);
}

inline
bool
operator!=(
	const ProcessSerialNumber&	inLhs,
	const ProcessSerialNumber&	inRhs)
{
	return (!(inLhs == inRhs));
}

// ---------------------------------------------------------------------------


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_UProcess
