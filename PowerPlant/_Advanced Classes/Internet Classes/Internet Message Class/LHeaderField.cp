// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LHeaderField.cp 			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
// Class for creating and parsing RFC822 Style Message Header Fields.
//
// ¥ Handles folding and unfolding fields


#include <LHeaderField.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <UInternet.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LHeaderField							Default Constructor
// ---------------------------------------------------------------------------

LHeaderField::LHeaderField()
{
	InitMembers();
}


// ---------------------------------------------------------------------------
//	¥ LHeaderField							Constructor
// ---------------------------------------------------------------------------

LHeaderField::LHeaderField(const char * inTitle, const char * inBody)
{
	InitMembers();
	SetTitle(inTitle);
	SetBody(inBody);
}


// ---------------------------------------------------------------------------
//	¥ LHeaderField							Constructor
// ---------------------------------------------------------------------------

LHeaderField::LHeaderField(const char * inField)
{
	InitMembers();
	SetField(inField);
}


// ---------------------------------------------------------------------------
//	¥ ~LHeaderField							Destructor
// ---------------------------------------------------------------------------

LHeaderField::~LHeaderField()
{
}


// ---------------------------------------------------------------------------
//	¥ InitMembers
// ---------------------------------------------------------------------------
//	Initializes member variables

void
LHeaderField::InitMembers()
{
	mTitle = "";
	mBody = "";
}

// ---------------------------------------------------------------------------
//	¥ GetTitle
// ---------------------------------------------------------------------------
//	Retuns the title portion of the field.

const char *
LHeaderField::GetTitle()
{
	return mTitle.c_str();
}

// ---------------------------------------------------------------------------
//		¥ GetBody
// ---------------------------------------------------------------------------
//	Retuns the body portion of the field.

const char *
LHeaderField::GetBody()
{
	return mBody.c_str();
}

// ---------------------------------------------------------------------------
//		¥ SetTitle
// ---------------------------------------------------------------------------
//	Sets the title portion of the field.

void
LHeaderField::SetTitle(const char * inTitle)
{
	mTitle = inTitle;
}

// ---------------------------------------------------------------------------
//		¥ SetTitle
// ---------------------------------------------------------------------------
//	Sets the title portion of the field.

void
LHeaderField::SetTitle(const char * inTitle, UInt32 inLength)
{
	mTitle.assign(inTitle, inLength);
}

// ---------------------------------------------------------------------------
//		¥ SetBody
// ---------------------------------------------------------------------------
//	Sets the body portion of the field.

void
LHeaderField::SetBody(const char * inBody)
{
	mBody = inBody;

	//store body as unwrapped field, we'll wrap it again on the way out
	UnfoldBody();
}

// ---------------------------------------------------------------------------
//		¥ SetBody
// ---------------------------------------------------------------------------
//	Sets the body portion of the field.

void
LHeaderField::SetBody(const char * inBody, UInt32 inLength)
{
	mBody.assign(inBody, inLength);

	//store body as unwrapped field, we'll wrap it again on the way out
	UnfoldBody();
}

// ---------------------------------------------------------------------------
//		¥ UnfoldBody
// ---------------------------------------------------------------------------
//	Unfolds the body portion of a field
//	This function operates directly on mBody to
//	remove all CRLF's and any white space surrounding them
// 	it *assumes* a single space as the new delimiter.

void
LHeaderField::UnfoldBody()
{
	UInt32 nextCRLF = mBody.find(CRLF);
	while(nextCRLF != mBody.npos) {
		UInt32 len;
		len = 0;

		while (PP_CSTD::isspace(mBody[nextCRLF + len]))
			len++;

		//replace old CRLF + WS with single SPACE
		mBody.replace(nextCRLF, len, " ");
		nextCRLF = mBody.find(CRLF);
	}
}

// ---------------------------------------------------------------------------
//		¥ Fold
// ---------------------------------------------------------------------------
//	Folds the body portion of a field

