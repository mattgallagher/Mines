// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LURL.cp						PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Class for parsing and creating URL's. Many thanks to Donald E. Carlile
//	for his submission of the udpates to this class.
//
//	NOTE:	This is intended to be a simple, general URL class. It does
//			however, know about specifics of common URL's like FTP, HTTP,
//			MAILTO, and FILE. For these URL's, further protocol specific
//			information is accessible. Other URL's will be parsed into scheme
//			and schemepart only.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LURL.h>

#include <cctype>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

unsigned char URLAcceptable[96] =
{/* 0 1 2 3 4 5 6 7 8 9 A B C D E F */
    0,0,0,0,0,0,0,0,0,0,7,6,0,7,7,4,		/* 2x   !"#$%&'()*+,-./	 */
    7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,0,		/* 3x  0123456789:;<=>?	 */
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,		/* 4x  @ABCDEFGHIJKLMNO  */
    7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,7,		/* 5X  PQRSTUVWXYZ[\]^_	 */
    0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,		/* 6x  `abcdefghijklmno	 */
    7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,0			/* 7X  pqrstuvwxyz{\}~	DEL */
};

char *hexchars = "0123456789ABCDEF";

// ---------------------------------------------------------------------------
//	¥ LURL									Default Constructor		  [public]
// ---------------------------------------------------------------------------

LURL::LURL()
{
	InitMembers();
}

// ---------------------------------------------------------------------------
//	¥ LURL									Constructor				  [public]
// ---------------------------------------------------------------------------

LURL::LURL(const char * inURL)
{
	InitMembers();
	SetURL(inURL);
}

// ---------------------------------------------------------------------------
//	¥ LURL									Constructor				  [public]
// ---------------------------------------------------------------------------

LURL::LURL(const char * inURL, SInt32 inLength)
{
	InitMembers();
	SetURL(inURL, inLength);
}

// ---------------------------------------------------------------------------
//	¥ LURL									Constructor				  [public]
// ---------------------------------------------------------------------------

LURL::LURL(EURLType inURLType, const char * inSchemePart)
{
	InitMembers();
	SetScheme(inURLType);
	SetSchemePart(inSchemePart);
}

// ---------------------------------------------------------------------------
//	¥ ~LURL									Destructor				  [public]
// ---------------------------------------------------------------------------

LURL::~LURL()
{
}

// ---------------------------------------------------------------------------
//		¥ InitMembers
// ---------------------------------------------------------------------------
//	Sets initial values for member variables

void
LURL::InitMembers()
{
	mPort = 0;
	mFTPTypeCode = FTPUndefinedXfer;
	mHavePassword = false;
}

// ---------------------------------------------------------------------------
//		¥ ResetMembers
// ---------------------------------------------------------------------------
//	Resets initial values for member variables

void
LURL::ResetMembers()
{
	mURL = "";
	mScheme = "";
	mSchemePart = "";
	mUsername = "";
	mPassword = "";
	mHavePassword = false;
	mHost = "";
	mPort = 0;
	mPath = "";
	mFTPTypeCode = FTPUndefinedXfer;
	mHTTPSearchPart = "";
}

// ---------------------------------------------------------------------------
//		¥ BuildURL
// ---------------------------------------------------------------------------
//	Builds URL as C string

void
LURL::BuildURL()
{
	mURL = "";
	mSchemePart = "";
	mURL += GetScheme();
	mURL += ':';
	mURL += GetSchemePart();
}

// ---------------------------------------------------------------------------
//		¥ SetURL
// ---------------------------------------------------------------------------
//	Sets URL from C string

void
LURL::SetURL(const char * inURL, SInt32 inLength)
{
	if (!inLength) {
		inLength = (SInt32) (PP_CSTD::strlen(inURL));
	}

	mURL.assign(inURL, (UInt32) inLength);

	const char * schemeSep = PP_CSTD::strchr(inURL, ':');
	if (!schemeSep)
		return;

	PP_STD::string temp;
	SInt32 schemeLen  = schemeSep - inURL;

	temp.assign(inURL, (UInt32) schemeLen);
	SetScheme(temp.c_str());

	temp.assign((schemeSep + 1), (UInt32) (inLength - schemeLen - 1));
	SetSchemePart(temp.c_str());
}


