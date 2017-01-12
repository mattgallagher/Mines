// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UInternetProtocol.cp		PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <UInternetProtocol.h>
#include <UInternetConfig.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//		¥ FTP_PutFile()
// ---------------------------------------------------------------------------

LSimpleFTPThread *
UInternetProtocol::FTP_PutFile(
	LListener *			inListener,
	EFTPTransferType	inType,
	LFile * 			inFile)
{
	ThrowIfNot_(UInternetConfig::PP_ICAvailable());

	//Assume IC Host info
	Str255 host;
	ThrowIfOSStatus_(UInternetConfig::PP_ICGetFTPHost(host));

	//Get file name from the file object
	char fileName[256];
	FSSpec	tempSpec;
	inFile->GetSpecifier(tempSpec);
	PP_CSTD::strncpy(fileName, (const char *)&(tempSpec.name[1]), tempSpec.name[0]);

	return UInternetProtocol::FTP_PutFile(
					inListener,
					host,
					nil,
					nil,
					nil,
					inType,
					fileName,
					inFile,
					nil,
					true,
					false);
}

// ---------------------------------------------------------------------------
//		¥ FTP_PutFile()
// ---------------------------------------------------------------------------

LSimpleFTPThread *
UInternetProtocol::FTP_PutFile(
	LListener *			inListener,
	ConstStr255Param	inRemoteHost,
	const char * 		inUser,
	const char *		inPassword,
	const char *		inAccount,
	EFTPTransferType	inType,
	const char *		inFileName,
	LFile * 			inFile,
	char *				outFileName,
	Boolean 			createUnique,
	Boolean				usePassive,
	SInt16				inRemotePort)
{
	LFTPEventElem * newEvent = new LFTPEventElem(FTP_PutFileAutoEvent);

	newEvent->mRemoteHost = inRemoteHost;
	PP_CSTD::strcpy(newEvent->mUser, inUser);
	PP_CSTD::strcpy(newEvent->mPassword, inPassword);
	if (inAccount) {
		PP_CSTD::strcpy(newEvent->mAccount, inAccount);
	}
	PP_CSTD::strcpy(newEvent->mOriginalFileName, inFileName);
	newEvent->mXferType = inType;
	newEvent->mFile = inFile;
	newEvent->mReturnFileName = outFileName;
	newEvent->mCreateUnique = createUnique;
	newEvent->mUsePassive = usePassive;
	newEvent->mRemotePort = inRemotePort;

	LSimpleFTPThread * ftpThread = new LSimpleFTPThread(inListener);
	ftpThread->QueueEvent(newEvent);
	return ftpThread;
}

// ---------------------------------------------------------------------------
//		¥ FTP_GetFile()
// ---------------------------------------------------------------------------

LSimpleFTPThread *
UInternetProtocol::FTP_GetFile(
	LListener *			inListener,
	LURL& 				inURL,
	LFile *				outFile)
{
	//Get Host and port from URL
	LStr255 host = inURL.GetHost();
	ThrowIf_(host.Length() == 0);

	SInt16 port = inURL.GetPort();
	if (!port)
		port = kFTPPort;

	//Get Username and Password from the URL if available
	PP_STD::string user = inURL.GetUserName();
	PP_STD::string password = inURL.GetPassword();

	//We default to FTPASCIIXfer if it's not stated in the URL
	EFTPTransferType typeCode = inURL.GetFTPTypeCode();
	if (typeCode == FTPUndefinedXfer)
		typeCode = FTPASCIIXfer;

	PP_STD::string path = inURL.GetPath();
	//Check to see if URL is simply a path and not a complete file specification
	ThrowIf_ (path[path.length() - 1] == '/');

	//If no out file specified then attempt to create one from the
	//	IC Prefs and the URL file name
	if (!outFile) {
		//get file name from path
		PP_CSTD::size_t lastSlash = path.find_last_of('/');
		lastSlash++;
		PP_STD::string filename(&path[lastSlash], path.length() - lastSlash);

		ThrowIfNot_(UInternetConfig::PP_ICAvailable());

		//Get IC's default save directory info
		FSSpec theSaveFile;
		ThrowIfOSStatus_(UInternetConfig::PP_ICGetDownloadFolder(theSaveFile));

		//put file name into the FSSpec
		PP_CSTD::size_t nameLen = filename.length() < sizeof(Str32) ? filename.length() : sizeof(Str32);
		::BlockMoveData(filename.c_str(), theSaveFile.name + 1, (SInt32) nameLen);
		theSaveFile.name[0] = (unsigned char) nameLen;

		//Create the new file
		outFile = new LFile(theSaveFile);
	}

	//Get the file
	return UInternetProtocol::FTP_GetFile(
							inListener,
							host,
							user.c_str(),
							password.c_str(),
							nil,
							typeCode,
							path.c_str(),
							outFile,
							false,
							port);
}

