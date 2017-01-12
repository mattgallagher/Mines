// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LNetworkTimeConnection.h	PowerPlant 2.2.2		 ©2000-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LNetworkTimeConnection
#define _H_LNetworkTimeConnection
#pragma once

#include <LUDPEndpoint.h>
#include <LThread.h>
#include <LDynamicBuffer.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

const UInt16	kDefaultNetworkTimePort = 123;

// ---------------------------------------------------------------------------

class LNetworkTimeConnection {
public:
	static const UInt32	kDefaultBufferSize = 1024;

	struct NTPMessageFormat {
		UInt8							LeapVersionMode;
		UInt8							Stratum;
		SInt8							PollInterval;
		SInt8							Precision;
		SInt32							RootDelay;
		UInt32							RootDispersion;
		UInt8							ReferenceIdentifier[4];
		UInt8							ReferenceTimestamp[8];
		UInt8							OriginateTimestamp[8];
		UInt8							ReceiveTimestamp[8];
		UInt8							TransmitTimestamp[8];
		UInt8							Authenticator[16];
	};
	typedef struct NTPMessageFormat		NTPMessageFormat;
	typedef NTPMessageFormat *			NTPMessageFormatPtr;

	struct NTPTimestampFormat {
		UInt32							Seconds;
		UInt32							SecondsFraction;
	};
	typedef struct NTPTimestampFormat	NTPTimestampFormat;
	typedef NTPTimestampFormat *		NTPTimestampFormatPtr;

							LNetworkTimeConnection(LThread& inThread);
	virtual					~LNetworkTimeConnection();

	virtual void			ResetMembers();

	// connection configuration

	void					Connect(
									ConstStringPtr		inRemoteHost,
									UInt16				inRemotePort);
	virtual void			Disconnect();
	virtual Boolean			IsIdle();

	virtual void			Abort();

	inline void				SetThread(LThread& inThread) {mThread = &inThread;}
	virtual void			SetBufferSize(UInt32 inSize)
									{ mBufferSize = inSize; }
	inline UInt32			GetBufferSize()
									{ return mBufferSize; }

	virtual void			SetTimeoutSeconds(UInt32	inSeconds)
							{ mTimeoutSeconds = inSeconds; }

	virtual UInt32			GetTimeoutSeconds()
							{ return mTimeoutSeconds; }

	virtual LUDPEndpoint*	GetEndpoint()
							{ return mEndpoint; }

	UInt32					GetTimeFrom(ConstStr255Param inRemoteHost,
									UInt16 inPort = kDefaultNetworkTimePort);

protected:
	virtual void			WaitResponse(UInt32 & outNetTime);

	LUDPEndpoint*			mEndpoint;					// our network endpoint object

	LThread*				mThread;

	LInternetAddress*		mRemoteAddress;

	UInt32					mBufferSize;				//size of send/rcv buffer
	UInt32					mTimeoutSeconds;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
