// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMailMessage.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LMailMessage
#define _H_LMailMessage
#pragma once

#include <LMIMEMessage.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LMailMessage : public LMIMEMessage {
public:
						LMailMessage();
						LMailMessage(const char * inMessage, UInt32 inLength = 0);
						LMailMessage(LDynamicBuffer * inBuffer);
						LMailMessage(LStream * inStream, SInt32 inLength = -1);
	virtual 			~LMailMessage();

	virtual void		InitMembers();
	virtual void		ResetMembers();
	virtual void		InitMessage();

	Boolean				ReadyToSend();

	LArray*				GetTo();
	LArray*				GetCC();
	LArray*				GetBCC();

	const char *		GetCustomTo();
	const char *		GetCustomCC();

	void				AddTo(const char * inItem);
	void				AddCC(const char * inItem);
	void				AddBCC(const char * inItem);

	void				SetTo(LArray * inList);
	void				SetCC(LArray * inList);
	void				SetBCC(LArray * inList);

	void				SetTo(const char * inList, Boolean inCustom = false);
	void				SetCC(const char * inList, Boolean inCustom = false);
	void				SetBCC(const char * inList);

	void				SetDateTime();
	void				SetDateTime(DateTimeRec * inDateTime);
	void				SetDateTime(const char * inDateTime);
	DateTimeRec *		GetDateTime();

	void				SetGMTOffset();
	void				SetGMTOffset(SInt32 inOffset);
	void				SetGMTOffset(const char * inOffset);
	SInt32				GetGMTOffset();

	void				SetFrom(const char * inFrom);
	const char *		GetFrom();

	void				SetSubject(const char * inSubject);
	const char *		GetSubject();

	void				SetMessageID();
	void				SetMessageID(const char * inMessageID);
	const char *		GetMessageID();

	void				SetStatus(const char * theStatus);
	Boolean				GetStatus();

	void				Clone(const LMailMessage& inOriginal);

protected:
	virtual void		GetDefaultsFromHeader();
	virtual void		BuildHeader(LDynamicBuffer * outHeader);
	virtual Boolean		CheckBody();
	void				BuildDate(char * outDateString);

	LArray *			mToList;
	LArray *			mCCList;
	LArray *			mBCCList;
	DateTimeRec 		mDateTime;
	Boolean				mDateValid;
	SInt32				mGMTOffset;
	PP_STD::string		mFrom;
	PP_STD::string		mSubject;
	PP_STD::string		mMessageID;
	PP_STD::string		mToField;
	PP_STD::string		mCCField;
	Boolean				mRead;
	Boolean				mCustomTo;
	Boolean				mCustomCC;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