// ---------------------------------------------------------------------------
//		¥ GetURL
// ---------------------------------------------------------------------------
//	Returns URL as C string

const char *
LURL::GetURL()
{
	//Build URL if necessary
	if (mURL.length() == 0) {
		mURL += GetScheme();
		mURL += ':';
		mURL += GetSchemePart();
	}

	return mURL.c_str();
}

// ---------------------------------------------------------------------------
//		¥ SetScheme
// ---------------------------------------------------------------------------
//	Sets URL from a EURLType

void
LURL::SetScheme(EURLType inURLType)
{
	mSchemeType = inURLType;
	mScheme = "";
	BuildURL();
}


// ---------------------------------------------------------------------------
//		¥ SetScheme
// ---------------------------------------------------------------------------
//	Sets URL from C string

void
LURL::SetScheme(const char * inURL)
{
	mScheme = inURL;
	mSchemeType = url_Unknown;
}

// ---------------------------------------------------------------------------
//		¥ GetSchemeType
// ---------------------------------------------------------------------------
//	Return scheme as EURLType

EURLType
LURL::GetSchemeType()
{
	if (mSchemeType == url_Unknown) {
		char tempScheme[513];
		PP_CSTD::strcpy(tempScheme, GetScheme());
		LowercaseText(tempScheme, (SInt16) PP_CSTD::strlen(tempScheme), nil);

		if (PP_CSTD::strcmp(tempScheme, "") == 0)
			mSchemeType = url_Unknown;

		else if (PP_CSTD::strcmp(tempScheme, kURLFTPType) == 0)
			mSchemeType = url_FTP;

		else if (PP_CSTD::strcmp(tempScheme, kURLMAILTOType) == 0)
			mSchemeType = url_MAILTO;

		else if (PP_CSTD::strcmp(tempScheme, kURLHTTPType) == 0)
			mSchemeType = url_HTTP;

		else if (PP_CSTD::strcmp(tempScheme, kURLFILEType) == 0)
			mSchemeType = url_FILE;
	}

	return mSchemeType;
}

// ---------------------------------------------------------------------------
//		¥ GetScheme
// ---------------------------------------------------------------------------
//	Return scheme as C string

const char *
LURL::GetScheme()
{
	if (mScheme.length() == 0) {
		switch (mSchemeType) {
			case url_FTP:
				mScheme = kURLFTPType;
				break;
			case url_MAILTO:
				mScheme = kURLMAILTOType;
				break;
			case url_HTTP:
				mScheme = kURLHTTPType;
				break;
			case url_FILE:
				mScheme = kURLFILEType;
				break;

			case url_Unknown:
			default:
				break;
		}
	}

	return mScheme.c_str();
}


// ---------------------------------------------------------------------------
//		¥ SetSchemePart
// ---------------------------------------------------------------------------
//	Sets URL from C string. Attempts to parse user, password, host, port, and path

