// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UProcessMgr.cp				PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UProcessMgr.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	 Constants

const	ProcessSerialNumber	UProcessMgr::PSN_NoProcess = { 0, kNoProcess };
const	ProcessSerialNumber	UProcessMgr::PSN_CurrentProcess = { 0, kCurrentProcess };


// ---------------------------------------------------------------------------
//	¥ FindPSN
// ---------------------------------------------------------------------------

OSStatus
UProcessMgr::FindPSN(
	ProcessSerialNumber	&outPSN,
	OSType				inSignature,
	OSType				inType)
{
	OSStatus		status;

	ProcessInfoRec	info;
	info.processInfoLength	= sizeof(ProcessInfoRec);
	info.processName		= nil;
	info.processAppSpec		= nil;

	outPSN = PSN_NoProcess;

	do {
		status = ::GetNextProcess(&outPSN);

		if (status == noErr) {
			OSErr	err = ::GetProcessInformation(&outPSN, &info);

			if ( (err == noErr)  &&
				 (info.processSignature == inSignature)  &&
				 (info.processType == inType) ) {

				break;
			}
		}
	} while (status == noErr);

	return status;
}


// ---------------------------------------------------------------------------
//	¥ ActivateFinder
// ---------------------------------------------------------------------------

OSStatus
UProcessMgr::ActivateFinder()
{
	ProcessSerialNumber	psnFinder;

	OSStatus	status = FindPSN( psnFinder,
								  FOUR_CHAR_CODE('MACS'),
								  FOUR_CHAR_CODE('FNDR') );

	if (status == noErr) {
		status = ::SetFrontProcess(&psnFinder);
	}

	return status;
}


PP_End_Namespace_PowerPlant
