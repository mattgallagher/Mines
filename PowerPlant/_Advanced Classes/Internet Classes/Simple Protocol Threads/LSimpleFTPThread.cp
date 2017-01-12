// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSimpleFTPThread.cp			PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LSimpleFTPThread.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LSimpleFTPThread						Constructor
// ---------------------------------------------------------------------------

LSimpleFTPThread::LSimpleFTPThread(
	LListener * inListener,
	Boolean inAutoDelete)
		: LSimpleInternetThread(inListener, inAutoDelete)
{
	mConnection = new LFTPConnection(*this);

	if (inListener) {
		mConnection->AddListener(inListener);
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LSimpleFTPThread						Destructor
// ---------------------------------------------------------------------------

LSimpleFTPThread::~LSimpleFTPThread()
{
	delete mConnection;
}


// ---------------------------------------------------------------------------
//	¥ RunPendingCommands
// ---------------------------------------------------------------------------

void
LSimpleFTPThread::RunPendingCommands()
{
	Boolean haveException = false;;

	while (mContinue) {
		LInternetEventElem*	baseEvent;

		if (not mEventArray.FetchItemAt(1, baseEvent)) {
			break;
		}
		
		mEventArray.RemoveItemsAt(1, 1);

		LFTPEventElem*	currEvent = dynamic_cast<LFTPEventElem*>(baseEvent);

		if (currEvent == nil) {
			break;
		}

		mLastResponse.ResetResponse();

		try {
			switch (currEvent->mEvent) {
				case FTP_ConnectEvent:
					mConnection->Connect(currEvent->mRemoteHost,
										currEvent->mUser,
										currEvent->mPassword,
										currEvent->mAccount,
										currEvent->mRemotePort);
					break;

				case FTP_DisconnectEvent:
					mConnection->Disconnect();
					break;

				case FTP_GetFileEvent:
					mConnection->GetFile(currEvent->mXferType,
										currEvent->mOriginalFileName,
										currEvent->mFile,
										currEvent->mUsePassive);
					break;

				case FTP_PutFileEvent:
					mConnection->PutFile(currEvent->mXferType,
										currEvent->mOriginalFileName,
										currEvent->mFile,
										currEvent->mReturnFileName,
										currEvent->mCreateUnique,
										currEvent->mUsePassive);
					break;

				case FTP_GetFileAutoEvent:
					mConnection->GetFile(currEvent->mRemoteHost,
										currEvent->mUser,
										currEvent->mPassword,
										currEvent->mAccount,
										currEvent->mXferType,
										currEvent->mOriginalFileName,
										currEvent->mFile,
										currEvent->mUsePassive,
										currEvent->mRemotePort);
					break;

				case FTP_PutFileAutoEvent:
					mConnection->PutFile(currEvent->mRemoteHost,
										currEvent->mUser,
										currEvent->mPassword,
										currEvent->mAccount,
										currEvent->mXferType,
										currEvent->mOriginalFileName,
										currEvent->mFile,
										currEvent->mReturnFileName,
										currEvent->mCreateUnique,
										currEvent->mUsePassive,
										currEvent->mRemotePort);
					break;

				case FTP_ChangeDirEvent:
					mConnection->SendChangeDir(currEvent->mOriginalFileName);
					break;

				case FTP_ListFolderEvent:
					break;

				default:
					break;

			}

			BroadcastMessage(currEvent->mEvent, this);

		} catch (LFTPResponse& theResponse) {
			mLastResponse = theResponse;
			haveException = true;
			mEventArray.RemoveAllItemsAfter(1);
			BroadcastMessage(FTP_Exception, &mLastResponse);
			break;
		}

		delete currEvent;
	}
}

#pragma mark -
// ===========================================================================
//	LFTPEventElem
// ===========================================================================

LFTPEventElem::LFTPEventElem(
	EInternetEvent	inEvent)

	: LInternetEventElem(inEvent)
{
	mAccount[0]				= '\0';
	mOriginalFileName[0]	= '\0';
	mFile					= nil;
	mReturnFileName			= nil;
	mCreateUnique			= false;
	mUsePassive				= false;
	mXferType				= FTPUndefinedXfer;

	mRemotePort				= kFTPPort;
}


LFTPEventElem::~LFTPEventElem()
{
}


PP_End_Namespace_PowerPlant
