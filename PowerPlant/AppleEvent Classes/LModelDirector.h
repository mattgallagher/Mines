// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LModelDirector.h			PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A wrapper class for AppleEvent handlers and the AE Object Support Library

#ifndef _H_LModelDirector
#define _H_LModelDirector
#pragma once

#include <PP_Prefix.h>
#include <AppleEvents.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

// ---------------------------------------------------------------------------
//	Type for refcon of an AppleEvent Handler callback function
//
//		Universal Interfaces have changed this type in the past, so we
//		declare our own type for ease of maintenance in case the type
//		changes again.

typedef	long	AEHandlerRefConT;


PP_Begin_Namespace_PowerPlant

class LModelObject;

// ---------------------------------------------------------------------------

class	LModelDirector {
public:
						LModelDirector();

						LModelDirector( LModelObject* inDefaultModel );

	virtual				~LModelDirector();

	void				SetResolveFlags( SInt16 inResolveFlags )
							{
								mResolveFlags = inResolveFlags;
							}

	virtual void		InstallCallBacks();

	virtual void		HandleAppleEvent(
								const AppleEvent&	inAppleEvent,
								AppleEvent&			outReply,
								AEHandlerRefConT	inRefCon);

	virtual void		HandleCountElementsEvent(
								const AppleEvent&	inAppleEvent,
								AppleEvent&			outReply,
								AEHandlerRefConT	inRefCon);

	virtual void		HandleCreateElementEvent(
								const AppleEvent&	inAppleEvent,
								AppleEvent&			outReply,
								AEHandlerRefConT	inRefCon);

	virtual void		HandleDoObjectsExistEvent(
								const AppleEvent&	inAppleEvent,
								AppleEvent&			outReply,
								AEHandlerRefConT	inRefCon);

	virtual void		ProcessTokenizedEvent(
								const AppleEvent&	inAppleEvent,
								AEDesc&				inDirectObjToken,
								AEDesc	&			outResult,
								AppleEvent&			outReply,
								AEHandlerRefConT	inRefCon);

	virtual void		AccessModelObject(
								DescType			inDesiredClass,
								const AEDesc&		inContainerToken,
								DescType			inContainerClass,
								DescType			inKeyForm,
								const AEDesc&		inKeyData,
								AEDesc&				outToken,
								SInt32				inRefCon);

	virtual void		AccessModelObjectList(
								DescType			inDesiredClass,
								const AEDesc&		inContainerToken,
								DescType			inContainerClass,
								DescType			inKeyForm,
								const AEDesc&		inKeyData,
								AEDesc&				outToken,
								SInt32				inRefCon);

	virtual void		DisposeToken(
								AEDesc&				ioToken);

	virtual void		CompareObjects(
								DescType			inComparisonOperator,
								const AEDesc&		inBaseObject,
								const AEDesc&		inCompareObjectOrDesc,
								Boolean&			outResult);

	virtual void		CountObjects(
								DescType			inDesiredClass,
								DescType			inContainerClass,
								const AEDesc&		inContainer,
								SInt32&				outCount);

	static	OSErr		Resolve(
								const AEDesc&		inSpec,
								AEDesc&				outToken);

	static LModelDirector*	GetModelDirector()		{ return sModelDirector; }

	static AppleEvent*		GetCurrentAEReply()		{ return sCurrentAEReply; }

protected:
	static LModelDirector*	sModelDirector;
	static AppleEvent*		sCurrentAEReply;

	SInt16					mResolveFlags;

	static pascal OSErr	AppleEventHandler(
								const AppleEvent*	inAppleEvent,
								AppleEvent*			outAEReply,
								AEHandlerRefConT	inRefCon);

	static pascal OSErr	CountElementsEventHandler(
								const AppleEvent*	inAppleEvent,
								AppleEvent*			outAEReply,
								AEHandlerRefConT	inRefCon);

	static pascal OSErr	CreateElementEventHandler(
								const AppleEvent*	inAppleEvent,
								AppleEvent*			outAEReply,
								AEHandlerRefConT	inRefCon);

	static pascal OSErr	DoObjectsExistEventHandler(
								const AppleEvent*	inAppleEvent,
								AppleEvent*			outAEReply,
								AEHandlerRefConT	inRefCon);

	static pascal OSErr	ModelObjectAccessor(
								DescType			inDesiredClass,
								const AEDesc*		inContainerToken,
								DescType			inContainerClass,
								DescType			inKeyForm,
								const AEDesc*		inKeyData,
								AEDesc*				outToken,
								SInt32				inRefCon);

	static pascal OSErr	ModelObjectListAccessor(
								DescType			inDesiredClass,
								const AEDesc*		inContainerToken,
								DescType			inContainerClass,
								DescType			inKeyForm,
								const AEDesc*		inKeyData,
								AEDesc*				outToken,
								SInt32				inRefCon);

	static pascal OSErr	OSLDisposeToken(
								AEDesc*				inToken);

	static pascal OSErr	OSLCompareObjects(
								DescType			inComparisonOperator,
								const AEDesc*		inBaseObject,
								const AEDesc*		inCompareObjectOrDesc,
								Boolean*			outResult);

	static pascal OSErr	OSLCountObjects(
								DescType			inDesiredClass,
								DescType			inContainerClass,
								const AEDesc*		inContainer,
								SInt32*				outCount);

	static void			StoreReplyErrorString(
								ConstStringPtr		inErrorString,
								AppleEvent*			ioReply);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
