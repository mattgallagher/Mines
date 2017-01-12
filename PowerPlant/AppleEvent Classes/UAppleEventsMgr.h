// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UAppleEventsMgr.h			PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Utility functions for dealing with AppleEvents

#ifndef _H_UAppleEventsMgr
#define _H_UAppleEventsMgr
#pragma once

#include <PP_Prefix.h>
#include <AppleEvents.h>
#include <Events.h>
#include <Notification.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class UAppleEventsMgr {
public:
	static void		Initialize();

	static void		InstallAEHandlers(
							AEEventHandlerUPP	inHandler);

	static void		CheckForMissedParams(
							const AppleEvent&	inAppleEvent);

	static void		MakeAppleEvent(
							AEEventClass		inEventClass,
							AEEventID			inEventID,
							AppleEvent&			outAppleEvent);

	static void		SendAppleEvent(
							AppleEvent&			inAppleEvent,
							Boolean				inExecute = true);

	static void		SendAppleEventWithReply(
							const AppleEvent&	inAppleEvent,
							AppleEvent&			outAEReply,
							Boolean				inRecord = true);

	static bool		CompareDescriptors(
							const AEDesc&		inLeftDesc,
							DescType			inComparisonOperator,
							const AEDesc&		inRightDesc);

	static bool		CompareAsNumbers(
							const AEDesc&		inLeftDesc,
							DescType			inComparisonOperator,
							const AEDesc&		inRightDesc);

	static bool		CompareAsFloats(
							const AEDesc&		inLeftDesc,
							DescType			inComparisonOperator,
							const AEDesc&		inRightDesc);

	static bool		CompareAsDates(
							const AEDesc&		inLeftDesc,
							DescType			inComparisonOperator,
							const AEDesc&		inRightDesc);

	static bool		CompareAsIntegers(
							const AEDesc&		inLeftDesc,
							DescType			inComparisonOperator,
							const AEDesc&		inRightDesc);

	static bool		CompareAsStrings(
							const AEDesc&		inLeftDesc,
							DescType			inComparisonOperator,
							const AEDesc&		inRightDesc);

	static SInt32	GetAbsolutePositionIndex(
							const AEDesc&		inKeyData,
							SInt32				inItemCount);

	static OSErr	InteractWithUser(
							bool				inThrowFail = Throw_No,
							SInt32				inTimeoutTicks = kAEDefaultTimeout,
							NMRecPtr			inNotification = nil);

	static pascal Boolean
					AEIdleProc(
							EventRecord*		inMacEvent,
							SInt32*				outSleepTime,
							RgnHandle*			outMouseRgn);

	static AEDesc	sAnyType;

private:
	static AEAddressDesc	sSelfTargetAddr;
	static bool				sDontExecuteWorks;

	static void		MakeSelfTargetAddr(
							AEAddressDesc			&outSelfTargetAddr);
};

// ---------------------------------------------------------------------------

class	StAEDescriptor {
public:
	AEDesc	mDesc;

			operator	AEDesc*()				{ return &mDesc; }
			operator	const AEDesc*() const	{ return &mDesc; }

			operator	AEDesc&() 				{ return mDesc; }
			operator	const AEDesc&() const	{ return mDesc; }

			StAEDescriptor();

			StAEDescriptor(const StAEDescriptor &inOriginal);

			StAEDescriptor(
					const AEDesc&	inDesc,
					AEKeyword		inKeyword,
					DescType		inDesiredType = typeWildCard);

			StAEDescriptor(DescType inType, const void *inData, SInt32 inSize);

			StAEDescriptor(Boolean inBoolean);

			StAEDescriptor(SInt16 inInt16);

			StAEDescriptor(SInt32 inInt32);

			StAEDescriptor(OSType inOSType);

			StAEDescriptor(ConstStringPtr inString);

			StAEDescriptor(const FSSpec& inFileSpec);

			~StAEDescriptor();


	StAEDescriptor&		operator = (const StAEDescriptor &inOriginal)
							{
								return Assign(inOriginal);
							}

	StAEDescriptor&		operator = (Boolean inBoolean)
							{
								return Assign(inBoolean);
							}

	StAEDescriptor&		operator = (SInt16 inInt16)
							{
								return Assign(inInt16);
							}

	StAEDescriptor&		operator = (SInt32 inInt32)
							{
								return Assign(inInt32);
							}

	StAEDescriptor&		operator = (OSType inOSType)
							{
								return Assign(inOSType);
							}

