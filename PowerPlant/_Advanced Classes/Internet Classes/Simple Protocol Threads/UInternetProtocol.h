// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UInternetProtocol.h			PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UInternetProtocol
#define _H_UInternetProtocol
#pragma once

#include <LSimpleFTPThread.h>
#include <LSimpleSMTPThread.h>
#include <LSimpleHTTPThread.h>
#include <LURL.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class UInternetProtocol {
public:
	static LSimpleFTPThread *
					FTP_PutFile(
							LListener *			inListener,
							EFTPTransferType	inType,
							LFile * 			inFile);

	static LSimpleFTPThread *
					FTP_PutFile(
							LListener *			inListener,
							ConstStr255Param	inRemoteHost,
							const char * 		inUser,
							const char * 		inPassword,
							const char * 		inAccount,
							EFTPTransferType	inType,
							const char * 		inFileName,
							LFile * 			inFile,
							char *				outFileName = nil,
							Boolean 			createUnique = false,
							Boolean				usePassive = false,
							SInt16				inRemotePort = kFTPPort);

	static LSimpleFTPThread *
					FTP_GetFile(
							LListener *			inListener,
							LURL& 				inURL,
							LFile *				outFile = nil);

	static LSimpleFTPThread *
					FTP_GetFile(
							LListener *			inListener,
							ConstStr255Param	inRemoteHost,
							const char * 		inUser,
							const char * 		inPassword,
							const char * 		inAccount,
							EFTPTransferType	inType,
							const char * 		inFileName,
							LFile * 			inFile,
							Boolean				usePassive = false,
							SInt16				inRemotePort = kFTPPort);

	static LSimpleSMTPThread *
					SMTP_SendOneMessage(
							LListener *			inListener,
							ConstStr255Param	inRemoteHost,
							LMailMessage*		theMessage,
							SInt16				inRemotePort = kSMTPPort);

	static LSimpleSMTPThread *
					SMTP_SendOneMessage(
							LListener *			inListener,
							LMailMessage*		theMessage);


	static LSimpleHTTPThread *
					HTTP_Get(
							LListener *			inListener,
							ConstStr255Param	inRemoteHost,
							ConstStr255Param	inRequest,
							LHTTPMessage*		theMessage = nil,
							SInt16				inRemotePort = kHTTPPort);

	static LSimpleHTTPThread *
					HTTP_Get(
							LListener *			inListener,
							LURL &				inURL,
							LHTTPMessage*		theMessage = nil);

	static LSimpleHTTPThread *
					HTTP_Head(
							LListener *			inListener,
							ConstStr255Param	inRemoteHost,
							ConstStr255Param	inRequest,
							LHTTPMessage*		theMessage = nil,
							SInt16				inRemotePort = kHTTPPort);

	static LSimpleHTTPThread *
					HTTP_Head(
							LListener *			inListener,
							LURL &				inURL,
							LHTTPMessage*		theMessage = nil);

	static LSimpleHTTPThread *
					HTTP_Post(
							LListener *			inListener,
							ConstStr255Param	inRemoteHost,
							ConstStr255Param	inRequest,
							LHTTPMessage*		theMessage = nil,
							SInt16				inRemotePort = kHTTPPort);

	static LSimpleHTTPThread *
					HTTP_Post(
							LListener *			inListener,
							LURL &				inURL,
							LHTTPMessage*		theMessage = nil);

	static LSimpleHTTPThread *
					HTTP_Put(
							LListener *			inListener,
							ConstStr255Param	inRemoteHost,
							ConstStr255Param	inRequest,
							LHTTPMessage*		theMessage = nil,
							SInt16				inRemotePort = kHTTPPort);

	static LSimpleHTTPThread *
					HTTP_Put(
							LListener *			inListener,
							LURL &				inURL,
							LHTTPMessage*		theMessage = nil);
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
