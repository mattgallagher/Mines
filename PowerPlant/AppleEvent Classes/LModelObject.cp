// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LModelObject.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Mix-in class for supporting the Apple Event Object Model. Subclasses of
//	LModelObject represent Apple Event Object defined by the Apple Event
//	Registry.
//
//	=== Functions to Override ===
//
//	To Respond to Events:
//		HandleAppleEvent
//
//	To Support Elements:
//		CountSubModels
//		GetSubModelByPosition
//		GetSubModelByName
//		GetPositionOfSubModel
//		HandleCreateElementEvent
//
//	To Support Properties:
//		GetAEProperty
//		SetAEProperty
//		AEPropertyExists

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LModelObject.h>
#include <LModelDirector.h>
#include <LModelProperty.h>
#include <TArrayIterator.h>
#include <UAppleEventsMgr.h>
#include <UExtractFromAEDesc.h>
#include <UMemoryMgr.h>

#include <AEObjects.h>
#include <AERegistry.h>
#include <AEPackObject.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Class Variables

LModelObject*			LModelObject::sDefaultModel = nil;
LModelObject*			LModelObject::sStreamingModel = nil;
TArray<LModelObject*>*	LModelObject::sLazyModels = nil;


// ---------------------------------------------------------------------------
//	¥ LModelObject							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LModelObject::LModelObject()
{
	mSuperModel			= nil;
	mModelKind			= typeNull;
	mLaziness			= false;
	mSubModels			= nil;
	mDefaultSubModel	= nil;
	mDefaultSuperModel	= nil;

	SetStreamingModel(this);
}


// ---------------------------------------------------------------------------
//	¥ LModelObject							Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct a ModelObject with a specified SuperModel
//
//	Use the optional inKind parameter to indicate the AppleEvent class for
//	this object.

LModelObject::LModelObject(
	LModelObject*	inSuperModel,
	DescType		inKind)
{
	mSuperModel			= inSuperModel;
	mModelKind			= inKind;
	mLaziness			= false;
	mSubModels			= nil;
	mDefaultSubModel	= nil;
	mDefaultSuperModel	= nil;

	if (mSuperModel != nil) {
		mSuperModel->AddSubModel(this);
	}

	SetStreamingModel(this);
}


// ---------------------------------------------------------------------------
//	¥ ~LModelObject							Destructor				  [public]
// ---------------------------------------------------------------------------