// ---------------------------------------------------------------------------
//		¥ FTP_GetFile()
// ---------------------------------------------------------------------------

LSimpleFTPThread *
UInternetProtocol::FTP_GetFile(
	LListener *			inListener,
	ConstStr255Param	inRemoteHost,
	const char * 		inUser,
	const char * 		inPassword,
	const char * 		inAccount,
	EFTPTransferType	inType,
	const char * 		inFileName,
	LFile * 			inFile,
	Boolean				usePassive,
	SInt16				inRemotePort)
{
	LFTPEventElem * newEvent = new LFTPEventElem(FTP_GetFileAutoEvent);

	newEvent->mRemoteHost = inRemoteHost;
	PP_CSTD::strcpy(newEvent->mUser, inUser);
	PP_CSTD::strcpy(newEvent->mPassword, inPassword);
	if (inAccount) {
		PP_CSTD::strcpy(newEvent->mAccount, inAccount);
	}
	newEvent->mXferType = inType;
	PP_CSTD::strcpy(newEvent->mOriginalFileName, inFileName);
	newEvent->mFile = inFile;
	newEvent->mUsePassive = usePassive;
	newEvent->mRemotePort = inRemotePort;

	LSimpleFTPThread * ftpThread = new LSimpleFTPThread(inListener);
	ftpThread->QueueEvent(newEvent);
	return ftpThread;
}

// ---------------------------------------------------------------------------
//		¥ SMTP_SendOneMessage()
// ---------------------------------------------------------------------------

LSimpleSMTPThread *
UInternetProtocol::SMTP_SendOneMessage(
	LListener *			inListener,
	ConstStr255Param	inRemoteHost,
	LMailMessage*		theMessage,
	SInt16				inRemotePort)
{
	LSMTPEventElem * newEvent = new LSMTPEventElem(SMTP_SendOneMessageEvent);
	newEvent->mRemoteHost = inRemoteHost;
	newEvent->mMailMessage = theMessage;
	newEvent->mRemotePort = inRemotePort;

	LSimpleSMTPThread * smtpThread = new LSimpleSMTPThread(inListener);
	smtpThread->QueueEvent(newEvent);
	return smtpThread;
}

// ---------------------------------------------------------------------------
//		¥ SMTP_SendOneMessage()
// ---------------------------------------------------------------------------

LSimpleSMTPThread *
UInternetProtocol::SMTP_SendOneMessage(
	LListener *			inListener,
	LMailMessage*		theMessage)
{
	ThrowIfNot_(UInternetConfig::PP_ICAvailable());

	LSMTPEventElem * newEvent = new LSMTPEventElem(SMTP_SendOneMessageEvent);
	Str255 host;
	ThrowIfOSStatus_(UInternetConfig::PP_ICGetSMTPHost(host));
	newEvent->mRemoteHost = host;
	newEvent->mMailMessage = theMessage;

	LSimpleSMTPThread * smtpThread = new LSimpleSMTPThread(inListener);
	smtpThread->QueueEvent(newEvent);
	return smtpThread;
}

// ---------------------------------------------------------------------------
//		¥ HTTP_Get()
// ---------------------------------------------------------------------------

