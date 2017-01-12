// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LURL.h						PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Class for parsing and creating URL's. Many thanks to Donald E. Carlile
//	for his submission of the udpates to this class.

#ifndef _H_LURL
#define _H_LURL
#pragma once

#include <LFTPConnection.h>
#include <cstring>
#include <string>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

const char kURLFTPType[]	= "ftp";
const char kURLMAILTOType[]	= "mailto";
const char kURLHTTPType[]	= "http";
const char kURLFILEType[]	= "file";

		// Specific URL Types supported by this object. Others
		// will be parsed into scheme and schemepart only
enum EURLType {
	url_Unknown = 0,
	url_FTP,
	url_MAILTO,
	url_HTTP,
	url_FILE
};

typedef enum EncodingMask {
    url_Aplha			= 0x1,
    url_PAlpha			= 0x2,
    url_Path            = 0x4
} EncodingMask;

// ---------------------------------------------------------------------------

class	LURL {
public:
							LURL();
							LURL(const char * inURL);
							LURL(const char * inURL, SInt32 inLength);
							LURL(EURLType inURLType, const char * inSchemePart);
	virtual					~LURL();

	virtual void			SetURL(const char * inURL, SInt32 inLength = 0);
	virtual const char *	GetURL();

	virtual void			SetScheme(const char * inURL);
	virtual void			SetScheme(EURLType inURLType);
	virtual const char *	GetScheme();
	virtual EURLType		GetSchemeType();

	virtual void			SetSchemePart(const char * inURL);
	virtual const char *	GetSchemePart();

	virtual void			SetHost(const char * inHost);
	virtual const char *	GetHost() const
								{ return mHost.c_str(); }

	virtual void			SetPort(SInt16 inPort)
								{ mPort = inPort; BuildURL(); }
	virtual SInt16			GetPort() const
								{ return mPort; }

	//Protocol Specific
	virtual void			SetUserName(const char * inUsername)
								{ mUsername = inUsername; BuildURL(); }
	virtual const char *	GetUserName() const
								{ return mUsername.c_str(); }

	virtual void			SetPassword(const char * inPassword);
	virtual const char *	GetPassword() const
								{ return mPassword.c_str(); }

	virtual void			SetPath(const char * inPath)
								{ mPath = inPath; BuildURL();}
	virtual const char *	GetPath() const
								{ return mPath.c_str(); }

	virtual void			SetSearchPart(const char * inSearch)
								{ mHTTPSearchPart = inSearch; }
	virtual const char *	GetSearchPart() const
								{ return mHTTPSearchPart.c_str(); }

	virtual void			SetFTPTypeCode(EFTPTransferType inTypeCode)
								{ mFTPTypeCode = inTypeCode; }

	virtual EFTPTransferType	GetFTPTypeCode() const
								{ return mFTPTypeCode; }
protected:

	virtual void			InitMembers();
	virtual void			ResetMembers();
	virtual void			BuildURL();
	virtual void			DecodeReservedChars(char * tempURL);
	virtual void			EncodeReservedChars (const char * str,
												PP_STD::string & outEncoded,
												EncodingMask mask = url_Path);
	char					ConvertHex(char inChar);

	PP_STD::string			mURL;
	PP_STD::string			mScheme;
	PP_STD::string			mSchemePart;

	//Protocol Specific
	EURLType 				mSchemeType;
	PP_STD::string			mUsername;
	PP_STD::string			mPassword;
	Boolean					mHavePassword;
	PP_STD::string			mHost;
	SInt16 					mPort;
	PP_STD::string			mPath;
	EFTPTransferType		mFTPTypeCode;
	PP_STD::string			mHTTPSearchPart;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