LModelObject::~LModelObject()
{
	if (IsLazy()) {
		sLazyModels->Remove(this);
	}

	if (GetStreamingModel() == this) {
		SetStreamingModel(nil);
	}

	//	Make sure pointers to this object aren't left dangling.
	SetDefaultSubModel(nil);
	if (GetDefaultSuperModel() != nil) {
		GetDefaultSuperModel()->SetDefaultSubModel(nil);
	}

	//	Remove/delete submodels
	if (mSubModels != nil) {
		TArrayIterator<LModelObject*>	iterator(*mSubModels, LArrayIterator::from_End);
		LModelObject*	sub;

		while (iterator.Previous(sub)) {
			sub->SetSuperModel(nil);
			sub->SuperDeleted();
		}

		delete mSubModels;
	}

	if (mSuperModel != nil) {
		mSuperModel->RemoveSubModel(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ Finalize
// ---------------------------------------------------------------------------
//	Called when a model object should consider deleting itself

void
LModelObject::Finalize()
{
		// Delete lazy ModelObjects that have no SubModels

	if ( IsLazy() &&
		 ((mSubModels == nil) || (mSubModels->GetCount() == 0)) ) {

		delete this;
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SuperDeleted
// ---------------------------------------------------------------------------
//	The super model of this object was just deleted and this object should
//	consider deleting itself.

void
LModelObject::SuperDeleted()
{
	if (not IsLazy()) {
		delete this;
	}
}


// ---------------------------------------------------------------------------
//	¥ SetSuperModel
// ---------------------------------------------------------------------------
//	Switch the SuperModel of a ModelObject.
//
//	You will rarely need to call this method -- constructors and the
//	destructor typically take care of such bookkeeping details.

void
LModelObject::SetSuperModel(
	LModelObject*	inSuperModel)
{
	if (inSuperModel != mSuperModel) {

		if (mSuperModel != nil) {			// Detach from old SuperModel
			mSuperModel->RemoveSubModel(this);
		}

		mSuperModel = inSuperModel;

		if (mSuperModel != nil) {			// Attach to new SuperModel
			mSuperModel->AddSubModel(this);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SetModelKind
// ---------------------------------------------------------------------------
//	Set the AppleEvent Class ID of this ModelObject
//
//	You may specify the ModelKind in a constructor or with this method
//	shortly after object construction.
//
//	If the ModelKind is not specified during or after construction, it
//	defaults to "typeNull."
//
//	Being able to specify the model kind means a given subclass of
//	LModelObject can be used to implement more than one type of Apple Event
//	Object Model "class."

void
LModelObject::SetModelKind(
	DescType	inModelKind)
{
	mModelKind = inModelKind;
}


// ---------------------------------------------------------------------------
//	¥ IsLazy
// ---------------------------------------------------------------------------
//	Return whether this ModelObject is "lazy."
//
//	A "lazy instantiated" model object is a transient object created solely
//	for the purpose of handling an AppleEvent.  After AppleEvent processing,
//	lazy objects are automatically deleted.
//
//	Default initialization sets laziness to false.  Laziness may be changed
//	with SetLaziness.

Boolean
LModelObject::IsLazy() const
{
	return mLaziness;
}


// ---------------------------------------------------------------------------
//	¥ SetLaziness
// ---------------------------------------------------------------------------
//	Changes whether this ModelObject is "Lazy."
//
//	Default initialization sets laziness to false.

void
LModelObject::SetLaziness(
	Boolean		inBeLazy)
{
	if (mLaziness != inBeLazy) {

		if (inBeLazy) {
			AddLazy(this);
		} else {
			RemoveLazy(this);
		}

		mLaziness = inBeLazy;
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetUseSubModelList
// ---------------------------------------------------------------------------
//	Changes whether a "submodel list" is kept for this ModelObject.
//
//	A submodel list allows submodels to be added to an LModelObject without
//	having to override many of the "GetSubModelBy..." functions.  This is
//	useful in cases where a model object, such as a window, has several AEOM
//	addressable submodels.
//
//	Default initialization creates ModelObjects that don't keep a submodel
//	list
//
//	Note:	It doesn't makes sense to turn this feature on then off

void
LModelObject::SetUseSubModelList(
	Boolean		inUseSubModelList)
{
	if (inUseSubModelList) {

		if (mSubModels == nil) {
			mSubModels = new TArray<LModelObject*>;
		}

	} else {
		delete mSubModels;
		mSubModels = nil;
	}
}


// ---------------------------------------------------------------------------
//	¥ AddSubModel
// ---------------------------------------------------------------------------
//	Notify a ModelObject that is has a new SubModel
//
//	If the submodel list is being used, this default method will add the
//	submodel to the submodel list.
//
//	You will not need to call this method.  Instead use:
//
//		inSubModel->SetSuperModel(itsSuperModel);

void
LModelObject::AddSubModel(
	LModelObject*	inSubModel)
{
	if (mSubModels != nil) {
		SInt32	index = mSubModels->FetchIndexOf(inSubModel);

		if (index) {
			Assert_(false);	//	Adding a pre-existent submodel!
			return;
		}

		mSubModels->AddItem(inSubModel);
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveSubModel
// ---------------------------------------------------------------------------
//	Notify a ModelObject that a SubModel is being deleted
//
//	If the submodel list is being used, this method will remove the submodel
//	from the submodel list.
//
//	You will not need to call this method.  Instead use:
//
//		inSubModel->SetSuperModel(nil);

void
LModelObject::RemoveSubModel(
	LModelObject*	inSubModel)
{
	if (mSubModels) {
		SInt32	index = mSubModels->FetchIndexOf(inSubModel);

		if (index == 0) {
			Assert_(false);	//	trying to remove non-existent submodel
			return;
		}

		mSubModels->RemoveItemsAt(1, index);
	}
}


// ---------------------------------------------------------------------------
//	¥ IsSubModelOf
// ---------------------------------------------------------------------------
//	Returns whether this is a submodel of inSuperModel

Boolean
LModelObject::IsSubModelOf(
	LModelObject*	inSuperModel) const
{
	if (mSuperModel == inSuperModel) {
		return true;
	}

	if (mSuperModel == nil) {
		return false;
	}

	return mSuperModel->IsSubModelOf(inSuperModel);
}

// ---------------------------------------------------------------------------
//	¥ CountSubModels
// ---------------------------------------------------------------------------
//	Return number of SubModels of the specified type
//
//	Must be overridden by subclasses which have SubModels that aren't
//	implemented using the submodel list (ie lazy instantiated submodels).
//
//	When overriding, you should add the inherited result to your overridden
//	result.

SInt32
LModelObject::CountSubModels(
	DescType	inModelID) const
{
	SInt32	count = 0;

	if (GetDefaultSubModel() != nil) {
		count += GetDefaultSubModel()->CountSubModels(inModelID);
	}

	if (mSubModels != nil) {
		TArrayIterator<LModelObject*>	iterator(*mSubModels);
		LModelObject	*p;

		while (iterator.Next(p)) {
			if ((p->GetModelKind() == inModelID) || (inModelID == typeWildCard))
				count++;
		}
	}

	return count;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetDefaultSubModel
// ---------------------------------------------------------------------------
//	Returns the default submodel (if any) of this ModelObject.
//
//	The default submodel allows this ModelObject to serve as an alias to
//	the submodel. Its use simplifies scripts.

LModelObject *
LModelObject::GetDefaultSubModel() const
{
	return mDefaultSubModel;
}


// ---------------------------------------------------------------------------
//	¥ SetDefaultSubModel
// ---------------------------------------------------------------------------
//	Sets the default submodel of this ModelObject.
//
//	The default submodel must already be a submodel of this ModelObject.

void
LModelObject::SetDefaultSubModel(
	LModelObject*	inSubModel)
{
	if (mDefaultSubModel != inSubModel) {

		if (mDefaultSubModel != nil) {
			mDefaultSubModel->SetDefaultSuperModel(nil);
		}

		mDefaultSubModel = inSubModel;

		if (mDefaultSubModel != nil) {
			mDefaultSubModel->SetDefaultSuperModel(this);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ IsDefaultSubModel
// ---------------------------------------------------------------------------
//	Is "this" ModelObject a default submodel of the
//	DefaultModel (Application).

Boolean
LModelObject::IsDefaultSubModel() const
{
	if (this == GetDefaultModel()) {
		return true;
	}

	if (GetDefaultSuperModel() != nil) {
		return GetDefaultSuperModel()->IsDefaultSubModel();

	} else {
		return false;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetDefaultSuperModel
// ---------------------------------------------------------------------------
//	Returns the default supermodel of this ModelObject.
//
//	You should not need to explicitly call this method.
//	You should not need to override this method.

LModelObject *
LModelObject::GetDefaultSuperModel() const
{
	return mDefaultSuperModel;
}


// ---------------------------------------------------------------------------
//	¥ SetDefaultSuperModel
// ---------------------------------------------------------------------------
//	Sets the default supermodel of this ModelObject.
//
//	You should not need to explicitly call this method -- use
//	SetDefaultSubModel instead and it will call this method.
//	You should not need to override this method.

void
LModelObject::SetDefaultSuperModel(LModelObject *inSuperModel)
{
	mDefaultSuperModel = inSuperModel;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetModelToken
// ---------------------------------------------------------------------------
//	Get a Token for the specified Model(s).
//
//	This method not only includes possible resolutions using "this" but also
//	recursively includes resolutions on the default submodel of "this."
//
//	This function gives preference to resolutions off of the default submodel
//	rather than resolutions off of "this."  This is because:
//
//	¥	the default submodel is probably what was wanted anyway,
//
//	¥	if there is no default submodel there is no real performance penatly,
//
//	¥	it prevents "link" objects from erroneously adding their submodels
//		to the output token.
//
//	If a token is found in the default submodel, no resolution based off of
//	"this" will be made.  This has the consequence of models in the default
//	submodel effectively hiding similar models of "this."

void
LModelObject::GetModelToken(
	DescType		inModelID,
	DescType		inKeyForm,
	const AEDesc&	inKeyData,
	AEDesc&			outToken) const
{
	AEDesc	originalOutToken = outToken;

	Assert_(outToken.descriptorType == typeNull ||
			outToken.descriptorType == type_ModelToken ||
			outToken.descriptorType == typeAEList);

									//	Recursively try default submodel(s)
	if (GetDefaultSubModel() != nil) {
		StDisableDebugThrow_();		// Ok to fail

		try {
			GetDefaultSubModel()->GetModelToken(inModelID, inKeyForm,
												inKeyData, outToken);
		}

		catch (...) { }				//	Continue after failure
	}

		// If the default submodel didn't set the outToken,
		// we get the token for "this" model object

	 if (outToken.descriptorType == typeNull) {
		try {
			GetModelTokenSelf(inModelID, inKeyForm, inKeyData, outToken);
		}

		catch (const LException& inException) {
			throw;
		}

		catch (ExceptionCode inErr) {
			throw;
		}
		
		catch (...) {
			ThrowOSErr_(errAENoSuchObject);
		}
	}

		// If the outToken has not changed since entry to this function,
		// then we didn't get a valid token. This is an error.

	if ( (outToken.descriptorType == originalOutToken.descriptorType) &&
		 (outToken.dataHandle == originalOutToken.dataHandle) ) {
		ThrowOSErr_(errAENoSuchObject);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetModelTokenSelf
// ---------------------------------------------------------------------------
//	Get a Token for the specified Model(s) without considering the
//	default submodel.
//
//	This function dispatches a call to an accessor using a specific
//	means of identification (key form). You will rarely override this.
//
//	Do not call this member.  Consider calling GetModelToken instead.

void
LModelObject::GetModelTokenSelf(
	DescType		inModelID,
	DescType		inKeyForm,
	const AEDesc&	inKeyData,
	AEDesc&			outToken) const
{
	switch (inKeyForm) {

		case formAbsolutePosition: {
			SInt32		subPosition = 0;
			SInt32		subCount = 0;
			DescType	subSpec = typeNull;
			bool		needCount = false;

			if (inKeyData.descriptorType == typeAbsoluteOrdinal) {

				#if ACCESSOR_CALLS_ARE_FUNCTIONS
					ThrowIfOSErr_( ::AEGetDescData(&inKeyData, &subSpec, sizeof(subSpec)));
				#else
					subSpec = **(DescType**) inKeyData.dataHandle;
				#endif

			} else {
				UExtractFromAEDesc::TheSInt32(inKeyData, subPosition);
			}

			if (subPosition < 0) {
				needCount = true;
			}

			switch (subSpec) {
				case kAEMiddle:
				case kAELast:
				case kAEAny:
					needCount = true;
					break;
			}

			if (needCount) {
				subCount = CountSubModels(inModelID);
			}

			switch (subSpec) {

				case kAEFirst:
					subPosition = 1;
					break;

				case kAEMiddle:
					subPosition = (subCount + 1) / 2;
					break;

				case kAELast:
					subPosition = subCount;
					break;

				case kAEAny:
					subPosition = 0;
					if (subCount > 0) {
						subPosition = ((UInt16) ::Random() % (UInt16) subCount)
										+ 1;
					}
					break;

				case kAEAll:
					subPosition = position_AllSubModels;
					break;

				default:
					if (subPosition < 0) {	// Negative position counts back from
											//   end, with -1 being the last item
						subPosition += subCount + 1;
						
						if (subPosition <= 0) {	// Non-existent position
							ThrowOSErr_(errAENoSuchObject);
						}
					}
					break;
			}

			if (subPosition == position_AllSubModels) {
				GetAllSubModels(inModelID, outToken);
			} else if (subPosition > 0) {
				GetSubModelByPosition(inModelID, subPosition, outToken);
			}
			break;
		}

		case formName: {
			Str255	subModelName;
			UExtractFromAEDesc::ThePString(inKeyData, subModelName, sizeof(subModelName));
			GetSubModelByName(inModelID, subModelName, outToken);
			break;
		}

		case formUniqueID:
			GetSubModelByUniqueID(inModelID, inKeyData, outToken);
			break;

		case formRelativePosition: {
			OSType	relativePosition;
			UExtractFromAEDesc::TheEnum(inKeyData, relativePosition);
			GetModelByRelativePosition(inModelID, relativePosition, outToken);
			break;
		}

		case formPropertyID: {
			DescType		propertyID;
			LModelObject*	property = nil;

			UExtractFromAEDesc::TheType(inKeyData, propertyID);

									// First see there's an actual
									//   object for the property
			property = GetModelProperty(propertyID);

			if ( (property == nil)  &&
				 AEPropertyExists(propertyID) ) {
				 					// No property object, but the property
				 					//   does exist so we make a lazy
				 					//   object for this property

				property = new LModelProperty(propertyID, const_cast<LModelObject *>(this));
			}

			if (property != nil) {
				PutInToken(property, outToken);
			}
			break;
		}

		default:					// formRange, formTest
			GetSubModelByComplexKey(inModelID, inKeyForm, inKeyData,
									outToken);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetSubModelByPosition
// ---------------------------------------------------------------------------
//	Pass back a Token for the SubModel(s) of the specified type at the
//	specified position (1 being the first).
//
//	Must be overridden by subclasses which have SubModels that aren't
//	implemented using the submodel list (ie lazy instantiated submodels).

void
LModelObject::GetSubModelByPosition(
	DescType		inModelID,
	SInt32			inPosition,
	AEDesc			&outToken) const
{
	if (mSubModels != nil) {
		TArrayIterator<LModelObject*>	iterator(*mSubModels);
		SInt32			index = 0;
		LModelObject	*p;
		Boolean			found = false;

		while (iterator.Next(p)) {
			if ((p->GetModelKind() == inModelID) || (inModelID == typeWildCard)) {
				index++;
				if (index == inPosition) {
					found = true;
					break;
				}
			}
		}

		if (found) {
			PutInToken(p, outToken);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ GetSubModelByName
// ---------------------------------------------------------------------------
//	Pass back a Token for the SubModel(s) of the specified type with the
//	specified name
//
//	Must be overridden by subclasses which have SubModels that aren't
//	implemented using the submodel list (ie lazy instantiated submodels).

void
LModelObject::GetSubModelByName(
	DescType		inModelID,
	Str255			inName,
	AEDesc			&outToken) const
{
	if (mSubModels) {
		TArrayIterator<LModelObject*>	iterator(*mSubModels);
		LModelObject*	p;
		Boolean			found = false;
		Str255			str;

		while (iterator.Next(p)) {
			if ((p->GetModelKind() == inModelID) || (inModelID == typeWildCard)) {
				if (::EqualString(p->GetModelName(str), inName, false, false)) {
					found = true;
					break;
				}
			}
		}

		if (found) {
			PutInToken(p, outToken);
			return;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ GetSubModelByUniqueID
// ---------------------------------------------------------------------------
//	Pass back a Token for the SubModel(s) of the specified type with the
//	specified unique ID
//
//	Must be overridden by subclasses which have SubModels that aren't
//	implemented using the submodel list (ie lazy instantiated submodels).
//
//	It is up to you to decide what constitutes a unique ID and you must also
//	provide a CompareToUniqueID().

void
LModelObject::GetSubModelByUniqueID(
	DescType		inModelID,
	const AEDesc	&inKeyData,
	AEDesc			&outToken) const
{
	if (mSubModels != nil) {
		TArrayIterator<LModelObject*>	iterator(*mSubModels);
		LModelObject	*p;
		Boolean			found = false;

		while (iterator.Next(p)) {
			if (p->GetModelKind() == inModelID) {
				if (p->CompareToUniqueID(kAEEquals, inKeyData)) {
					found = true;
					break;
				}
			}
		}

		if (found) {
			PutInToken(p, outToken);
			return;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ GetModelByRelativePosition
// ---------------------------------------------------------------------------
//	Pass back a Token for the Model of the specified type at the specified
//	relative position ("before" or "after" this ModelObject).
//
//	This function handles the case where the Model to get is of the same
//	kind as this ModelObject by getting the position of this ModelObject,
//	then getting the ModelObject at the position before or after it.
//	For example, to get the "paragraph after this paragraph", we get the
//	position of the "this" paragraph, add one, then get the paragraph at
//	that position.
//
//	Subclass should override this function to implement a more efficient
//	way to get the next or previous item, or to handle the case where the
//	object to get is a different kind of object. For example, to be able
//	to get the "table after this paragraph".

void
LModelObject::GetModelByRelativePosition(
	DescType		inModelID,
	OSType			inRelativePosition,
	AEDesc			&outToken) const
{
	if (inModelID == GetModelKind()) {
		LModelObject	*theSuper = mSuperModel;
		if (theSuper == nil) {
			theSuper = GetDefaultModel();
		}

		SInt32	thePosition = theSuper->GetPositionOfSubModel(inModelID, this);

		switch (inRelativePosition) {

			case kAENext:
				thePosition += 1;
				break;

			case kAEPrevious:
				thePosition -= 1;
				break;

			default:
				return;
		}
		theSuper->GetSubModelByPosition(inModelID, thePosition, outToken);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetSubModelByComplexKey
// ---------------------------------------------------------------------------
//	Pass back a Token for the SubModel(s) of the specified type identified
//	by a complex key. Complex keys are formRange, formTest, and formWhose.
//
//	Subclasses which support complex keys for identifying SubModels must
//	override this function.

void
LModelObject::GetSubModelByComplexKey(
	DescType		/* inModelID */,
	DescType		/* inKeyForm */,
	const AEDesc&	/* inKeyData */,
	AEDesc&			/* outToken */) const
{
	ThrowOSErr_(errAEEventNotHandled);
}


// ---------------------------------------------------------------------------
//	¥ GetAllSubModels
// ---------------------------------------------------------------------------
//	Pass back a Token list for all SubModels of the specified type
//
//	This function uses a brute force approach:
//		Get the count of items
//		Get Token for each item in order and add it to the list
//
//	Override this function if a ModelObject can create this Token list
//	in a more efficient manner.

void
LModelObject::GetAllSubModels(
	DescType		inModelID,
	AEDesc			&outToken) const
{
	SInt32	subCount = CountSubModels(inModelID);
	OSErr	err;

	if (outToken.descriptorType == typeNull) {
		err = ::AECreateList(nil, 0, false, &outToken);
		ThrowIfOSErr_(err);
	}

	for (SInt32 i = 1; i <= subCount; i++) {
		StAEDescriptor	subToken;

		GetSubModelByPosition(inModelID, i, subToken);
		err = ::AEPutDesc(&outToken, 0, subToken);
		ThrowIfOSErr_(err);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetPositionOfSubModel
// ---------------------------------------------------------------------------
//	Return the absolute position of the specified SubModel, with 1 being
//	the first
//
//	Must be overridden by subclasses which have SubModels that aren't
//	implemented using the submodel list (ie lazy instantiated submodels).

SInt32
LModelObject::GetPositionOfSubModel(
	DescType			inModelID,
	const LModelObject	*inSubModel) const
{
	SInt32			index = 0;

	if (mSubModels != nil) {			// Search submodel list
		TArrayIterator<LModelObject*>	iterator(*mSubModels);
		LModelObject	*p;

		while (iterator.Next(p)) {
			if ((p->GetModelKind() == inModelID) || (inModelID == typeWildCard)) {

					// Kind matches. Increment index since we want
					// the position of the model with respect to
					// other models of the same kind.

				index++;
				if (p == inSubModel) {	// This is it. Exit loop.
					break;
				}
			}
		}
	}

	if (index == 0) {
		ThrowOSErr_(errAENoSuchObject);	//	Actually more of an internal
	}									//	implementation error.

	return index;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetInsertionTarget
//		¥ GetInsertionContainer
//		¥ GetInsertionElement
// ---------------------------------------------------------------------------
//	These three functions help in dealing with typeInsertionLoc parameters.
//
//	GetInsertionTarget converts the ModelObject referred to in the kAEObject
//	typeInsertionLoc sub parameter to the actual insertion target.
//	Specifically, GetInsertionTarget takes default submodels into
//	consideration.  The GetInsertionTarget result should be used as the
//	object to pass the other two messages thru.
//
//	GetInsertionContainer returns the "container" object of the
//	typeInsertionLoc parameter.
//
//	GetInsertionElement returns the actual element that represents the
//	typeInsertionLoc ModelObject.
//
//	All three methods take an "inInsertPosition" DescType parameter.
//	InInsertPosition corresponds to the kAEPosition value of the
//	typeInsertionLoc record.  If the AppleEvent parameter corresponding to
//	an insertion location is an object specifier instead of a
//	typeInsertionLoc, use an inInsertPosition value of typeNull.  This
//	convention is useful when an insertion location is completely
//	representable by an object specifier rather than having to use a
//	typeInsertionLoc record.
//
//	You may need to override GetInsertionElement to lazy instantiate an
//	object, or allow its return value to be nil.  The later works fine
//	in HandleCreateElementEvents where the element is the result of the
//	HandleCreateElementEvent.

LModelObject*
LModelObject::GetInsertionTarget(
	DescType	inInsertPosition) const
{
	LModelObject	*model = nil;

	if (GetDefaultSubModel()) {
		model = GetDefaultSubModel()->GetInsertionTarget(inInsertPosition);
	}

	if (model == nil) {
		model = const_cast<LModelObject *>(this);
	}

	return model;
}


LModelObject*
LModelObject::GetInsertionContainer(
	DescType	inInsertPosition) const
{
	LModelObject	*container = nil;

	switch (inInsertPosition) {

		case typeNull:
			container = (const_cast<LModelObject *>(this))->GetSuperModel();
			break;

		case kAEBefore:		// For these positions, the insert target
		case kAEAfter:		// specifies an object in the same
		case kAEReplace:	// container as that in which to create
							// the new element
			container = (const_cast<LModelObject *>(this))->GetSuperModel();
			break;

		case kAEBeginning:	// For these positions, the insert target
		case kAEEnd:		// is the container in which to create
							// the new element
			container = const_cast<LModelObject *>(this);
			break;

		default:
			ThrowOSErr_(errAEEventNotHandled);
			break;
	}

	return container;
}


LModelObject*
LModelObject::GetInsertionElement(
	DescType	inInsertPosition) const
{
	LModelObject*	model = nil;

	switch (inInsertPosition) {

		case typeNull:
			model = const_cast<LModelObject *>(this);
			break;

		case kAEReplace:
			model = const_cast<LModelObject *>(this);
			break;

		case kAEBefore:
		case kAEAfter:
		case kAEBeginning:
		case kAEEnd:
			//	you need to override this method to do the mapping to
			//	outModel or, if this is being used in a HandleCreateElementEvent,
			//	recognize the outModel hasn't been created yet.
			model = nil;
			break;

		default:
			Throw_(errAEEventNotHandled);
			break;
	}

	return model;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetModelName
// ---------------------------------------------------------------------------
//	Return the name of a ModelObject

StringPtr
LModelObject::GetModelName(
	Str255	outModelName) const
{
	outModelName[0] = 0;
	return outModelName;
}


// ---------------------------------------------------------------------------
//	¥ CompareToModel
// ---------------------------------------------------------------------------
//	Return result of comparing this ModelObject with another one
//
//	Subclasses must override this method to support accessing objects
//	by selection criteria, commonly called "whose" clauses.
//		For example:  get all words equal to word 1

Boolean
LModelObject::CompareToModel(
	DescType		/* inComparisonOperator */,
	LModelObject*	/*	inCompareModel */) const
{
	ThrowOSErr_(errAEEventNotHandled);
	return false;
}


// ---------------------------------------------------------------------------
//	¥ CompareToDescriptor
// ---------------------------------------------------------------------------
//	Return result of comparing this ModelObject with Descriptor data
//
//	Subclasses must override this method to support accessing objects
//	by selection criteria, commonly called "whose" clauses.
//		For example:  get all words that contain "foo"

Boolean
LModelObject::CompareToDescriptor(
	DescType		/* inComparisonOperator */,
	const AEDesc&	/* inCompareDesc */) const
{
	ThrowOSErr_(errAEEventNotHandled);
	return false;
}


// ---------------------------------------------------------------------------
//	¥ CompareToUniqueID
// ---------------------------------------------------------------------------
//	Return result of comparing this ModelObject with a "unique id."
//
//	It is up to you to decide what constitutes a unique ID.

Boolean
LModelObject::CompareToUniqueID(
	DescType		/* inComparisonOperator */,
	const AEDesc&	/* inCompareDesc */) const
{
	ThrowOSErr_(errAEEventNotHandled);
	return false;
}


// ---------------------------------------------------------------------------
//	¥ CompareProperty
// ---------------------------------------------------------------------------
//	Return the result of comparing a property of this ModelObject with
//	another object
//
//	Subclasses must override this method to support accessing objects by
//	selection criteria on their properties, commonly called "whose" clauses.
//		For example:  get all words whose point size is less than 12

Boolean
LModelObject::CompareProperty(
	DescType		/* inPropertyID */,
	DescType		/* inComparisonOperator */,
	const AEDesc&	/* inCompareObjectOrDesc */) const
{
	ThrowOSErr_(errAEEventNotHandled);
	return false;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetModelPropertyAll
// ---------------------------------------------------------------------------
//	Return a ModelObject object representing the specified property
//
//	This implementation always creates a LModelProperty object for the
//	specificied property (although the property may not really exist).
//	For nonexistent properties, a later attempt to get or set the property
//	value will pass back an error.
//
//	To return a special LModelObject for a given property,
//	override GetModelProperty (which is called by this function).

LModelObject*
LModelObject::GetModelPropertyAll(
	DescType	inProperty) const
{
	LModelObject	*property = GetModelProperty(inProperty);

	if (property == nil) {
		property = new LModelProperty(inProperty, const_cast<LModelObject *>(this));
	}

	return property;
}


// ---------------------------------------------------------------------------
//	¥ GetModelProperty
// ---------------------------------------------------------------------------
//	Return a ModelObject object for explicitly defined properties
//
//	Must be overridden for subclasses which return "special" ModelObjects for
//	given property id's.
//
//	For the default case in overrides, return the inherited value.

LModelObject*
LModelObject::GetModelProperty(
	DescType	/* inProperty */) const
{
	return nil;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ MakeSpecifier
// ---------------------------------------------------------------------------
//	Make an Object Specifier for a ModelObject
//
//	This is a helper function that uses recursion to call MakeSelfSpecifier
//	for a ModelObject and all its SuperModels. You will not need to
//	override this function.

void
LModelObject::MakeSpecifier(
	AEDesc	&outSpecifier) const
{
	if (GetDefaultSuperModel() != nil) {
		GetDefaultSuperModel()->MakeSpecifier(outSpecifier);
	} else {
		StAEDescriptor		superSpecifier;

		if (mSuperModel != nil) {
			mSuperModel->MakeSpecifier(superSpecifier);
		}

		MakeSelfSpecifier(superSpecifier, outSpecifier);
	}
}


// ---------------------------------------------------------------------------
//	¥ MakeSelfSpecifier
// ---------------------------------------------------------------------------
//	Make an Object Specifier for a ModelObject
//
//	This function creates a specifier using the absolute position of
//	a ModelObject within its SuperModel.
//
//	Override this function for subclasses which can't be specified by
//	absolute position or for which another means of identification
//	(for example, by name) is more appropriate or that don't have
//	a SuperModel (access from null container).

void
LModelObject::MakeSelfSpecifier(
	AEDesc	&inSuperSpecifier,
	AEDesc	&outSelfSpecifier) const
{
		// Find position of this ModelObject within its SuperModel
		// Error if SuperModel does not exist or if SuperModel
		// can't return a valid position for this object

	if (mSuperModel == nil) {
		ThrowOSErr_(errAEEventNotHandled);
	}

	SInt32	modelIndex = mSuperModel->GetPositionOfSubModel(
										GetModelKind(), this);
	if (modelIndex == 0) {
		ThrowOSErr_(errAEEventNotHandled);
	}

	StAEDescriptor	absPosKeyData;
	OSErr	err = ::CreateOffsetDescriptor(modelIndex, absPosKeyData);
	ThrowIfOSErr_(err);

	err = ::CreateObjSpecifier(GetModelKind(), &inSuperSpecifier,
			formAbsolutePosition, absPosKeyData, false,
			&outSelfSpecifier);
	ThrowIfOSErr_(err);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ HandleAppleEventAll
// ---------------------------------------------------------------------------
//	Try handling an AppleEvent by both "this" ModelObject and its default
//	submodel.
//
//	If submodel handling succeeds, "this" is not tried.

void
LModelObject::HandleAppleEventAll(
	const AppleEvent	&inAppleEvent,
	AppleEvent			&outAEReply,
	AEDesc				&outResult,
	long				inAENumber)
{
	bool	handled = false;

	if (GetDefaultSubModel()) {
		try {
			StDisableDebugThrow_();	// Ok to fail

			GetDefaultSubModel()->HandleAppleEventAll(inAppleEvent,
									outAEReply, outResult, inAENumber);
			handled = true;
		}

		catch (ExceptionCode inErr) {
									// If default SubModel fails for some
									//   specific reason, rethrow the error
			if (inErr != errAEEventNotHandled) {
				throw;
			}
									// A generic errAEEventNotHandled means
									//   that the SubModel doesn't deal with
									//   such events, just continue and give
									//   this Model a chance to handle it.
		}

		catch (const LException& inException) {
			if (inException.GetErrorCode() != errAEEventNotHandled) {
				throw;
			}
		}
	}

	if (!handled) {			// Default SubModel didn't handle it, so this
							// object must handle it
		HandleAppleEvent(inAppleEvent, outAEReply, outResult, inAENumber);
	}
}


// ---------------------------------------------------------------------------
//	¥ HandleCreateElementEventAll
// ---------------------------------------------------------------------------
//	Try handling a CreateElementEvent by both "this" ModelObject and its
//	default submodel.
//
//	If submodel handling succeeds, "this" is not tried.

LModelObject*
LModelObject::HandleCreateElementEventAll(
	DescType			inElemClass,
	DescType			inInsertPosition,
	LModelObject*		inTargetObject,
	const AppleEvent&	inAppleEvent,
	AppleEvent&			outAEReply)
{
	LModelObject*	model	= nil;
	bool			handled	= false;

	if (GetDefaultSubModel()) {		// First, see if defaul submodel can
									//   create the element
		try {
			StDisableDebugThrow_();	// Ok to fail

			model = GetDefaultSubModel()->
						HandleCreateElementEventAll(
									inElemClass, inInsertPosition,
									inTargetObject, inAppleEvent, outAEReply);
			handled = true;
		}

		catch (...) { }				// If we fail, "handled" will be false
	}

	if (not handled) {				// Default submodel didn't create element.
									// Let "this" ModelObject create it.
		model = HandleCreateElementEvent(
								inElemClass, inInsertPosition,
								inTargetObject, inAppleEvent, outAEReply);
	}

	return model;
}


// ---------------------------------------------------------------------------
//	¥ HandleAppleEvent
// ---------------------------------------------------------------------------
//	Respond to an AppleEvent. This is the default handler for AppleEvents
//	that do not have a specific handler.
//
//	Subclasses must override this function to support AppleEvents other than:
//		Create Element,
//		Get Data,
//		Set Data,
//		Clone,
//		Move,
//		CountElements,
//		Delete,
//
//
//	The events above have specific handler methods which may be overridden.

void
LModelObject::HandleAppleEvent(
	const AppleEvent&	inAppleEvent,
	AppleEvent&			outAEReply,
	AEDesc&				outResult,
	long				inAENumber)
{
	switch (inAENumber) {

		case ae_Clone:
			HandleClone(inAppleEvent, outAEReply, outResult);
			break;

		case ae_Move:
			HandleMove(inAppleEvent, outAEReply, outResult);
			break;

//		case ae_CountElements:			// See LModelDirector::HandleCountElementsEvent

//		case ae_Create:					// See HandleCreateElementEvent

		case ae_Delete:
			HandleDelete(outAEReply, outResult);
			break;

//		case ae_DoObjectsExist:			// See LModelDirector::HandleDoObjectsExistEvent

//		case ae_GetClassInfo:			// Not implemented

		case ae_GetData:
		case ae_GetDataSize:
		case ae_SetData:
		{
			//	Must be referring to contents so...  simulate it.
			LModelObject	*content = GetModelPropertyAll(pContents);
			ThrowIfNil_(content);
			content->HandleAppleEvent(inAppleEvent, outAEReply, outResult, inAENumber);
			break;
		}

		default:
			ThrowOSErr_(errAEEventNotHandled);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ HandleCreateElementEvent
// ---------------------------------------------------------------------------
//	Respond to a Create Element AppleEvent ("make new" in AppleScript).
//
//	The parameters specify the Class ID for the new element, and where to
//	insert the new element in relation to a target object. Also, the
//	AppleEvent record may contain additional parameters that specify
//	initial values for the new element.
//
//	Subclasses which have SubModels which can be dynamically created should
//	override this function. Return a pointer to the newly created SubModel.
//	The calling function takes care of putting an object specifier for
//	this new SubModel in the AppleEvent reply.

LModelObject*
LModelObject::HandleCreateElementEvent(
	DescType			/* inElemClass */,
	DescType			/* inInsertPosition */,
	LModelObject*		/* inTargetObject */,
	const AppleEvent&	/* inAppleEvent */,
	AppleEvent&			/* outAEReply */)
{
	ThrowOSErr_(errAEEventNotHandled);
	return nil;
}


// ---------------------------------------------------------------------------
//	¥ HandleCount
// ---------------------------------------------------------------------------
//	Respond to the Count Elements AppleEvent ("count" in AppleScript).
//
//	The parameters specify the Class ID of submodels to count.
//
//	You should not need to explicitly call this method.
//	You should not need to override this method.

void
LModelObject::HandleCount(
	DescType			inModelID,
	AppleEvent			&outResult)
{
	SInt32	count = CountSubModels(inModelID);

	UAEDesc::AddPtr(&outResult, 0, typeLongInteger, &count, sizeof(count));
}

// ---------------------------------------------------------------------------
//	¥ HandleDelete
// ---------------------------------------------------------------------------
//	Respond to the Delete AppleEvent ("delete" in AppleScript).
//
//	You should not need to explicitly call this method.
//	You may need to override and inherit this method.

void
LModelObject::HandleDelete(
	AppleEvent&	/* outAEReply */,
	AEDesc&		/* outResult */)
{
	if ((GetModelKind() == cProperty) ||		// Can't delete a Property
		(GetModelKind() == cApplication)) {		//   or the Application

		Throw_(errAEEventNotHandled);
	}

	SetLaziness(true);	//	Should automatically delete when AE is done
}


// ---------------------------------------------------------------------------
//	¥ HandleClone
// ---------------------------------------------------------------------------
//	Respond to the Clone AppleEvent ("duplicate" in AppleScript).
//
//	You should not need to explicitly call this method.
//	You rarely need to override this method.

void
LModelObject::HandleClone(
	const AppleEvent&	inAppleEvent,
	AppleEvent&			outAEReply,
	AEDesc&				outResult)
{
		// A Duplicate event is functionally the same as a Create Element
		// event. We just need to get the property data from the object
		// to duplicate, and determine the insertion location. Then, we
		// just send ourself a Create Element event.

	OSErr			err = noErr;
	StAEDescriptor	createEvent;
	StAEDescriptor	replyEvent;

	{
			// Create Element event has 4 parameters:
			//
			//		keyAEObjectClass - class of the new object
			//		keyAEInsertHere - where to put the new object
			//		keyAEPropData - properties for the new object
			//		keyAEData - data for the new object

		UAppleEventsMgr::MakeAppleEvent(kAECoreSuite, kAECreateElement, createEvent);

			// * keyAEObjectClass
			//
			//		This class of object to create is the kind of "this" object

		DescType		objectClass = GetModelKind();
		StAEDescriptor	classD(typeType, &objectClass, sizeof(objectClass));
		UAEDesc::AddKeyDesc(createEvent, keyAEObjectClass, classD);

			// * keyAEInsertHere
			//
			//		Duplicate event also has a keyAEInsertHere parameter

		StAEDescriptor	insertHere(inAppleEvent, keyAEInsertHere);
		switch (insertHere.DescriptorType()) {

			case typeNull: {

					// If the Duplicate event has no insertion location, the
					// default location is after the object to duplicate,
					// which is "this" object

				StAEDescriptor	thisObject;
				MakeSpecifier(thisObject);

				StAEDescriptor	defaultInsertHere;
				UAEDesc::MakeInsertionLoc(thisObject, kAEAfter, defaultInsertHere);

				UAEDesc::AddKeyDesc(createEvent, keyAEInsertHere, defaultInsertHere);
				break;
			}

			default: {

					// Use same insertion location for Create Element as is
					// used for Duplicate

				UAEDesc::AddKeyDesc(createEvent, keyAEInsertHere, insertHere);
				break;
			}
		}

			// * keyAEPropData
			//
			//		Get important properties for this object. If such properties
			//		exist, pass them to the Create Element event

		StAEDescriptor	objProps;
		GetImportantAEProperties(objProps);
		if (objProps.IsNotNull()) {
			UAEDesc::AddKeyDesc(createEvent, keyAEPropData, objProps);
		}

			// * keyAEData
			//
			//		Not used. We initialize duplicate object using properties.
	}

									// Execute create element event (but don't record)
	try {
		StLazyLock	lockMe(this);	//	Don't lose ourself from implied FinalizeLazies().
		UAppleEventsMgr::SendAppleEventWithReply(createEvent, replyEvent, false);
	}

	catch (const LException& inException) {
									// Create element failed. Copy error
									//   number and string from create
									//   element reply into reply for the
									//   clone event
		StAEDescriptor	errDesc;
		err = ::AEGetParamDesc(replyEvent, keyErrorNumber, typeWildCard, errDesc);
		if (err == noErr) {
			::AEPutParamDesc(&outAEReply, keyErrorNumber, errDesc);
		}

		errDesc.Dispose();
		err = ::AEGetParamDesc(replyEvent, keyErrorString, typeWildCard, errDesc);
		if (err == noErr) {
			::AEPutParamDesc(&outAEReply, keyErrorString, errDesc);
		}

		throw;
	}

							//	Put result of create element event in reply
	err = AEGetKeyDesc(replyEvent, keyAEResult, typeObjectSpecifier,
						&outResult);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ HandleMove
// ---------------------------------------------------------------------------
//	Respond to the Move AppleEvent ("move" in AppleScript).
//
//	You should not need to explicitly call this method.
//	You will rarely need to override this method.

void
LModelObject::HandleMove(
	const AppleEvent	&inAppleEvent,
	AppleEvent			&outAEReply,
	AEDesc				&outResult)
{
		// To "move" an object, we clone the object, putting the clone at
		// the desired destination. Then we delete the original object.

		// Clone behavior
		//
		//		If this is an offset based lazy object, the create element event
		//		in the default HandleClone should "magically" update this object's
		//		offsets as necessary.

	StAEDescriptor	cloneResult;
	HandleClone(inAppleEvent, outAEReply, cloneResult);

		// Delete behavior
		//
		//		If the object to be removed is also in a lazy object scheme,
		//		conversion of the above cloneResult to an LModelObject will allow
		//		the result object specifier (the new copy) to have its offsets
		//		"magically" updated by the HandleDelete.  So, get the clone now,
		//		do the delete, then make the adjusted result specifier.

	StAEDescriptor	token;
	OSErr	err = LModelDirector::Resolve(cloneResult, token);
	ThrowIfOSErr_(err);
	LModelObject*	clone = GetModelFromToken(token);

	StAEDescriptor	deleteResult;
	HandleDelete(outAEReply, deleteResult);

		// "Hole specifier?"
		//
		//	As an aid for undo inside of a PowerPlant app, the HandleDelete
		//	has already set the keyAEInsertHere parameter of the reply to the "hole"
		//	left by the deletion.

		// Result
		//
		//	Fill in the adjusted object specifier

	clone->MakeSpecifier(outResult);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetAEProperty
// ---------------------------------------------------------------------------
//	Return a descriptor for the specified Property
//
//	Subclasses which have Properties must override this function

void
LModelObject::GetAEProperty(
	DescType		inProperty,
	const AEDesc&	/* inRequestedType */,
	AEDesc			&outPropertyDesc) const
{
	switch (inProperty) {
		case pClass:
		{
			DescType	value = GetModelKind();

			UAEDesc::AddPtr(&outPropertyDesc, 0, typeType, &value, sizeof(value));
			break;
		}

		case pContents:
			MakeSpecifier(outPropertyDesc);
			break;

		default:
			ThrowOSErr_(errAEUnknownObjectType);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ SetAEProperty
// ---------------------------------------------------------------------------
//	Set a Property using data from a descriptor
//
//	Subclasses which have modifiable Properties must override this function

void
LModelObject::SetAEProperty(
	DescType		/* inProperty */,
	const AEDesc&	/* inValue */,
	AEDesc&			/* outAEReply */)
{
	ThrowOSErr_(errAEUnknownObjectType);
}


// ---------------------------------------------------------------------------
//	¥ AEPropertyExists
// ---------------------------------------------------------------------------
//	Subclasses should override to return true for properties supported
//	in overrides of GetAEProperty and SetAEProperty

bool
LModelObject::AEPropertyExists(
	DescType	inProperty) const
{
	bool	exists = false;

	switch (inProperty) {

		case pClass:
		case pContents:
			exists = true;
			break;

		default: {

				// Subclasses should override if they have properties.
				// In case they don't, we try to actually get the
				// property. If that succeeds, the property exists;
				// If not, the property doesn't exist.

			StDisableDebugThrow_();		// Ok to fail

			try {
				StAEDescriptor	propDesc;
				GetAEProperty(inProperty, UAppleEventsMgr::sAnyType, propDesc);
				exists = true;
			}

			catch (...) { }
			break;
		}
	}

	return exists;
}


// ---------------------------------------------------------------------------
//	¥ GetImportantAEProperties
// ---------------------------------------------------------------------------
//	Return a record containing all "important" Properties
//
//	"Important" includes things necessary for cloning.
//
//	Subclasses which have Properties should override this function like:
//
//	{
//		inherited::GetImportantAEProperties(outRecord);
//
//		{	//	font
//			StAEDescriptor	aProp;
//			GetAEProperty(pFont, typeDesc, aProp);
//			UAEDesc::AddKeyDesc(&outRecord, pFont, aProp);
//		}
//		...
//		{	//	size
//			StAEDescriptor	aProp;
//			GetAEProperty(pSize, typeDesc, aProp);
//			UAEDesc::AddKeyDesc(&outRecord, pSize, aProp);
//		}
//	}

void
LModelObject::GetImportantAEProperties(
	AERecord &outRecord) const
{
	StAEDescriptor		contents;
	StAEDescriptor		reqType;

	StDisableDebugThrow_();		// Ok to fail

	try {
		GetAEProperty(pContents, reqType, contents);
		UAEDesc::AddKeyDesc(&outRecord, pContents, contents);
	}

	catch (...) { }
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SendSelfAE
// ---------------------------------------------------------------------------
//	Send an AppleEvent to the current process with this ModelObject as
//	the direct parameter

void
LModelObject::SendSelfAE(
	AEEventClass	inEventClass,
	AEEventID		inEventID,
	Boolean			inExecute)
{
	try {
		AppleEvent	theAppleEvent;
		UAppleEventsMgr::MakeAppleEvent(inEventClass, inEventID, theAppleEvent);

		StAEDescriptor	modelSpec;
		MakeSpecifier(modelSpec);
		OSErr err = ::AEPutParamDesc(&theAppleEvent, keyDirectObject,
										modelSpec);
		ThrowIfOSErr_(err);

		UAppleEventsMgr::SendAppleEvent(theAppleEvent, inExecute);
	}

	catch (...) {					// Rethrow if event was to be executed
		if (inExecute) {			// If it wasn't executable, caller
			throw;					//    probably doesn't care if it failed
		}
	}
}


// ===========================================================================
// ¥ Static Member Functions						 Static Member Functions ¥
// ===========================================================================

// ---------------------------------------------------------------------------
//	DefaultModel
// ---------------------------------------------------------------------------
//
//	The DefaultModel responds to all AppleEvents not directed at any
//	specific object. It also represents the "null" container and the
//	top container in the AppleEvent Object Model container hierarchy.
//	In most cases, the DefaultModel will be the "Application" object.
//
//	If you do not use a PowerPlant class that automatically sets the
//	DefaultModel (such as one of the Application-type classes), you
//	must create a class that handles the DefaultModel's responsibilities
//	and set the DefaultModel appropriately.

LModelObject*
LModelObject::GetDefaultModel()
{
	return sDefaultModel;
}


void
LModelObject::SetDefaultModel(
	LModelObject	*inModel)
{
	sDefaultModel = inModel;
}


// ---------------------------------------------------------------------------
//	StreamingModel
// ---------------------------------------------------------------------------
//
//	The StreamingModel refers to the last LModelObject that was constructed
//	and that still exists.  It is useful when constructing LModelObject
//	hierarchies.
//
//
//	Implementation note:
//
//	The StreamingModel should not be a "claiming" shared object reference.
//	Doing such would mean LModelObjects couldn't go away as long as they were
//	the StreamingModel -- that would require extra & explicit code. So...
//	The sStreamingModel is a normal reference that is never left dangling
//	because ~LModelObject will SetStreamingModel(nil) as necessary.


LModelObject*
LModelObject::GetStreamingModel()
{
	return sStreamingModel;
}

void
LModelObject::SetStreamingModel(
	LModelObject	*inModel)
{
	sStreamingModel = inModel;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	TellTarget
// ---------------------------------------------------------------------------
//
//	The "TellTarget" (the default submodel for the Application) allows the
//	recording of more concise scripts.
//
//	Call DoAESwitchTellTarget when your application wishes to change the
//	"focus," or "TellTarget" of AppleEvent recording.  When calling
//	DoAESwitchTellTarget, any object specifiers constructed
//	with a previous TellTarget in place become "stale."  This means those
//	object specifiers shouldn't be used when sending or recording
//	subsequent AppleEvents.

void
LModelObject::DoAESwitchTellTarget(
	LModelObject	*inModelObject)
{
	if 	(inModelObject == GetTellTarget())
		return;	//	Don't record a non-effective change

	StAEDescriptor	appleEvent;
	UAppleEventsMgr::MakeAppleEvent(kAEPowerPlantSuite, kAESwitchTellTarget, appleEvent);

	if (inModelObject && (inModelObject != GetDefaultModel())) {
		StTempTellTarget	makeOSpecsBasedFrom(nil);

		StAEDescriptor	ospec;
		inModelObject->MakeSpecifier(ospec);
		UAEDesc::AddKeyDesc(appleEvent, keyAEData, ospec);
	}

	UAppleEventsMgr::SendAppleEvent(appleEvent);
}


LModelObject *
LModelObject::GetTellTarget(void)
{
	LModelObject	*defModel = GetDefaultModel();

	ThrowIfNil_(defModel);

	return defModel->GetDefaultSubModel();
}


void
LModelObject::SetTellTarget(LModelObject *inModel)
{
	LModelObject	*defModel = GetDefaultModel();

	ThrowIfNil_(defModel);

	defModel->SetDefaultSubModel(inModel);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ PutInToken
// ---------------------------------------------------------------------------
//	Place the pointer to a ModelObject within a Token. Tokens are used
//	when resolving an AppleEvent object specifier

void
LModelObject::PutInToken(
	LModelObject	*inModel,
	AEDesc			&outToken)
{
	if (inModel == nil) {
		ThrowOSErr_(errAENoSuchObject);
	}

	SModelToken		theToken;
	theToken.modelObject = inModel;

	//	AddPtr will automatically convert outToken to an AEList
	//	when necessary.
	UAEDesc::AddPtr(&outToken, 0, type_ModelToken, &theToken, sizeof(theToken));
}


// ---------------------------------------------------------------------------
//	¥ GetModelFromToken
// ---------------------------------------------------------------------------
//	Return the ModelObject represented by a Token Descriptor record

LModelObject*
LModelObject::GetModelFromToken(
	const AEDesc	&inToken)
{
	LModelObject*	theModel = nil;

	switch (inToken.descriptorType) {

		case typeNull:
			theModel = GetDefaultModel();
			break;

		case type_ModelToken: {
			#if ACCESSOR_CALLS_ARE_FUNCTIONS
			
				SModelToken	theToken;
				ThrowIfOSErr_( ::AEGetDescData(&inToken, &theToken, sizeof(SModelToken)));
				theModel = theToken.modelObject;
				
			#else
				theModel = (**((SModelTokenH) inToken.dataHandle)).modelObject;
			#endif
			
			break;
		}

		case typeAEList:
			SignalStringLiteral_("Can't get token from a list");
			ThrowOSErr_(errAENotASingleObject);
			break;

		default:
			SignalStringLiteral_("Unknown token type");
			ThrowOSErr_(errAEUnknownObjectType);
			break;
	}

	return theModel;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FinalizeLazies
// ---------------------------------------------------------------------------
//	Send a Finalize message to all objects in the lazy list.

void
LModelObject::FinalizeLazies()
{
	try {
		TArrayIterator<LModelObject*>	iterator(*sLazyModels, LArrayIterator::from_End);
		LModelObject	*model;

		while (iterator.Previous(model)) {

			try {
				model->Finalize();
			}

			catch (...) { }
		}

	}

	catch (...) { }
}


// ---------------------------------------------------------------------------
//	¥ AddLazy
// ---------------------------------------------------------------------------
//	Adds the object to the "lazy" list of objects that receive Finalize
//	messages in response to FinalizeLazies.
//
//	You should never need to call this function.  Use SetLaziness(true)
//	instead.

void
LModelObject::AddLazy(
	LModelObject	*inModel)
{
	sLazyModels->AddItem(inModel);
}


// ---------------------------------------------------------------------------
//	¥ RemoveLazy
// ---------------------------------------------------------------------------
//	Remove the object from the "lazy" list of objects.
//
//	You should never need to call this function.  Use SetLaziness(false)
//	instead.

void
LModelObject::RemoveLazy(
	LModelObject	*inModel)
{
	sLazyModels->Remove(inModel);
}

#pragma mark -

// ===========================================================================
//	StLazyLock
// ===========================================================================
//
//	StLazyLock is an "St" class that temporarily changes the "laziness" of a
//	ModelObject to false -- preventing a ModelObject from being deleted
//	during the scope of the StLazyLock.  This is similar to StSharer but,
//	when the StLazyLock is destroyed, the object will remain even if there
//	are no outstanding claims on the model.

StLazyLock::StLazyLock(
	LModelObject	*inModel)
{
	mModel = inModel;
	
	if (mModel != nil) {
		mLaziness = mModel->IsLazy();
		mModel->SetLaziness(false);
	}
}

StLazyLock::~StLazyLock()
{
	if (mModel != nil) {
		mModel->SetLaziness(mLaziness);
	}
}

#pragma mark -

// ===========================================================================
//	StTempTellTarget
// ===========================================================================
//
//	Sometimes it is useful to temporarily change the TellTarget (things like
//	recording a complete object specifier instead of a truncated object
//	specifier).  StTempTellTarget provides a exception safe mechanism for
//	doing this for a given scope.
//
//	Use an inModel of nil to correspond to no TellTarget.

StTempTellTarget::StTempTellTarget(
	LModelObject	*inModel)
{
	mOldTarget = LModelObject::GetTellTarget();
	
	if (mOldTarget != nil) {
		mOldTargetLaziness = mOldTarget->IsLazy();

		mOldTarget->SetLaziness(false);
	}

	LModelObject::SetTellTarget(inModel);
}


StTempTellTarget::~StTempTellTarget()
{
	LModelObject::SetTellTarget(mOldTarget);

	if (mOldTarget != nil) {
		mOldTarget->SetLaziness(mOldTargetLaziness);
	}
}

PP_End_Namespace_PowerPlant