LSimpleHTTPThread *
UInternetProtocol::HTTP_Get(
	LListener *			inListener,
	// ADM 000413 - Bug #IL9903-1688 - Block Start
	// ADM 000413 - Bug #IL9903-1688 - Update function prototype to take
	//				optional LHTTPMessage parameter.
	//LURL &				inURL)	// ADM 000413
	LURL &				inURL,
	LHTTPMessage*		theMessage)
	// ADM 000413 - Bug #IL9903-1688 - Block End
{
	//Get Host and port from URL
	LStr255 host = inURL.GetHost();
	ThrowIf_(host.Length() == 0);

	SInt16 port = inURL.GetPort();
	if (!port)
		port = kHTTPPort;

	//Get Username and Password from the URL if available
	PP_STD::string user = inURL.GetUserName();
	PP_STD::string password = inURL.GetPassword();

	//Get request portion of the URL
	PP_STD::string path = inURL.GetPath();
	LStr255 theRequest = (const char*)path.c_str();

	// ADM 000411 - Bug #WB1-9657 - Block Start
	// ADM 000411 - Bug #WB1-9657 - If a search query is part of the URL, then
	//				append the search string to the request. See LURL::GetSchemePart
	//				and LURL::SetSchemePart.
	PP_STD::string searchPart = inURL.GetSearchPart();
	if(searchPart.length()) {
		theRequest += '?';
		theRequest += searchPart.c_str();
	}
	// ADM 000411 - Bug #WB1-9657 - Block End

	// ADM 000413 - Bug #IL9903-1688 - Block Start
	// ADM 000413 - Bug #IL9903-1688 - Check if LHTTPMessage parameter is
	//				valid. If invalid, allocate LHTTPMessage message.
	//LHTTPMessage * theMessage = new LHTTPMessage();	// ADM 000413
	if (!theMessage) {
		theMessage = new LHTTPMessage();
		if (user.length()) {
			theMessage->SetUserName(user.c_str());
			if (password.length()) {
				theMessage->SetPassword(password.c_str());
			}
		}
	}
	//if (user.length()) {								// ADM 000413
	//	theMessage->SetUserName(user.c_str());			// ADM 000413
	//	if (password.length()) {						// ADM 000413
	//		theMessage->SetPassword(password.c_str());	// ADM 000413
	//	}												// ADM 000413
	//}													// ADM 000413
	// ADM 000413 - Bug #IL9903-1688 - Block End

	return UInternetProtocol::HTTP_Get(
			inListener,
			host,
			theRequest,
			theMessage,
			port);
}

// ---------------------------------------------------------------------------
//		¥ HTTP_Get()
// ---------------------------------------------------------------------------

LSimpleHTTPThread *
UInternetProtocol::HTTP_Get(
	LListener *			inListener,
	ConstStr255Param	inRemoteHost,
	ConstStr255Param	inRequest,
	LHTTPMessage*		theMessage,
	SInt16				inRemotePort)
{
	if (!theMessage) {
		theMessage = new LHTTPMessage();
	}

	LHTTPEventElem * newEvent = new LHTTPEventElem(HTTP_GetEvent);
	newEvent->mRemoteHost = inRemoteHost;
	newEvent->mRequest = inRequest;
	newEvent->mMessage = theMessage;
	newEvent->mRemotePort = inRemotePort;

	LSimpleHTTPThread * httpThread = new LSimpleHTTPThread(inListener);
	httpThread->QueueEvent(newEvent);
	return httpThread;
}

// ---------------------------------------------------------------------------
//		¥ HTTP_Post()
// ---------------------------------------------------------------------------

LSimpleHTTPThread *
UInternetProtocol::HTTP_Post(
	LListener *			inListener,
	// ADM 000413 - Bug #IL9903-1688 - Block Start
	// ADM 000413 - Bug #IL9903-1688 - Update function prototype to take
	//				optional LHTTPMessage parameter.
	//LURL &				inURL)	// ADM 000413
	LURL &				inURL,
	LHTTPMessage*		theMessage)
	// ADM 000413 - Bug #IL9903-1688 - Block End
{
	//Get Host and port from URL
	LStr255 host = inURL.GetHost();
	ThrowIf_(host.Length() == 0);

	SInt16 port = inURL.GetPort();
	if (!port)
		port = kHTTPPort;

	//Get Username and Password from the URL if available
	PP_STD::string user = inURL.GetUserName();
	PP_STD::string password = inURL.GetPassword();

	//Get request portion of the URL
	PP_STD::string path = inURL.GetPath();
	LStr255 theRequest = path.c_str();

	// ADM 000413 - Bug #WB1-9657 - Block Start
	// ADM 000413 - Bug #WB1-9657 - If a search query is part of the URL, then
	//				append the search string to the request. See LURL::GetSchemePart
	//				and LURL::SetSchemePart.
	PP_STD::string searchPart = inURL.GetSearchPart();
	if(searchPart.length()) {
		theRequest += '?';
		theRequest += searchPart.c_str();
	}
	// ADM 000413 - Bug #WB1-9657 - Block End

	// ADM 000413 - Bug #IL9903-1688 - Block Start
	// ADM 000413 - Bug #IL9903-1688 - Check if LHTTPMessage parameter is
	//				valid. If invalid, allocate LHTTPMessage message.
	//LHTTPMessage * theMessage = new LHTTPMessage();	// ADM 000413
	if (!theMessage) {
		theMessage = new LHTTPMessage();
		if (user.length()) {
			theMessage->SetUserName(user.c_str());
			if (password.length()) {
				theMessage->SetPassword(password.c_str());
			}
		}
	}
	//if (user.length()) {								// ADM 000413
	//	theMessage->SetUserName(user.c_str());			// ADM 000413
	//	if (password.length()) {						// ADM 000413
	//		theMessage->SetPassword(password.c_str());	// ADM 000413
	//	}												// ADM 000413
	//}													// ADM 000413
	// ADM 000413 - Bug #IL9903-1688 - Block End

	return UInternetProtocol::HTTP_Post(
			inListener,
			host,
			theRequest,
			theMessage,
			port);
}

