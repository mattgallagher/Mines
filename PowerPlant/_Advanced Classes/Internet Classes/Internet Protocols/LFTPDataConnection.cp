// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFTPDataConnection.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
// FTP Connection - This object represnts the "Command Connection" portion of
//	the FTP protocol.
//
// NOTE: This will normally only used by the LFTPConnection object. You should
//		never have to call this class directly.


#include <LFTPDataConnection.h>
#include <LInternetAddress.h>
#include <LInternetMapper.h>
#include <LMacBinaryFile.h>
#include <UNetworkMessages.h>
#include <UNetworkFactory.h>
#include <UInternet.h>
#include <UThread.h>

#include <Files.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LFTPDataConnection						Constructor
// ---------------------------------------------------------------------------

LFTPDataConnection::LFTPDataConnection(
	LFTPConnection*	inMaster)

	: mMasterConnection(inMaster)
{
	mEndpoint		= nil;
	mRemoteHostPort	= 0;
	mThread			= nil;
	mConnected		= false;
	mSendSize		= kFTPDataDefaultSendSize;
	mRcvSize		= kFTPDataDefaultRcvSize;
	mTimeoutSeconds = inMaster->GetTimeoutSeconds();
}

// ---------------------------------------------------------------------------
//	¥ ~LFTPDataConnection						Destructor
// ---------------------------------------------------------------------------

LFTPDataConnection::~LFTPDataConnection()
{
	delete mEndpoint;
}


// ---------------------------------------------------------------------------
//	¥ MakeDataConnection
// ---------------------------------------------------------------------------
// usePassive indicates that we need to create and open a connection to the host
//	and port passed into the method.
// !usePassive indicates that we need to create a new port to accept incomming
//	connections and return our host/port info to the calling routine.

void
LFTPDataConnection::MakeDataConnection(
	LStr255				ioHost,
	UInt32 *			ioHostNum,
	SInt16 *			ioPort,
	Boolean				usePassive)
{
	SetRemoteHost(ioHost);
	SetRemotePort((UInt16) *ioPort);

	mEndpoint = UNetworkFactory::CreateTCPEndpoint();
	mEndpoint->AddListener(this);
	LInternetAddress localAddress(0, 0);

	if (usePassive) {
		mEndpoint->Bind(localAddress);

		LInternetAddress remoteAddress(ioHost, (UInt16) *ioPort, true);
		mEndpoint->Connect(remoteAddress);
		mConnected = true;
	} else {
		mEndpoint->Bind(localAddress, 1);

		LInternetAddress* myAddress = mEndpoint->GetLocalAddress();
		*ioHostNum = myAddress->GetIPAddress(ioHost);
		*ioPort = (SInt16) myAddress->GetHostPort();
		delete myAddress;
	}
}


// ---------------------------------------------------------------------------
//	¥ Disconnect
// ---------------------------------------------------------------------------

void
LFTPDataConnection::Disconnect()
{
	mConnected = false;
	try {
		Abort();
		mEndpoint->SendDisconnect();
	} catch(...) {
		// We want this to fail quitely without
		// throwing all the way to the calling function of the CmdConnection.
	}
}


// ---------------------------------------------------------------------------
//	¥ Abort
// ---------------------------------------------------------------------------

void
LFTPDataConnection::Abort()
{
	if (mThread) {
		mEndpoint->AbortThreadOperation(mThread);
	}
}


// ---------------------------------------------------------------------------
//	¥ ListenToMessage
// ---------------------------------------------------------------------------

