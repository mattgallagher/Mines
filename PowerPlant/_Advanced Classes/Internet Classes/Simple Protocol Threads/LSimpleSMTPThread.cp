// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSimpleSMTPThread.cp		PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LSimpleSMTPThread.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LSimpleSMTPThread						Constructor
// ---------------------------------------------------------------------------

LSimpleSMTPThread::LSimpleSMTPThread(LListener * inListener, Boolean inAutoDelete)
		: LSimpleInternetThread(inListener, inAutoDelete)
{
	mConnection = new LSMTPConnection(*this);

	if (inListener) {
		mConnection->AddListener(inListener);
	}
}

// ---------------------------------------------------------------------------
//	¥ ~LSimpleSMTPThread					Destructor
// ---------------------------------------------------------------------------

LSimpleSMTPThread::~LSimpleSMTPThread()
{
	delete mConnection;
}

// ---------------------------------------------------------------------------
//	¥ RunPendingCommands
// ---------------------------------------------------------------------------

void
LSimpleSMTPThread::RunPendingCommands()
{
	Boolean haveException = false;;

	while (mContinue) {
		LInternetEventElem*	baseEvent;

		if (not mEventArray.FetchItemAt(1, baseEvent)) {
			break;
		}
		
		mEventArray.RemoveItemsAt(1, 1);

		LSMTPEventElem*	currEvent = dynamic_cast<LSMTPEventElem*>(baseEvent);

		if (currEvent == nil) {
			break;
		}

		mLastResponse.ResetResponse();

		try {
			switch (currEvent->mEvent) {
				case SMTP_SendOneMessageEvent:
					mConnection->SendOneMessage(currEvent->mRemoteHost,
										*(currEvent->mMailMessage),
										currEvent->mRemotePort);
					break;

				default:
					break;

			}

			BroadcastMessage(currEvent->mEvent, this);

		} catch (LSMTPResponse& theResponse) {
			mLastResponse = theResponse;
			haveException = true;
			mEventArray.RemoveAllItemsAfter(1);
			BroadcastMessage(SMTP_Exception, &mLastResponse);
			break;
		}

		delete currEvent;
	}
}

#pragma mark -
// ===========================================================================
//	LSMTPEventElem
// ===========================================================================

LSMTPEventElem::LSMTPEventElem(
	EInternetEvent	inEvent)

	: LInternetEventElem(inEvent)
{
	mMailMessage = nil;

	mRemotePort = kSMTPPort;;
}


LSMTPEventElem::~LSMTPEventElem()
{
	delete mMailMessage;
}


PP_End_Namespace_PowerPlant