// ---------------------------------------------------------------------------
//		¥ HTTP_Post()
// ---------------------------------------------------------------------------

LSimpleHTTPThread *
UInternetProtocol::HTTP_Post(
	LListener *			inListener,
	ConstStr255Param	inRemoteHost,
	ConstStr255Param	inRequest,
	LHTTPMessage*		theMessage,
	SInt16				inRemotePort)
{
	if (!theMessage) {
		theMessage = new LHTTPMessage();
	}

	LHTTPEventElem * newEvent = new LHTTPEventElem(HTTP_PostEvent);
	newEvent->mRemoteHost = inRemoteHost;
	newEvent->mRequest = inRequest;
	newEvent->mMessage = theMessage;
	newEvent->mRemotePort = inRemotePort;

	LSimpleHTTPThread * httpThread = new LSimpleHTTPThread(inListener);
	httpThread->QueueEvent(newEvent);
	return httpThread;
}

// ADM 000413 - Bug #IL9903-1688 - Block Start
// ADM 000413 - Bug #IL9903-1688 - Add HTTP_Head function that accepts
//				LURL parameter.
// ---------------------------------------------------------------------------
//		¥ HTTP_Head()
// ---------------------------------------------------------------------------

LSimpleHTTPThread *
UInternetProtocol::HTTP_Head(
	LListener *			inListener,
	// ADM 000413 - Bug #IL9903-1688 - Block Start
	// ADM 000413 - Bug #IL9903-1688 - Update function prototype to take
	//				optional LHTTPMessage parameter.
	//LURL &				inURL)	// ADM 000413
	LURL &				inURL,
	LHTTPMessage*		theMessage)
	// ADM 000413 - Bug #IL9903-1688 - Block End
{
	//Get Host and port from URL
	LStr255 host = inURL.GetHost();
	ThrowIf_(host.Length() == 0);

	SInt16 port = inURL.GetPort();
	if (!port)
		port = kHTTPPort;

	//Get Username and Password from the URL if available
	PP_STD::string user = inURL.GetUserName();
	PP_STD::string password = inURL.GetPassword();

	//Get request portion of the URL
	PP_STD::string path = inURL.GetPath();
	LStr255 theRequest = (const char*)path.c_str();

	// ADM 000411 - Bug #WB1-9657 - Block Start
	// ADM 000411 - Bug #WB1-9657 - If a search query is part of the URL, then
	//				append the search string to the request. See LURL::GetSchemePart
	//				and LURL::SetSchemePart.
	PP_STD::string searchPart = inURL.GetSearchPart();
	if(searchPart.length()) {
		theRequest += '?';
		theRequest += searchPart.c_str();
	}
	// ADM 000411 - Bug #WB1-9657 - Block End

	// ADM 000413 - Bug #IL9903-1688 - Block Start
	// ADM 000413 - Bug #IL9903-1688 - Check if LHTTPMessage parameter is
	//				valid. If invalid, allocate LHTTPMessage message.
	//LHTTPMessage * theMessage = new LHTTPMessage();	// ADM 000413
	if (!theMessage) {
		theMessage = new LHTTPMessage();
		if (user.length()) {
			theMessage->SetUserName(user.c_str());
			if (password.length()) {
				theMessage->SetPassword(password.c_str());
			}
		}
	}
	//if (user.length()) {								// ADM 000413
	//	theMessage->SetUserName(user.c_str());			// ADM 000413
	//	if (password.length()) {						// ADM 000413
	//		theMessage->SetPassword(password.c_str());	// ADM 000413
	//	}												// ADM 000413
	//}													// ADM 000413
	// ADM 000413 - Bug #IL9903-1688 - Block End

	return UInternetProtocol::HTTP_Head(
			inListener,
			host,
			theRequest,
			theMessage,
			port);
}
// ADM 000413 - Bug #IL9903-1688 - Block End

// ---------------------------------------------------------------------------
//		¥ HTTP_Head()
// ---------------------------------------------------------------------------

