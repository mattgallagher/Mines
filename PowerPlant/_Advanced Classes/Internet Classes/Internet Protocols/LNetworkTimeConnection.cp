// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LNetworkTimeConnection.cp	PowerPlant 2.2.2		 ©2000-2005 Metrowerks Inc.
// ===========================================================================
//
// Internet Network Time Protocol

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LNetworkTimeConnection.h>
#include <LInternetAddress.h>
#include <UNetworkFactory.h>

PP_Begin_Namespace_PowerPlant


const SInt32 kBaseInternetTime = -126144000;	//Jan 1, 1900 - 12:00:00 am in Mac time

// ===========================================================================

#pragma mark === LNetworkTimeConnection ===

// ---------------------------------------------------------------------------
//	¥ LNetworkTimeConnection				Constructor
// ---------------------------------------------------------------------------

LNetworkTimeConnection::LNetworkTimeConnection(LThread& inThread)
	: mThread(&inThread)
{
	mEndpoint		= nil;
	mRemoteAddress	= nil;
	mBufferSize		= kDefaultBufferSize;
	mTimeoutSeconds	= Timeout_None;
}


// ---------------------------------------------------------------------------
//	¥ ~LNetworkTimeConnection				Destructor
// ---------------------------------------------------------------------------

LNetworkTimeConnection::~LNetworkTimeConnection()
{
	ResetMembers();
}


// ---------------------------------------------------------------------------
//	¥ ResetMembers
// ---------------------------------------------------------------------------

void
LNetworkTimeConnection::ResetMembers()
{
	if (mEndpoint) {
		delete mEndpoint;
		mEndpoint = nil;
	}

	if (mRemoteAddress) {
		delete mRemoteAddress;
		mRemoteAddress = nil;
	}
}

// ---------------------------------------------------------------------------
//		¥ Connect
// ---------------------------------------------------------------------------

void
LNetworkTimeConnection::Connect(
	ConstStringPtr		inRemoteHost,
	UInt16				inRemotePort)
{
	ResetMembers();

	mEndpoint = UNetworkFactory::CreateUDPEndpoint();

	LInternetAddress localAddress(0, 0);
	mEndpoint->Bind(localAddress);

	mRemoteAddress = new LInternetAddress(inRemoteHost, inRemotePort, false);
}


// ---------------------------------------------------------------------------
//	¥ Disconnect
// ---------------------------------------------------------------------------

void
LNetworkTimeConnection::Disconnect()
{
	if (mEndpoint) {
		mEndpoint->AbortThreadOperation(mThread);
	}
}


// ---------------------------------------------------------------------------
//	¥ Abort
// ---------------------------------------------------------------------------

void
LNetworkTimeConnection::Abort()
{
	if (mEndpoint) {
		mEndpoint->AbortThreadOperation(mThread);
	}
}


// ---------------------------------------------------------------------------
//	¥ IsIdle
// ---------------------------------------------------------------------------

Boolean
LNetworkTimeConnection::IsIdle()
{
	if (mEndpoint == nil)
		return true;
	EEndpointState netState = mEndpoint->GetState();
	return ((netState == endpoint_Idle) || (netState == endpoint_Unbound));
}

UInt32
LNetworkTimeConnection::GetTimeFrom(ConstStr255Param	inRemoteHost,
								UInt16 inPort)
{
	Connect(inRemoteHost, inPort);

	UInt32 netTime;
	WaitResponse(netTime);

	Disconnect();

	//Convert 'Net seconds to Mac seconds
	netTime += (UInt32) kBaseInternetTime;

	return netTime;
}


// ---------------------------------------------------------------------------
//	¥ WaitResponse
// ---------------------------------------------------------------------------

