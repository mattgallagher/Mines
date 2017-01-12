// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LModelProperty.cp			PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Class for a Property defined by the Apple Event Object Model. A Property,
//	identified by a 4-character ID, is associated with an LModelObject. You
//	change the value of a Property by sending SetData and GetData Apple
//	Events.
//
//	This class handles SetData, GetData, and GetDataSize Apple Events by
//	calling the SetAEProperty and GetAEProperty functions of the LModelObject
//	which owns the LModelProperty.
//
//	Normally, you will not need to create subclasses of LModelProperty. You
//	support Properties by overriding SetAEProperty and GetAEProperty for
//	a subclass of LModelObject.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LModelProperty.h>
#include <LModelDirector.h>
#include <UAppleEventsMgr.h>

#include <AERegistry.h>
#include <AEObjects.h>
#include <AEPackObject.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LModelProperty						Constructor				  [public]
// ---------------------------------------------------------------------------

LModelProperty::LModelProperty(
	DescType		inPropertyID,
	LModelObject*	inSuperModel,
	Boolean			inBeLazy)

	: LModelObject(inSuperModel, cProperty)
{
	mPropertyID = inPropertyID;
	SetLaziness(inBeLazy);
}


// ---------------------------------------------------------------------------
//	¥ ~LModelProperty						Destructor				  [public]
// ---------------------------------------------------------------------------

LModelProperty::~LModelProperty()
{
}


// ---------------------------------------------------------------------------
//	¥ HandleAppleEvent												  [public]
// ---------------------------------------------------------------------------
//	Respond to an AppleEvent
//
//	ModelProperties can respond to "GetData", "GetDataSize", and "SetData"
//	AppleEvents

void
LModelProperty::HandleAppleEvent(
	const AppleEvent&	inAppleEvent,
	AppleEvent&			outAEReply,
	AEDesc&				outResult,
	SInt32				inAENumber)
{
	switch (inAENumber) {

		case ae_GetData:
		case ae_GetDataSize:
			HandleGetData(inAppleEvent, outResult, inAENumber);
			break;

		case ae_SetData:
			HandleSetData(inAppleEvent, outAEReply);
			break;

		default:
			LModelObject::HandleAppleEvent(inAppleEvent, outAEReply,
									outResult, inAENumber);
			break;
	}

}


// ---------------------------------------------------------------------------
//	¥ SendSetDataAE													  [public]
// ---------------------------------------------------------------------------
//	Send this program a "SetData" AppleEvent for a ModelProperty, with
//	the Property value specified by a data type, data ptr, and data length

void
LModelProperty::SendSetDataAE(
	DescType	inDataType,
	Ptr			inDataPtr,
	Size		inDataSize,
	Boolean		inExecute)
{
	OSErr		err;

	AppleEvent	theAppleEvent	= {typeNull, nil};

	UAppleEventsMgr::MakeAppleEvent(kAECoreSuite, kAESetData, theAppleEvent);

	StAEDescriptor	propertySpec;
	MakeSpecifier(propertySpec.mDesc);
	err = ::AEPutParamDesc(&theAppleEvent, keyDirectObject, propertySpec);
	ThrowIfOSErr_(err);

	err = ::AEPutParamPtr(&theAppleEvent, keyAEData, inDataType, inDataPtr,
									inDataSize);
	ThrowIfOSErr_(err);

	UAppleEventsMgr::SendAppleEvent(theAppleEvent, inExecute);
}


// ---------------------------------------------------------------------------
//	¥ SendSetDataAEDesc												  [public]
// ---------------------------------------------------------------------------
//	Send this program a "SetData" AppleEvent for a ModelProperty, with the
//	Property value specified by an AE descriptor record

void
LModelProperty::SendSetDataAEDesc(
	const AEDesc&	inDesc,
	Boolean			inExecute)
{
	OSErr		err;

	AppleEvent	theAppleEvent	= {typeNull, nil};

	UAppleEventsMgr::MakeAppleEvent(kAECoreSuite, kAESetData, theAppleEvent);

	StAEDescriptor	propertySpec;
	MakeSpecifier(propertySpec.mDesc);

	err = ::AEPutParamDesc(&theAppleEvent, keyDirectObject, propertySpec);
	ThrowIfOSErr_(err);

	err = ::AEPutParamDesc(&theAppleEvent, keyAEData, &inDesc);
	ThrowIfOSErr_(err);

	UAppleEventsMgr::SendAppleEvent(theAppleEvent, inExecute);
}


// ---------------------------------------------------------------------------
//	¥ CompareToDescriptor											  [public]
// ---------------------------------------------------------------------------
//	Return the result of comparing a ModelProperty to the value in an
//	AppleEvent descriptor