void
LFTPDataConnection::ListenToMessage(MessageT inMessage, void* /* ioParam */)
{
	switch (inMessage) {
		case T_LISTEN:
			HandleListen();
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ WaitTillConnected
// ---------------------------------------------------------------------------

void
LFTPDataConnection::WaitTillConnected()
{
	while (!mConnected) {
		LThread::Yield();
	}
}


// ---------------------------------------------------------------------------
//	¥ IsIdle
// ---------------------------------------------------------------------------

Boolean
LFTPDataConnection::IsIdle()
{
	if (mEndpoint == nil) {
		return true;
	}

	EEndpointState netState = mEndpoint->GetState();
	return ((netState == endpoint_Idle) || (netState == endpoint_Unbound));
}


// ---------------------------------------------------------------------------
//	¥ SendFile
// ---------------------------------------------------------------------------

void
LFTPDataConnection::SendFile(
	LFile * inFile,
	SProgressMessage&	inProgressMsg)
{
	OSErr err;
	mThread = LThread::GetCurrentThread();
	WaitTillConnected();

	if ((mMasterConnection->GetTransferType() == FTPImageXfer)
		&& (mMasterConnection->GetMacBinaryNegotiated())) {
		SendFileMacBinary(inFile, inProgressMsg);
	} else {
		SInt32 count = 0;

			// If transfer type is ASCII, double the data buffer size to
			// allow for LF characters to be added if needed.

		SInt32 dataBufferSize = (SInt32) mSendSize;
		if (mMasterConnection->GetTransferType() == FTPASCIIXfer) {
			dataBufferSize *= 2;
		}

		StPointerBlock theData((SInt32) dataBufferSize);

		SInt16 fileRef = inFile->OpenDataFork(fsRdPerm);

		try {
			ThrowIfOSErr_(::SetFPos(fileRef, fsFromStart, 0));

			SInt32 theSize;
			ThrowIfOSErr_(::GetEOF(fileRef, &theSize));
			if (!inProgressMsg.totalBytes)
				inProgressMsg.totalBytes = (UInt32) theSize;

			do {
				count = (SInt32) mSendSize;
				err = ::FSRead(fileRef, &count, theData);
				if (err != eofErr) {
					ThrowIfOSErr_(err);
				}

				// If transfer type is ASCII, loop through the data searching
				// for CR not followed by LF. If a single CR is found, insert
				// the following LF character. The last character of buffer
				// is handled as a special case.

				if (mMasterConnection->GetTransferType() == FTPASCIIXfer) {
					SInt32 index = 0;
					while (index < (count - 1)) {
						if ( (theData[index] == CR) && (theData[index+1] != LF) ) {
							::BlockMove(theData.Get() + index + 1, theData.Get() + index + 2,
								count - index - 1);
							theData[index + 1] = LF;
							count ++;
						}
						index ++;
					}

					if (theData[count - 1] == CR) {
						theData[count] = LF;
						count ++;
					}
				}
				SendData(theData, (UInt32)count, inProgressMsg);
			} while (err != eofErr);

			inFile->CloseDataFork();
		}

		catch (...) {
			inFile->CloseDataFork();
			mThread = nil;
			throw;
		}
	}

	mThread = nil;
}


// ---------------------------------------------------------------------------
//	¥ SendFileMacBinary
// ---------------------------------------------------------------------------

void
LFTPDataConnection::SendFileMacBinary(
					LFile * inFile,
					SProgressMessage&	inProgressMsg)
{
	FSSpec theSpec;
	inFile->GetSpecifier(theSpec);
	LString::PToCStr(theSpec.name);

	LMacBinaryFile theBinFile;
	theBinFile.Open((char*)theSpec.name, theSpec.vRefNum, theSpec.parID, MB_READ);

	SInt32 count = 0;
	StPointerBlock theData((SInt32) mSendSize);

	try {
		if (!inProgressMsg.totalBytes)
			inProgressMsg.totalBytes = (UInt32) theBinFile.GetFileSize();

		while (true) {
			count = (SInt32) mSendSize;
			count = theBinFile.Read(theData, count);
			if (count <= 0) {
				break;
			}
			SendData(theData, (UInt32)count, inProgressMsg);
		}

		theBinFile.Close();
	}
	catch(...) {
		theBinFile.Close();
		mThread = nil;
		throw;
	}
}


// ---------------------------------------------------------------------------
//	¥ RetrieveFile
// ---------------------------------------------------------------------------

void
LFTPDataConnection::RetrieveFile(
						LFile *				inFile,
						SProgressMessage&	inProgressMsg)
{
	mThread = LThread::GetCurrentThread();
	WaitTillConnected();

	if ((mMasterConnection->GetTransferType() == FTPImageXfer)
		&& (mMasterConnection->GetMacBinaryNegotiated())) {
		RetrieveFileMacBinary(inFile, inProgressMsg);
	} else {
		UInt32 dataSize;
		SInt32 bytesWritten;
		StPointerBlock theData((SInt32) mRcvSize);
		Boolean dataExpedited;

		SInt16 fileRef = inFile->OpenDataFork(fsRdWrPerm);

		try {
			ThrowIfOSErr_(::SetFPos(fileRef, fsFromStart, 0));

			inProgressMsg.completedBytes = 0;

			while (1) {
				try {
					dataSize = mRcvSize;
					mEndpoint->ReceiveData(theData, dataSize, dataExpedited, mTimeoutSeconds);

					// If transfer type is ASCII, loop through the data searching
					// for CR followed by LF. If the sequence CR + LF is found,
					// replace with a single CR character. The last character of
					// buffer is handled as a special case and can be ignored.

					if (mMasterConnection->GetTransferType() == FTPASCIIXfer) {
						SInt32 index = 0;
						while (index < (dataSize - 2)) {
							if ( (theData[index] == CR) && (theData[index+1] == LF) ) {
								::BlockMove(theData.Get() + index + 2, theData.Get() + index + 1,
									(SInt32) (dataSize - index - 2));
								dataSize --;
							}
							index ++;
						}
					}

					bytesWritten = (SInt32) dataSize;
					ThrowIfOSErr_(::FSWrite(fileRef, &bytesWritten, theData));
					ThrowIf_(bytesWritten != dataSize);
					inProgressMsg.completedBytes += dataSize;
					BroadcastProgress(msg_ReceivingData, inProgressMsg);
				}

				catch (const LException& err) {
					if (IsDisconnectError(err.GetErrorCode())) {
						break;
					} else {
						mThread = nil;
						throw;
					}
				}

				catch (ExceptionCode err) {
					if (IsDisconnectError(err)) {
						break;
					} else {
						mThread = nil;
						throw;
					}
				}
			}

			//Force the last msg_ReceivingData message just in case
			BroadcastProgress(msg_ReceivingData, inProgressMsg, true);
			inFile->CloseDataFork();
		}

		catch(...) {
			inFile->CloseDataFork();
			mThread = nil;
			throw;
		}
	}

	mThread = nil;
}


// ---------------------------------------------------------------------------
//	¥ RetrieveFileMacBinary
// ---------------------------------------------------------------------------

void
LFTPDataConnection::RetrieveFileMacBinary(
						LFile *				inFile,
						SProgressMessage&	inProgressMsg)
{
	FSSpec theSpec;
	inFile->GetSpecifier(theSpec);
	LString::PToCStr(theSpec.name);

	LMacBinaryFile theBinFile;
	theBinFile.Open((char*)theSpec.name, theSpec.vRefNum, theSpec.parID, MB_READ);

	UInt32			dataSize;
	SInt32			bytesWritten;
	StPointerBlock	theData((SInt32) mRcvSize);
	Boolean			dataExpedited;

	try {
		inProgressMsg.completedBytes = 0;

		while (1) {
			try {
				dataSize = mRcvSize;
				mEndpoint->ReceiveData(theData, dataSize, dataExpedited, mTimeoutSeconds);

				bytesWritten = theBinFile.Write(theData, (SInt32) dataSize);
				ThrowIf_(bytesWritten != dataSize);

				inProgressMsg.completedBytes += dataSize;
				BroadcastProgress(msg_ReceivingData, inProgressMsg);
			}

			catch (const LException& err) {
				if (IsDisconnectError(err.GetErrorCode())) {
					break;
				} else {
					mThread = nil;
					throw;
				}
			}

			catch (ExceptionCode err) {
				if (IsDisconnectError(err)) {
					break;
				} else {
					mThread = nil;
					throw;
				}
			}
		}

		//Force the last msg_ReceivingData message just in case
		BroadcastProgress(msg_ReceivingData, inProgressMsg, true);
		theBinFile.Close();
	} catch(...) {
		theBinFile.Close();
		mThread = nil;
		throw;
	}
}


// ---------------------------------------------------------------------------
//	¥ RetrieveData
// ---------------------------------------------------------------------------

void
LFTPDataConnection::RetrieveData(
					LDynamicBuffer* outBuffer,
					SProgressMessage&	inProgressMsg)
{
	mThread = LThread::GetCurrentThread();
	WaitTillConnected();

	UInt32 dataSize;
	StPointerBlock theData((SInt32) mRcvSize);
	Boolean dataExpedited;

	inProgressMsg.completedBytes = 0;

	while (1) {
		try {
			dataSize = mRcvSize;
			mEndpoint->ReceiveData(theData, dataSize, dataExpedited, mTimeoutSeconds);
			outBuffer->ConcatenateBuffer(theData, (SInt32) dataSize);
			inProgressMsg.completedBytes += dataSize;
			BroadcastProgress(msg_ReceivingData, inProgressMsg);
		}

		catch (const LException& err) {
			if (IsDisconnectError(err.GetErrorCode())) {
				break;
			} else {
				mThread = nil;
				throw;
			}
		}

		catch (ExceptionCode err) {
			if (IsDisconnectError(err)) {
				break;
			} else {
				mThread = nil;
				throw;
			}
		}
	}
	mThread = nil;

	//Force the last msg_ReceivingData message just in case
	BroadcastProgress(msg_ReceivingData, inProgressMsg, true);
}


// ---------------------------------------------------------------------------
//	¥ RetrieveData
// ---------------------------------------------------------------------------

void
LFTPDataConnection::RetrieveData(
					LDynamicBuffer* outBuffer)
{
	mThread = LThread::GetCurrentThread();
	WaitTillConnected();

	UInt32 dataSize;
	StPointerBlock theData((SInt32) mRcvSize);
	Boolean dataExpedited;

	while (true) {
		try {
			dataSize = mRcvSize;
			mEndpoint->ReceiveData(theData, dataSize, dataExpedited, mTimeoutSeconds);
			outBuffer->ConcatenateBuffer(theData, (SInt32) dataSize);
		}

		catch (const LException& err) {
			if (IsDisconnectError(err.GetErrorCode())) {
				break;
			} else {
				mThread = nil;
				throw;
			}
		}

		catch (ExceptionCode err) {
			if (IsDisconnectError(err)) {
				break;
			} else {
				mThread = nil;
				throw;
			}
		}
	}
	mThread = nil;
}


// ---------------------------------------------------------------------------
//	¥ HandleListen
// ---------------------------------------------------------------------------
// Makes new thread since we told the endpoint it was threaded when we created it.

void
LFTPDataConnection::HandleListen()
{
	LConnectorThread * thread = new LConnectorThread(this);
	thread->Resume();
}


// ---------------------------------------------------------------------------
//	¥ ConnectThreadDone
// ---------------------------------------------------------------------------

void
LFTPDataConnection::ConnectThreadDone()
{
	mConnected = true;
	if (mThread) {
		LThread::Yield(mThread);
	}
}


// ---------------------------------------------------------------------------
//	¥ SendData
// ---------------------------------------------------------------------------

void
LFTPDataConnection::SendData(
	char*				inData,
	UInt32				totalSize,
	SProgressMessage&	inProgressMsg)
{
	if (!inProgressMsg.totalBytes)
		inProgressMsg.totalBytes = totalSize;

	UInt32 amountSent = 0;
	char * p = inData;
	UInt32 sendBufferSize = GetSendSize();

	while (amountSent < totalSize) {
		if (sendBufferSize > (totalSize - amountSent))
			sendBufferSize = totalSize - amountSent;

		mEndpoint->Send(p, sendBufferSize);
		p += sendBufferSize;

		amountSent += sendBufferSize;
		inProgressMsg.completedBytes += sendBufferSize;
		//Forces send of broadcast message on last group of data...
		//this ensures that we will broadcast at least once for each SendData call
		BroadcastProgress(msg_SendingData, inProgressMsg, !(amountSent < totalSize));
	}
}


// ---------------------------------------------------------------------------
//	¥ BroadcastProgress
// ---------------------------------------------------------------------------

void
LFTPDataConnection::BroadcastProgress(
			MessageT				inMessage,
			SProgressMessage&		inProgressMsg,
			Boolean					inForceBroadcast)
{
	mMasterConnection->BroadcastProgress(inMessage, inProgressMsg, inForceBroadcast);
}

// ===========================================================================
#pragma mark -


LConnectorThread::LConnectorThread(
	LFTPDataConnection*	inMaster)
		: LThread(false, thread_DefaultStack, threadOption_Default, nil),
		  mMaster(inMaster)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LConnectorThread
// ---------------------------------------------------------------------------

LConnectorThread::~LConnectorThread()
{
}


// ---------------------------------------------------------------------------
//	¥ Run
// ---------------------------------------------------------------------------

void*
LConnectorThread::Run()
{
	try {
		mMaster->mEndpoint->Listen();
		mMaster->mEndpoint->AcceptIncoming(mMaster->mEndpoint);
	}
	catch(...) {
	}

	mMaster->ConnectThreadDone();
	return nil;
}


PP_End_Namespace_PowerPlant