void
LNetworkTimeConnection::WaitResponse(UInt32 & outNetTime)
{
	NTPMessageFormat theMessage;
	PP_CSTD::memset(&theMessage, sizeof(theMessage), '\0');

	theMessage.LeapVersionMode = 0x1B;	// 00011011
	theMessage.Stratum = 0;
	theMessage.PollInterval = 0;
	theMessage.Precision = 0;
	theMessage.RootDelay = 0;
	theMessage.RootDispersion = 0;
	PP_CSTD::memset(&theMessage.ReferenceIdentifier, sizeof(theMessage.ReferenceIdentifier), '\0');
	PP_CSTD::memset(&theMessage.ReferenceTimestamp, sizeof(theMessage.ReferenceTimestamp), '\0');
	PP_CSTD::memset(&theMessage.OriginateTimestamp, sizeof(theMessage.OriginateTimestamp), '\0');
	PP_CSTD::memset(&theMessage.ReceiveTimestamp, sizeof(theMessage.ReceiveTimestamp), '\0');
	PP_CSTD::memset(&theMessage.TransmitTimestamp, sizeof(theMessage.TransmitTimestamp), '\0');
	PP_CSTD::memset(&theMessage.Authenticator, sizeof(theMessage.Authenticator), '\0');

//	4. NTP Message Format RFC 2030
//
//	Both NTP and SNTP are clients of the User Datagram Protocol (UDP)
//	[POS80], which itself is a client of the Internet Protocol (IP)
//	[DAR81]. The structure of the IP and UDP headers is described in the
//	cited specification documents and will not be detailed further here.
//	The UDP port number assigned to NTP is 123, which should be used in
//	both the Source Port and Destination Port fields in the UDP header.
//	The remaining UDP header fields should be set as described in the
//	specification.
//
//	Below is a description of the NTP/SNTP Version 4 message format,
//	which follows the IP and UDP headers. This format is identical to
//	that described in RFC-1305, with the exception of the contents of the
//	reference identifier field. The header fields are defined as follows:
//
//	                       1                   2                   3
//	   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//	  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	  |LI | VN  |Mode |    Stratum    |     Poll      |   Precision   |
//	  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	  |                          Root Delay                           |
//	  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	  |                       Root Dispersion                         |
//	  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	  |                     Reference Identifier                      |
//	  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	  |                                                               |
//	  |                   Reference Timestamp (64)                    |
//	  |                                                               |
//	  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	  |                                                               |
//	  |                   Originate Timestamp (64)                    |
//	  |                                                               |
//	  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	  |                                                               |
//	  |                    Receive Timestamp (64)                     |
//	  |                                                               |
//	  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	  |                                                               |
//	  |                    Transmit Timestamp (64)                    |
//	  |                                                               |
//	  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	  |                 Key Identifier (optional) (32)                |
//	  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	  |                                                               |
//	  |                                                               |
//	  |                 Message Digest (optional) (128)               |
//	  |                                                               |
//	  |                                                               |
//	  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//	As described in the next section, in SNTP most of these fields are
//	initialized with pre-specified data. For completeness, the function
//	of each field is briefly summarized below.
//
//	Leap Indicator (LI): This is a two-bit code warning of an impending
//	leap second to be inserted/deleted in the last minute of the current
//	day, with bit 0 and bit 1, respectively, coded as follows:
//
//	  LI       Value     Meaning
//	  -------------------------------------------------------
//	  00       0         no warning
//	  01       1         last minute has 61 seconds
//	  10       2         last minute has 59 seconds)
//	  11       3         alarm condition (clock not synchronized)
//
//	Version Number (VN): This is a three-bit integer indicating the
//	NTP/SNTP version number. The version number is 3 for Version 3 (IPv4
//	only) and 4 for Version 4 (IPv4, IPv6 and OSI). If necessary to
//	distinguish between IPv4, IPv6 and OSI, the encapsulating context
//	must be inspected.
//
//	Mode: This is a three-bit integer indicating the mode, with values
//	defined as follows:
//
//	  Mode     Meaning
//	  ------------------------------------
//	  0        reserved
//	  1        symmetric active
//	  2        symmetric passive
//	  3        client
//	  4        server
//	  5        broadcast
//	  6        reserved for NTP control message
//	  7        reserved for private use
//
//	In unicast and anycast modes, the client sets this field to 3
//	(client) in the request and the server sets it to 4 (server) in the
//	reply. In multicast mode, the server sets this field to 5
//	(broadcast).
//
//	Stratum: This is a eight-bit unsigned integer indicating the stratum
//	level of the local clock, with values defined as follows:
//
//	  Stratum  Meaning
//	  ----------------------------------------------
//	  0        unspecified or unavailable
//	  1        primary reference (e.g., radio clock)
//	  2-15     secondary reference (via NTP or SNTP)
//	  16-255   reserved
//
//	Poll Interval: This is an eight-bit signed integer indicating the
//	maximum interval between successive messages, in seconds to the
//	nearest power of two. The values that can appear in this field
//	presently range from 4 (16 s) to 14 (16284 s); however, most
//	applications use only the sub-range 6 (64 s) to 10 (1024 s).
//
//	Precision: This is an eight-bit signed integer indicating the
//	precision of the local clock, in seconds to the nearest power of two.
//	The values that normally appear in this field range from -6 for
//	mains-frequency clocks to -20 for microsecond clocks found in some
//	workstations.
//
//	Root Delay: This is a 32-bit signed fixed-point number indicating the
//	total roundtrip delay to the primary reference source, in seconds
//	with fraction point between bits 15 and 16. Note that this variable
//	can take on both positive and negative values, depending on the
//	relative time and frequency offsets. The values that normally appear
//	in this field range from negative values of a few milliseconds to
//	positive values of several hundred milliseconds.
//
//	Root Dispersion: This is a 32-bit unsigned fixed-point number
//	indicating the nominal error relative to the primary reference
//	source, in seconds with fraction point between bits 15 and 16. The
//	values that normally appear in this field range from 0 to several
//	hundred milliseconds.
//
//	Reference Identifier: This is a 32-bit bitstring identifying the
//	particular reference source. In the case of NTP Version 3 or Version
//	4 stratum-0 (unspecified) or stratum-1 (primary) servers, this is a
//	four-character ASCII string, left justified and zero padded to 32
//	bits. In NTP Version 3 secondary servers, this is the 32-bit IPv4
//	address of the reference source. In NTP Version 4 secondary servers,
//	this is the low order 32 bits of the latest transmit timestamp of the
//	reference source. NTP primary (stratum 1) servers should set this
//	field to a code identifying the external reference source according
//	to the following list. If the external reference is one of those
//	listed, the associated code should be used. Codes for sources not
//	listed can be contrived as appropriate.
//
//	  Code     External Reference Source
//	  ----------------------------------------------------------------
//	  LOCL     uncalibrated local clock used as a primary reference for
//	           a subnet without external means of synchronization
//	  PPS      atomic clock or other pulse-per-second source
//	           individually calibrated to national standards
//	  ACTS     NIST dialup modem service
//	  USNO     USNO modem service
//	  PTB      PTB (Germany) modem service
//	  TDF      Allouis (France) Radio 164 kHz
//	  DCF      Mainflingen (Germany) Radio 77.5 kHz
//	  MSF      Rugby (UK) Radio 60 kHz
//	  WWV      Ft. Collins (US) Radio 2.5, 5, 10, 15, 20 MHz
//	  WWVB     Boulder (US) Radio 60 kHz
//	  WWVH     Kaui Hawaii (US) Radio 2.5, 5, 10, 15 MHz
//	  CHU      Ottawa (Canada) Radio 3330, 7335, 14670 kHz
//	  LORC     LORAN-C radionavigation system
//	  OMEG     OMEGA radionavigation system
//	  GPS      Global Positioning Service
//	  GOES     Geostationary Orbit Environment Satellite
//
//	Reference Timestamp: This is the time at which the local clock was
//	last set or corrected, in 64-bit timestamp format.
//
//	Originate Timestamp: This is the time at which the request departed
//	the client for the server, in 64-bit timestamp format.
//
//	Receive Timestamp: This is the time at which the request arrived at
//	the server, in 64-bit timestamp format.
//
//	Transmit Timestamp: This is the time at which the reply departed the
//	server for the client, in 64-bit timestamp format.
//
//	Authenticator (optional): When the NTP authentication scheme is
//	implemented, the Key Identifier and Message Digest fields contain the
//	message authentication code (MAC) information defined in Appendix C
//	of RFC-1305.

	mEndpoint->SendPacketData(*mRemoteAddress, &theMessage, sizeof(theMessage));

	NTPMessageFormat theMessageReply;
	PP_CSTD::memset(&theMessageReply, sizeof(theMessageReply), '\0');

	UInt32 ioDataSize = sizeof(theMessageReply);

	bool result = false;
	result = mEndpoint->ReceiveFrom(*mRemoteAddress, &theMessageReply, ioDataSize,10);

//	3. NTP Timestamp Format RFC 2030
//
//	SNTP uses the standard NTP timestamp format described in RFC-1305 and
//	previous versions of that document. In conformance with standard
//	Internet practice, NTP data are specified as integer or fixed-point
//	quantities, with bits numbered in big-endian fashion from 0 starting
//	at the left, or high-order, position. Unless specified otherwise, all
//	quantities are unsigned and may occupy the full field width with an
//	implied 0 preceding bit 0.
//
//	Since NTP timestamps are cherished data and, in fact, represent the
//	main product of the protocol, a special timestamp format has been
//	established. NTP timestamps are represented as a 64-bit unsigned
//	fixed-point number, in seconds relative to 0h on 1 January 1900. The
//	integer part is in the first 32 bits and the fraction part in the
//	last 32 bits. In the fraction part, the non-significant low order can
//	be set to 0.
//
//	  It is advisable to fill the non-significant low order bits of the
//	  timestamp with a random, unbiased bitstring, both to avoid
//	  systematic roundoff errors and as a means of loop detection and
//	  replay detection (see below). One way of doing this is to generate
//	  a random bitstring in a 64-bit word, then perform an arithmetic
//	  right shift a number of bits equal to the number of significant
//	  bits of the timestamp, then add the result to the original
//	  timestamp.
//
//	This format allows convenient multiple-precision arithmetic and
//	conversion to UDP/TIME representation (seconds), but does complicate
//	the conversion to ICMP Timestamp message representation, which is in
//	milliseconds. The maximum number that can be represented is
//	4,294,967,295 seconds with a precision of about 200 picoseconds,
//	which should be adequate for even the most exotic requirements.
//
//	                    1                   2                   3
//	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|                           Seconds                             |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|                  Seconds Fraction (0-padded)                  |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//	Note that, since some time in 1968 (second 2,147,483,648) the most
//	significant bit (bit 0 of the integer part) has been set and that the
//	64-bit field will overflow some time in 2036 (second 4,294,967,296).
//	Should NTP or SNTP be in use in 2036, some external means will be
//	necessary to qualify time relative to 1900 and time relative to 2036
//	(and other multiples of 136 years). There will exist a 200-picosecond
//	interval, henceforth ignored, every 136 years when the 64-bit field
//	will be 0, which by convention is interpreted as an invalid or
//	unavailable timestamp.
//
//	  As the NTP timestamp format has been in use for the last 17 years,
//	  it remains a possibility that it will be in use 40 years from now
//	  when the seconds field overflows. As it is probably inappropriate
//	  to archive NTP timestamps before bit 0 was set in 1968, a
//	  convenient way to extend the useful life of NTP timestamps is the
//	  following convention: If bit 0 is set, the UTC time is in the
//	  range 1968-2036 and UTC time is reckoned from 0h 0m 0s UTC on 1
//	  January 1900. If bit 0 is not set, the time is in the range 2036-
//	  2104 and UTC time is reckoned from 6h 28m 16s UTC on 7 February
//	  2036. Note that when calculating the correspondence, 2000 is not a
//	  leap year. Note also that leap seconds are not counted in the
//	  reckoning.

	NTPTimestampFormat* theTimestamp;
	theTimestamp = (NTPTimestampFormat *)&theMessageReply.TransmitTimestamp;

	UInt32 TransmitTime;
	TransmitTime = theTimestamp->Seconds;

	PP_CSTD::memcpy(&outNetTime, &TransmitTime, sizeof(TransmitTime));
}


PP_End_Namespace_PowerPlant