LSimpleHTTPThread *
UInternetProtocol::HTTP_Head(
	LListener *			inListener,
	ConstStr255Param	inRemoteHost,
	ConstStr255Param	inRequest,
	LHTTPMessage*		theMessage,
	SInt16				inRemotePort)
{
	if (!theMessage) {
		theMessage = new LHTTPMessage();
	}

	LHTTPEventElem * newEvent = new LHTTPEventElem(HTTP_HeadEvent);
	newEvent->mRemoteHost = inRemoteHost;
	newEvent->mRequest = inRequest;
	newEvent->mMessage = theMessage;
	newEvent->mRemotePort = inRemotePort;

	LSimpleHTTPThread * httpThread = new LSimpleHTTPThread(inListener);
	httpThread->QueueEvent(newEvent);
	return httpThread;
}

// ADM 000413 - Bug #IL9903-1688 - Block Start
// ADM 000413 - Bug #IL9903-1688 - Add HTTP_Put function that accepts
//				LURL parameter.
// ---------------------------------------------------------------------------
//		¥ HTTP_Put()
// ---------------------------------------------------------------------------

LSimpleHTTPThread *
UInternetProtocol::HTTP_Put(
	LListener *			inListener,
	// ADM 000413 - Bug #IL9903-1688 - Block Start
	// ADM 000413 - Bug #IL9903-1688 - Update function prototype to take
	//				optional LHTTPMessage parameter.
	//LURL &				inURL)	// ADM 000413
	LURL &				inURL,
	LHTTPMessage*		theMessage)
	// ADM 000413 - Bug #IL9903-1688 - Block End
{
	//Get Host and port from URL
	LStr255 host = inURL.GetHost();
	ThrowIf_(host.Length() == 0);

	SInt16 port = inURL.GetPort();
	if (!port)
		port = kHTTPPort;

	//Get Username and Password from the URL if available
	PP_STD::string user = inURL.GetUserName();
	PP_STD::string password = inURL.GetPassword();

	//Get request portion of the URL
	PP_STD::string path = inURL.GetPath();
	LStr255 theRequest = (const char*)path.c_str();

	// ADM 000411 - Bug #WB1-9657 - Block Start
	// ADM 000411 - Bug #WB1-9657 - If a search query is part of the URL, then
	//				append the search string to the request. See LURL::GetSchemePart
	//				and LURL::SetSchemePart.
	PP_STD::string searchPart = inURL.GetSearchPart();
	if(searchPart.length()) {
		theRequest += '?';
		theRequest += searchPart.c_str();
	}
	// ADM 000411 - Bug #WB1-9657 - Block End

	// ADM 000413 - Bug #IL9903-1688 - Block Start
	// ADM 000413 - Bug #IL9903-1688 - Check if LHTTPMessage parameter is
	//				valid. If invalid, allocate LHTTPMessage message.
	//LHTTPMessage * theMessage = new LHTTPMessage();	// ADM 000413
	if (!theMessage) {
		theMessage = new LHTTPMessage();
		if (user.length()) {
			theMessage->SetUserName(user.c_str());
			if (password.length()) {
				theMessage->SetPassword(password.c_str());
			}
		}
	}
	//if (user.length()) {								// ADM 000413
	//	theMessage->SetUserName(user.c_str());			// ADM 000413
	//	if (password.length()) {						// ADM 000413
	//		theMessage->SetPassword(password.c_str());	// ADM 000413
	//	}												// ADM 000413
	//}													// ADM 000413
	// ADM 000413 - Bug #IL9903-1688 - Block End

	return UInternetProtocol::HTTP_Put(
			inListener,
			host,
			theRequest,
			theMessage,
			port);
}
// ADM 000413 - Bug #IL9903-1688 - Block End

// ---------------------------------------------------------------------------
//		¥ HTTP_Put()
// ---------------------------------------------------------------------------

LSimpleHTTPThread *
UInternetProtocol::HTTP_Put(
	LListener *			inListener,
	ConstStr255Param	inRemoteHost,
	ConstStr255Param	inRequest,
	LHTTPMessage*		theMessage,
	SInt16				inRemotePort)
{
	if (!theMessage) {
		theMessage = new LHTTPMessage();
	}

	LHTTPEventElem * newEvent = new LHTTPEventElem(HTTP_PutEvent);
	newEvent->mRemoteHost = inRemoteHost;
	newEvent->mRequest = inRequest;
	newEvent->mMessage = theMessage;
	newEvent->mRemotePort = inRemotePort;

	LSimpleHTTPThread * httpThread = new LSimpleHTTPThread(inListener);
	httpThread->QueueEvent(newEvent);
	return httpThread;
}

PP_End_Namespace_PowerPlant