//The folded field is returned in the outField
void
LHeaderField::Fold(const char * inField, PP_STD::string& outField)
{
	//9/6/97 - Fix problems where inField may have trailing whitespace
	//	this will now be removed even if folding does not occur...
	SInt32 theLength = (SInt32) PP_CSTD::strlen(inField);
	StPointerBlock localCopy(theLength + 1);
	PP_CSTD::strcpy(localCopy, inField);
	while(PP_CSTD::isspace(localCopy[theLength - 1])) {
		theLength--;
	}
	localCopy[theLength] = '\0';

	outField = "";
	if (theLength > kFieldFoldStart) {
		const char * nextStart = localCopy;
		const char * nextBreak = &localCopy[kFieldFoldStart];
		while (nextBreak < (localCopy + theLength)) {
			const char * LWSPEnds;

			//find END of next LWSP
			for(; !PP_CSTD::isspace(*nextBreak); nextBreak--)
				;

			//Sanity Check... if we found no whitespace just copy the
			//	rest and bail out.
			if (nextBreak <= nextStart) {
				outField += PP_STD::string(nextStart, PP_CSTD::strlen(nextStart));
				return;
			}

			LWSPEnds = nextBreak;

			//find START of next LWSP
			for(; PP_CSTD::isspace(*nextBreak); nextBreak--)
				;

			nextBreak++;	//Now pointed at start of LWSP

			outField += PP_STD::string(nextStart, (UInt32) (nextBreak - nextStart));
			outField += kFoldIndent;

			nextStart = ++LWSPEnds;
			nextBreak = &nextStart[kFieldFoldStart];
		}
		outField += nextStart;
	}
	else
		outField = localCopy;
}

// ---------------------------------------------------------------------------
//		¥ GetField
// ---------------------------------------------------------------------------
//	Returns "Title: Body" with field folding as necessary.

void
LHeaderField::GetField(PP_STD::string& outField)
{
	if ((mTitle.length() == 0) || (mBody.length() == 0)) {
		return;
	}

	PP_STD::string tempField;

	tempField = mTitle;
	tempField += ": ";
	tempField += mBody;

	Fold(tempField.c_str(), outField);

	//Add CRLF after folding to ensure we get proper field seperation
	outField += CRLF;
}


// ---------------------------------------------------------------------------
//		¥ SetField
// ---------------------------------------------------------------------------
//	Takes "Title: Body", unfolds as necessary and sets the appropriate member
//	variables.

void
LHeaderField::SetField(const char * inField)
{
	char * tempP1, * tempP2;

	//Copy inField to temp string...
	//	not effecient on memory, but allows us to
	//	deal with parsing the data more easily.
	PP_STD::string tempBuffer(inField);
	const char * tempStr = tempBuffer.c_str();

	tempP1 = const_cast<char*>(PP_CSTD::strstr(tempStr, ": "));
	if (tempP1) {
		SetTitle(tempStr, (UInt32) (tempP1 - tempStr));

		tempP1 += 2; //walk past the ": "

		//Find *real* end of field... checks for folded fields
		PP_STD::string tempBuffer2;
		tempP2 = PP_CSTD::strtok(tempP1, CRLF);
		while (tempP2) {
			tempBuffer2 += tempP2;
			tempP2 = PP_CSTD::strtok(nil, CRLF);
			if (!tempP2)
				break;

			if (!PP_CSTD::isspace(*tempP2))
				break;

			while(PP_CSTD::isspace(*tempP2))
				tempP2++;

			tempBuffer2 += " ";
		}
		SetBody(tempBuffer2.c_str());
	}
}


// ---------------------------------------------------------------------------
//	¥ FieldLength
// ---------------------------------------------------------------------------
//	Returns estimated length of field
//ÊNOTE: This routine does not take into account folding

UInt32
LHeaderField::FieldLength()
{
	return mTitle.length() + mBody.length() + kFieldOverHead;
}


PP_End_Namespace_PowerPlant