void
LURL::SetSchemePart(const char * inURL)
{
	mSchemePart = inURL;

	//check to make sure we have '//' at head... if not
	//	we don't bother trying to parse the scheme part.
	if (inURL[0] == '/' && inURL[1] == '/') {
		//make temporary copy to work with
		char * tempURL = new char[PP_CSTD::strlen(inURL)];

		//check for file URL
		if (inURL[2] =='/') {
			PP_CSTD::strcpy(tempURL, &inURL[3]);	//only copy stuff past the '///' for convenience
			mHost = "";
			mUsername = "";
			mPassword = "";
			mPort = 0;
			mPath = tempURL;
		} else {
			PP_CSTD::strcpy(tempURL, &inURL[2]);	//only copy stuff past the '//' for convenience

			char * currentOffset = PP_CSTD::strtok(tempURL, "/");

			// temporarily copy out host info to mHost
			mHost = currentOffset;
			// copy rest to mPath
			currentOffset = PP_CSTD::strtok(nil, "");
			// Path must begin with a slash
			if (currentOffset) {
				mPath = '/';
				mPath += currentOffset;
			} else {
				mPath = '/';
			}

			//Now go back to host and parse out user, password, host, and port
			//check for username:password@host:port
			PP_CSTD::strcpy(tempURL, mHost.c_str());
			currentOffset = PP_CSTD::strchr(tempURL, '@');
			if (currentOffset) {
				mUsername.assign(tempURL, (UInt32) (currentOffset - tempURL));
				currentOffset++;
				PP_CSTD::strcpy(tempURL, currentOffset);
			} else {
				mUsername = "";
			}

			//Check for host:port
			currentOffset = PP_CSTD::strchr(tempURL, ':');
			if (currentOffset) {
				mHost.assign(tempURL, (UInt32) (currentOffset - tempURL));
				currentOffset++;
				mPort = (SInt16) PP_CSTD::atoi(currentOffset);
			} else {
				mHost = tempURL;
				mPort = 0;
			}

			//go back and parse out username:password from mUsername
			PP_CSTD::strcpy(tempURL, mUsername.c_str());
			currentOffset = PP_CSTD::strchr(tempURL, ':');
			if (currentOffset) {
				mUsername.assign(tempURL, (UInt32) (currentOffset - tempURL));
				currentOffset++;
				mPassword = currentOffset;
				mHavePassword = true;
			} else {
				mUsername = tempURL;
				mPassword = "";
				mHavePassword = false;
			}

			//Protocol specific
			PP_CSTD::strcpy(tempURL, mPath.c_str());
			switch (GetSchemeType()) {
				case url_HTTP:	//check for and pull out <searchpart>
					if (PP_CSTD::strchr(tempURL, '?') != 0) {
						currentOffset = PP_CSTD::strtok(tempURL, "?");
						mPath = currentOffset;
						currentOffset = PP_CSTD::strtok(nil, "");
						mHTTPSearchPart = currentOffset;
					}
					break;

				case url_FTP:	//check for and pull out <typecode>
					if (PP_CSTD::strchr(tempURL, ';') != 0) {
						currentOffset = PP_CSTD::strtok(tempURL, ";");
						mPath = currentOffset;
						//get after semi-colon
						currentOffset = PP_CSTD::strtok(nil, "");
						//get after the equal sign
						currentOffset = PP_CSTD::strtok(currentOffset, "=");
						currentOffset = PP_CSTD::strtok(nil, "");
						currentOffset[0] = (char) PP_CSTD::toupper(currentOffset[0]);
						switch (currentOffset[0]) {
							case FTPASCIIXfer:
								mFTPTypeCode = FTPASCIIXfer;
								break;

							case FTPImageXfer:
								mFTPTypeCode = FTPImageXfer;
								break;

							default:
								mFTPTypeCode = FTPUndefinedXfer;
								break;
						}
					}
					break;

				default:
					break;
			}
		}

		if (GetSchemeType() != url_MAILTO) {
			//decode the path
			PP_CSTD::strcpy(tempURL, mPath.c_str());
			DecodeReservedChars(tempURL);
			mPath = tempURL;
		}

		delete[] tempURL;
	}

}

// ---------------------------------------------------------------------------
//		¥ GetSchemePart
// ---------------------------------------------------------------------------
//	Returns schemepart as C string
//  http and ftp URLs have the same format, viz., scheme://host/path
//  file URLS have the format file:///path, host being irrelevant
//  mailto URLs have the format mailto:user@host, path and password being irrelevant.

