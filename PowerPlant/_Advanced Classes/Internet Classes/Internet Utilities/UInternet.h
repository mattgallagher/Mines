// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UInternet.h					PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UInternet
#define _H_UInternet
#pragma once

#include <md5.h>
#include <uucode.h>
#include <LHeaderField.h>
#include <LDynamicBuffer.h>
#include <LString.h>
#include <cctype>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LArray;

const char		CR						= '\r';
const char		LF						= '\n';
const char		CRLF[]					= "\r\n";
const char		kMailTermSequence[]		= "\r\n.\r\n";
const SInt16	kMailTermSequenceLength	= 5;
const char		kHeaderSep[]			= "\r\n\r\n";
const char		kAltHeaderSep[]			= "\n\n";
const SInt16	kSMTPMaxLineLen			= 1000;

// ---------------------------------------------------------------------------

class UInternet {
public:
	static void			MD5String(const char *inString, char * outDigest);
	static char *		UUEncode(const void * inData, UInt32 * InOutDataSize);
	static void			UUEncode(const void * inData, UInt32 * InOutDataSize, LDynamicBuffer* outBuffer);
	static void *		UUDecode(char * inData, UInt32 * InOutDataSize);
	static void			UUDecode(char * inData, UInt32 * InOutDataSize, LDynamicBuffer * outBuffer);
	static Boolean		CheckSMTPValid (LDynamicBuffer* theBuffer);
	static Boolean		SMTPEncode(LDynamicBuffer* theBuffer);
	static void			SMTPDecode(LDynamicBuffer* theBuffer);
	static void			RemoveLF(LDynamicBuffer* theBuffer);
	static SInt32		GetContentLength(LDynamicBuffer* dataBuffer);
	static char *		FindBodyStarts(const char * inMessage, UInt32 inLength = 0);
	static SInt32		FindBodyStarts(LDynamicBuffer* theBuffer);
	static SInt32		FindBodyStarts(LStream* inStream, SInt32 inLength = -1);
	static Boolean		FindField(const char * inFieldType, const char * inHeader, LHeaderField * outField);
	static Boolean		FindField(const char * inFieldType, const char * inHeader, SInt32& outPos, SInt32& outLen);
	static void			GetCleanAddress(LStr255& InOutAddress);
	static char *		PointToNextLine(const char * ioPos);
	static void			PointToNextLine(LStream * inStream);
	static void			CommaStringToList(const char * inString, LArray * outList);
	static void			ListToCommaString(LArray * inList, PP_STD::string& outString);
	static SInt32		GetGMTOffset();
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
