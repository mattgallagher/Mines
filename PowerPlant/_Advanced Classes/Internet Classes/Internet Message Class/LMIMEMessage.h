// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMIMEMessage.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LMIMEMessage
#define _H_LMIMEMessage
#pragma once

#include <LInternetMessageList.h>
#include <LInternetMessage.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LMIMEMessage : public LInternetMessage {
public:
								LMIMEMessage();
								LMIMEMessage(const char * inMessage, UInt32 inLength = 0);
								LMIMEMessage(LDynamicBuffer * inBuffer);
								LMIMEMessage(LStream * inStream, SInt32 inLength = -1);
	virtual 					~LMIMEMessage();

	virtual void				InitMembers();
	virtual void				ResetMembers();

	virtual void				SetMessageBody(const char * inBody, UInt32 inLength = 0);

	virtual void				SetMessageBody(LStream * inStream, SInt32 inLength = -1, Boolean ownStream = false);

	virtual void				SetPartialMessage(LDynamicBuffer * inBuffer);
	virtual Boolean				GetPartialMessage(LDynamicBuffer * outMessage, Boolean inStartGet);
	virtual void				AddMessageBodySegment(LMIMEMessage * inMessage);
	virtual LInternetMessageList *	GetMessageBodyList();

	virtual Boolean				GetIsMIME();
	virtual const char *		GetMIMEVersion();
	virtual void				SetIsMIME(Boolean inValue = true, const char * inVersion = kDefaultMIMEVers);

	virtual void				SetBoundary(const char * inSubject = nil);
	virtual const char *		GetBoundary();

	virtual void				SetContentType(const char * inContentType = nil);
	virtual const char *		GetContentType() ;

	virtual void				SetMIMEPreamble(const char * inPreamble, SInt32 inLength = 0);
	virtual const char *		GetMIMEPreamble();

	virtual UInt32				GetMessageBodySize();

	void						Clone(const LMIMEMessage& inOriginal);

protected:
	virtual void				GetDefaultsFromHeader();
	virtual void				BuildHeader(LDynamicBuffer * outHeader);
	virtual Boolean				BuildMessageBody(LDynamicBuffer * outMessage);
	virtual void				AdjustSegmentDepth(UInt32 inDepth = 1);
	virtual Boolean				FindBoundaryInStream(LStream * inStream);
	virtual Boolean				IsTerminationBoundary(LStream * inStream);

	//MIME Support
	LInternetMessageList		mBodyList;
	Boolean						mIsMIME;
	PP_STD::string				mMIMEVers;
	PP_STD::string				mMIMEPreamble;
	PP_STD::string				mContentType;
	PP_STD::string				mBoundary;
	UInt32						mSegmentDepth;

	//Partial Get/Put globals
	UInt32						mCurrentWorkingSegment;
	Boolean						mFinishedSegment;
	Boolean						mSentSegmentSeperator;
	Boolean						mHaveBoundaryStart;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
