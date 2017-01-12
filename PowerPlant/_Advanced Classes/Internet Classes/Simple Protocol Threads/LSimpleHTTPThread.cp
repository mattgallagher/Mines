// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSimpleHTTPThread.cp		PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LSimpleHTTPThread.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LSimpleHTTPThread						Constructor
// ---------------------------------------------------------------------------

LSimpleHTTPThread::LSimpleHTTPThread(LListener * inListener, Boolean inAutoDelete)
		: LSimpleInternetThread(inListener, inAutoDelete)
{
	mConnection = new LHTTPConnection(*this);

	if (inListener) {
		mConnection->AddListener(inListener);
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LSimpleHTTPThread					Destructor
// ---------------------------------------------------------------------------

LSimpleHTTPThread::~LSimpleHTTPThread()
{
	delete mConnection;
}


// ---------------------------------------------------------------------------
//	¥ RunPendingCommands
// ---------------------------------------------------------------------------

void
LSimpleHTTPThread::RunPendingCommands()
{
	Boolean haveException = false;;

	while (mContinue) {
		LInternetEventElem*	baseEvent;

		if (not mEventArray.FetchItemAt(1, baseEvent)) {
			break;
		}
		
		mEventArray.RemoveItemsAt(1, 1);

		LHTTPEventElem*	currEvent = dynamic_cast<LHTTPEventElem*>(baseEvent);

		if (currEvent == nil) {
			break;
		}

		mLastResponse.ResetResponse();

		try {
			switch (currEvent->mEvent) {
				case HTTP_GetEvent:
					mConnection->Get(currEvent->mRemoteHost,
										currEvent->mRequest,
										*(currEvent->mMessage),
										mLastResponse,
										currEvent->mRemotePort);
					break;
				case HTTP_HeadEvent:
					mConnection->Head(currEvent->mRemoteHost,
										currEvent->mRequest,
										*(currEvent->mMessage),
										mLastResponse,
										currEvent->mRemotePort);
					break;
				case HTTP_PostEvent:
					mConnection->Post(currEvent->mRemoteHost,
										currEvent->mRequest,
										*(currEvent->mMessage),
										mLastResponse,
										currEvent->mRemotePort);
					break;
				case HTTP_PutEvent:
					mConnection->Put(currEvent->mRemoteHost,
										currEvent->mRequest,
										*(currEvent->mMessage),
										mLastResponse,
										currEvent->mRemotePort);
					break;

				default:
					break;

			}

			BroadcastMessage(currEvent->mEvent, this);

		} catch (LHTTPResponse& theResponse) {
			mLastResponse = theResponse;
			haveException = true;
			mEventArray.RemoveAllItemsAfter(1);
			BroadcastMessage(HTTP_Exception, &mLastResponse);
			break;
		}

		delete currEvent;
	}
}

#pragma mark -
// ===========================================================================
//	LHTTPEventElem
// ===========================================================================

LHTTPEventElem::LHTTPEventElem(
	EInternetEvent	inEvent)

	: LInternetEventElem(inEvent)
{
	mMessage = nil;

	mRemotePort = kHTTPPort;
}


LHTTPEventElem::~LHTTPEventElem()
{
	delete mMessage;
}


PP_End_Namespace_PowerPlant