const char *
LURL::GetSchemePart()
{
	EURLType theURLType = GetSchemeType();
	PP_STD::string tempEncodedString;
	if (theURLType != url_MAILTO) {
		//encode the path
		EncodeReservedChars(mPath.c_str(), tempEncodedString);
	}

	PP_STD::string tempSchemePart = "";

	switch (theURLType) {
		case url_HTTP:
		case url_FTP:
			tempSchemePart += "//";
			if (mUsername.length()) {
				tempSchemePart += mUsername;
				if (mHavePassword) {
					tempSchemePart += ':';
					tempSchemePart += mPassword;
				}
				tempSchemePart += '@';
			}
			tempSchemePart += mHost;
			if (mPort) {
				tempSchemePart += ':';
				Str255 tempStr;
				NumToString(mPort,tempStr);
				tempSchemePart += LString::PToCStr(tempStr);
			}

			tempSchemePart += tempEncodedString;

			switch (GetSchemeType()) {
				case url_HTTP:	//add <searchpart> if necessary
					if(mHTTPSearchPart.length()) {
						tempSchemePart += '?';
						tempSchemePart += mHTTPSearchPart;
					}
					break;

				case url_FTP:	//add <typecode> if necessary
					if(mFTPTypeCode != FTPUndefinedXfer) {
						tempSchemePart += ";type=";
						tempSchemePart += mFTPTypeCode;
					}
					break;
				
				default:
					break;
			}
			break;
		case url_FILE:
			tempSchemePart += "///";
			tempSchemePart += tempEncodedString;
			break;
		case url_MAILTO:
			tempSchemePart += mUsername;
			tempSchemePart += '@';
			tempSchemePart += mHost;
			break;
		default:
			break;
	}
	mSchemePart = tempSchemePart;
	return mSchemePart.c_str();
}

// ---------------------------------------------------------------------------
//		¥ SetHost
// ---------------------------------------------------------------------------
// Most of this routine is intended to remove a trailing "/" on a host name.
void
LURL::SetHost(const char * inHost)
{
	PP_CSTD::size_t theSize = PP_CSTD::strlen(inHost) + 1;
	char* strPtr = NewPtr((SInt32) theSize);
	PP_CSTD::strcpy(strPtr,inHost);
	if (strPtr[theSize - 1] == '/') {
		strPtr[theSize - 1] = 0;
	}
	mHost = strPtr;
	DisposePtr(strPtr);
	BuildURL();
}

// ---------------------------------------------------------------------------
//		¥ SetPassword
// ---------------------------------------------------------------------------
void
LURL::SetPassword(const char * inPassword)
{
	mPassword = inPassword;
	if (!mPassword.length()) {
		mHavePassword = false;
	} else {
		mHavePassword = true;
	}
	BuildURL();
}

// ===========================================================================

#pragma mark -
#pragma mark === Utilities ===

// ---------------------------------------------------------------------------
//	¥ EncodeReservedChars
// ---------------------------------------------------------------------------
//	adds "%00" encoding to URLS

void
LURL::EncodeReservedChars(const char * str, PP_STD::string & outEncoded, EncodingMask mask)
{
    const char * p;

    for(p = str; *p; p++) {
    	unsigned char a = (unsigned char) *p;
		if (!(a >= 32 && a < 128 && ((URLAcceptable[a-32]) & mask))) {
		    outEncoded += '%';
		    outEncoded += hexchars[a >> 4];
		    outEncoded += hexchars[a & 15];
		} else
			outEncoded += *p;
    }
}


// ---------------------------------------------------------------------------
//	¥ DecodeReservedChars
// ---------------------------------------------------------------------------
//	removes "%00" encoding from URLS

void
LURL::DecodeReservedChars(char * inURL)
{
    char * p = inURL;
    char * q = inURL;

    while(*p) {
	    if (*p == '%') {
		    p++;
		    if (*p)
		    	*q = (char) (ConvertHex(*p++) * 16);
		    if (*p)
		    	*q = (char) (*q + ConvertHex(*p));
		    p++, q++;
		} else {
		    *q++ = *p++;
		}
    }

    *q++ = 0;
}


// ---------------------------------------------------------------------------
//	¥ ConvertHex
// ---------------------------------------------------------------------------
//	Return decimal value for a hexadecimal character

char
LURL::ConvertHex(char inChar)
{
	int	value = inChar - '0';

	if (inChar >= 'A'  &&  inChar <= 'F') {
		value = inChar - 'A' + 10;

	} else if (inChar >= 'a'  &&  inChar <= 'f') {
		value = inChar - 'a' + 10;
	}

	return (char) value;
}

PP_End_Namespace_PowerPlant