Boolean
LModelProperty::CompareToDescriptor(
	DescType		inComparisonOperator,
	const AEDesc&	inCompareDesc) const
{
	StAEDescriptor	propDesc;
	AEDesc			requestedType = {typeNull, nil};
	mSuperModel->GetAEProperty(mPropertyID, requestedType, propDesc.mDesc);
	Boolean result = UAppleEventsMgr::CompareDescriptors(propDesc,
								inComparisonOperator, inCompareDesc);
	return result;
}


// ---------------------------------------------------------------------------
//	¥ MakeSelfSpecifier											   [protected]
// ---------------------------------------------------------------------------
//	Pass back an AppleEvent object specifier for a ModelProperty

void
LModelProperty::MakeSelfSpecifier(
	AEDesc&		inSuperSpecifier,
	AEDesc&		outSelfSpecifier) const
{
		// Make descriptor for the property

	StAEDescriptor	keyData;
	OSErr	err = ::AECreateDesc(typeType, (Ptr) &mPropertyID,
									sizeof(mPropertyID), keyData);
	ThrowIfOSErr_(err);

		// Make ospec for the property

	err = ::CreateObjSpecifier(cProperty, &inSuperSpecifier, formPropertyID,
									keyData, false, &outSelfSpecifier);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ HandleGetData												   [protected]
// ---------------------------------------------------------------------------
//	Respond to a "GetData" or "GetDataSize" AppleEvent for a ModelProperty
//	by loading the value or size of the property's data into a
//	descriptor record

void
LModelProperty::HandleGetData(
	const AppleEvent&	inAppleEvent,
	AEDesc&				outResult,
	long				inAENumber)
{
									// Find requested type for the data
									// This parameter is optional, so it's OK
									//   if it's not found
	StAEDescriptor	requestedType;
	requestedType.GetOptionalParamDesc(inAppleEvent, keyAERequestedType,
									typeAEList);

									// Error if there are more parameters
	UAppleEventsMgr::CheckForMissedParams(inAppleEvent);

									// Ask SuperModel for the property value
	GetSuperModel()->GetAEProperty(mPropertyID, requestedType, outResult);

	if (inAENumber == ae_GetDataSize) {
									// For GetDataSize, size of Property
									//   is the result

		#if ACCESSOR_CALLS_ARE_FUNCTIONS
			SInt32	theSize = ::AEGetDescDataSize(&outResult);
		#else
			SInt32	theSize = ::GetHandleSize(outResult.dataHandle);
		#endif

		::AEDisposeDesc(&outResult);
		outResult.dataHandle = nil;
		OSErr err = ::AECreateDesc(typeLongInteger, &theSize, sizeof(SInt32),
								&outResult);
		ThrowIfOSErr_(err);
	}
}


// ---------------------------------------------------------------------------
//	¥ HandleSetData												   [protected]
// ---------------------------------------------------------------------------
//	Respond to a "SetData" AppleEvent for a ModelProperty

void
LModelProperty::HandleSetData(
	const AppleEvent&	inAppleEvent,
	AppleEvent&			outAEReply)
{
	StAEDescriptor	value;			// Extract value to which to set
	value.GetParamDesc(inAppleEvent, keyAEData, typeWildCard);

	if (value.DescriptorType() == typeObjectSpecifier) {

			// "value" is an object specifier. Resolve it to a LModelObject

		StAEDescriptor	tokenD;
		OSErr	err = LModelDirector::Resolve(value, tokenD);
		
		if (err == noErr) {			// Resolve succeeded. Let's see what
									//   kind of object we have.

			LModelObject*	refObject = GetModelFromToken(tokenD);

			if (refObject->GetModelKind() == cProperty) {

					// ospec resolved to a Property object. Change "value"
					// to the data for that Property object.

				LModelProperty*	refProperty = dynamic_cast<LModelProperty*>(refObject);

				StAEDescriptor	getDataEvent;
				UAppleEventsMgr::MakeAppleEvent(kAECoreSuite, kAEGetData, getDataEvent);

				value.Dispose();			// We are going to change the value
				refProperty->HandleGetData(getDataEvent, value, ae_GetData);

			} else {

					// ospec isn't a Property. Change "value" to the token for
					// this object so that SetAEProperty() doesn't have to
					// resolve the ospec again. You can extract the underlying
					// LModelObject* by calling GetModelFromToken().

				value = tokenD;
			}
		}
	}

	UAppleEventsMgr::CheckForMissedParams(inAppleEvent);

	GetSuperModel()->SetAEProperty(mPropertyID, value, outAEReply);
}


PP_End_Namespace_PowerPlant