	StAEDescriptor&		operator = (ConstStringPtr inString)
							{
								return Assign(inString);
							}

	StAEDescriptor&		operator = (const FSSpec& inFileSpec)
							{
								return Assign(inFileSpec);
							}

	StAEDescriptor&		Assign(const StAEDescriptor& inOriginal);

	StAEDescriptor&		Assign(Boolean inBoolean);

	StAEDescriptor&		Assign(SInt16 inInt16);

	StAEDescriptor&		Assign(SInt32 inInt32);

	StAEDescriptor&		Assign(OSType inOSType);

	StAEDescriptor&		Assign(ConstStringPtr inString);

	StAEDescriptor&		Assign(const FSSpec& inFileSpec);

	void	Dispose();

	bool	IsNull() const			{ return (mDesc.descriptorType == typeNull); }

	bool	IsNotNull() const		{ return (mDesc.descriptorType != typeNull); }

	DescType	DescriptorType() const	{ return mDesc.descriptorType; }

	void	GetParamDesc(
					const AppleEvent&	inAppleEvent,
					AEKeyword			inKeyword,
					DescType			inDesiredType);

	void	GetOptionalParamDesc(
					const AppleEvent&	inAppleEvent,
					AEKeyword			inKeyword,
					DescType			inDesiredType);

private:
	void	InitToNull()
				{
					mDesc.descriptorType = typeNull;
					mDesc.dataHandle     = nil;
				}
};

// ---------------------------------------------------------------------------

class UAEDesc {
public:
	static	void		AddPtr(
								AEDesc*			ioDesc,
								long			inIndex,
								DescType		inDescType,
								const void*		inDataPtr,
								Size			inDataSize);

	static	void		AddDesc(
								AEDesc*			ioDesc,
								long			inIndex,
								const AEDesc&	inDesc);

	static	void		AddKeyDesc(
								AERecord*		ioRecord,
								AEKeyword		inKeyword,
								const AEDesc&	inDesc);

	static	void		MakeRange(
								const AEDesc&	inSuperSpecifier,
								DescType		inKind,
								SInt32			inIndex1,
								SInt32			inIndex2,
								AEDesc*			outDesc);

	static	void		MakeInsertionLoc(
								const AEDesc&	inObjSpec,
								DescType		inPosition,
								AEDesc*			outDesc);

	static	void		MakeBooleanDesc(
								Boolean			inValue,
								AEDesc*			outDesc);
};


// === AppleEvent Numbers ===

										// Required Suite
const long	ae_OpenApp			= 1001;
const long	ae_OpenDoc			= 1002;
const long	ae_PrintDoc			= 1003;
const long	ae_Quit				= 1004;
const long	ae_ReopenApp		= 1005;
const long	ae_Preferences		= 1006;	// Mac OS X Preferences menu item

										// Core Suite
const long	ae_Clone			= 2001;
const long	ae_Close			= 2002;
const long	ae_CountElements	= 2003;
const long	ae_CreateElement	= 2004;
const long	ae_Delete			= 2005;
const long	ae_DoObjectsExist	= 2006;
const long	ae_GetClassInfo		= 2007;
const long	ae_GetData			= 2008;
const long	ae_GetDataSize		= 2009;
const long	ae_GetEventInfo		= 2010;
const long	ae_Move				= 2011;
const long	ae_Open				= 1002;		// Same as ae_OpenDoc
const long	ae_Print			= 1003;		// Same as ae_PrintDoc
const long	ae_Save				= 2012;
const long	ae_SetData			= 2013;

										// Miscellaneous Standards
const long	ae_ApplicationDied	= 3001;
const long	ae_BeginTransaction	= 3002;
const long	ae_Copy				= 3003;
const long	ae_CreatePublisher	= 3004;
const long	ae_Cut				= 3005;
const long	ae_DoScript			= 3006;
const long	ae_EditGraphic		= 3007;
const long	ae_EndTransaction	= 3008;
const long	ae_ImageGraphic		= 3009;
const long	ae_IsUniform		= 3010;
const long	ae_MakeObjVisible	= 3011;
const long	ae_Paste			= 3012;
const long	ae_Redo				= 3013;
const long	ae_Revert			= 3014;
const long	ae_TransactionTerm	= 3015;
const long	ae_Undo				= 3016;
const long	ae_Select			= 3017;
const long	ae_SwitchTellTarget = 3018;


enum {
	kAEPowerPlantSuite		= FOUR_CHAR_CODE('ppnt'),
	kAESwitchTellTarget		= FOUR_CHAR_CODE('sttg')
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
