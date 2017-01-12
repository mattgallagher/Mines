// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UResourceMgr.cp				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Stack-based classes for manipulating resources

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UResourceMgr.h>
#include <LString.h>

#include <LowMem.h>

PP_Begin_Namespace_PowerPlant


// ===========================================================================
//	¥ StNewResource Class
// ===========================================================================
//	Constructor gets the resource handle (if it existed)
//		Caller updates the resource handle, then...
//	Destructor writes it to resource fork and releases the resource handle

StNewResource::StNewResource(
	ResType	inResType,
	ResIDT	inResID,
	Size	inDefaultSize,			// Default size if resource created
	Boolean	inGet1Resource)			// True to fetch resource from current fork only

	: StResource(inResType, inResID, false, inGet1Resource)
{
	mResName[0]	 = 0;
	mResType	 = inResType;
	mResID		 = inResID;
	mResAttrs	 = 0;
	mResAttrsSet = mResNameSet = mResourceExisted = false;
	mNeedToWrite = true;

	if (mResourceH) {
		mResourceExisted = true;
		if (::GetHandleSize(mResourceH) < inDefaultSize) {
			::SetHandleSize(mResourceH, inDefaultSize);
			ThrowIfMemError_();
		}
	} else {
		mResourceH = ::NewHandle(inDefaultSize);
		ThrowIfNil_(mResourceH);
	}
}


StNewResource::~StNewResource()
{
	try {
		if (mNeedToWrite) {
			Write();
		}
	}

	catch (...) { }
}


void StNewResource::SetResName(
	ConstStringPtr	inNewName)			// new name for resource
{
	LString::CopyPStr(inNewName, mResName);
	mResNameSet = true;
}


void StNewResource::SetResAttrs(
	SInt16 inResAttrs)					// set resource attributes
{
	mResAttrs = inResAttrs;
	mResAttrsSet = true;
}


void StNewResource::DontWrite()
{
	mNeedToWrite = false;
}


// ---------------------------------------------------------------------------
// StNewResource::Write
// ---------------------------------------------------------------------------
// Write the resource to disk.
// Use if you need to catch error notifications.


void StNewResource::Write(
	Boolean inUpdateResFile)			// True to force a call to UpdateResFile
{
	// stomp out future write requests (from destructor)

	mNeedToWrite = false;

	// sanity check: make sure we have a resource

	ThrowIfNil_(mResourceH);

	// if the resource was there, update it; else add it

	if (mResourceExisted) {
		if (::GetResAttrs(mResourceH) & resProtected) {
			::SetResAttrs(mResourceH, 0);
			mResAttrsSet = true;
		}

		::ChangedResource(mResourceH);
		if (mResNameSet) {

			// change name:
			//	SetResInfo is a slow operation, so we want to bypass it
			//	unless the given name actually is different

			Str255 tempName;
			SInt16 foo;
			ResType foo1;
			::GetResInfo(mResourceH, &foo, &foo1, tempName);
			if (!::EqualString(tempName, mResName, true, true))
				::SetResInfo(mResourceH, mResID, mResName);
		}

	} else {
		::AddResource(mResourceH, mResType, mResID, mResName);
		mResourceExisted = true;		// Resource now exists
	}

	ThrowIfResError_();


	// make sure it gets into the file... must be done before SetResAttrs
	// in case the resource is marked protected or purgeable

	::WriteResource(mResourceH);
	ThrowIfResError_();

	// now set the attributes

	if (mResAttrsSet) {
		::SetResAttrs(mResourceH, mResAttrs);
		ThrowIfResError_();
	}

	// update the resource map

	if (inUpdateResFile) {
		::UpdateResFile(::CurResFile());
		ThrowIfResError_();
	}
}

#pragma mark -

// ===========================================================================
//	¥ StDeleteResource Class
// ===========================================================================
//	Constructor gets the resource handle
//		Caller updates the resource handle, then...
//	Destructor removes the resource from file and releases the resource handle

StDeleteResource::StDeleteResource(
	ResType	inResType,
	ResIDT	inResID,
	Boolean	inThrowFail,
	Boolean	inGet1Resource)

	: StResource(inResType, inResID, inThrowFail, inGet1Resource)
{
}


StDeleteResource::~StDeleteResource()
{
	if (mResourceH != nil) {
		::RemoveResource(mResourceH);
		::DisposeHandle(mResourceH);
		mResourceH = nil;
	}
}

#pragma mark -

// ===========================================================================
//	¥ StResLoad Class
// ===========================================================================
//	Constructor gets value of ResLoad, then changes it to its parameter
//		(usually off)
//	Destructor restores ResLoad

StResLoad::StResLoad(
	Boolean inResLoad)
{
	mOriginalResLoad = ::LMGetResLoad();
	::SetResLoad(inResLoad);
}


StResLoad::~StResLoad()
{
	::SetResLoad(mOriginalResLoad);
}

#pragma mark -

// ===========================================================================
//	¥ StCurResFile Class
// ===========================================================================
//	Constructor saves reference number of current resource file
//	Destructor restores it

StCurResFile::StCurResFile(
	SInt16		inNewResFile)
{
	mSaveResFile = ::CurResFile();

	if (inNewResFile != -1) {		// -1 is always an invalid refNum
		::UseResFile(inNewResFile);
	}
}


StCurResFile::~StCurResFile()
{
	::UseResFile(mSaveResFile);
}

#pragma mark -
// ===========================================================================
//	UResourceContext Class
// ===========================================================================
//	Constructor changes the current resource file (via UseResFile)
//	Destructor restores the current resource file
//
//	An assumption is made for obtaining the application context (sAppContext
//	is initialized with CurResFile). Due to this you must ensure that this
//	assumption will remain valid (e.g. under 68K place it in CODE Segment 1).


SInt16				StResourceContext::sAppContext		= ::CurResFile();
StResourceContext*	StResourceContext::sCurrentContext	= nil;


// ---------------------------------------------------------------------------
//	¥ StResourceContext												  [public]
// ---------------------------------------------------------------------------
//	Constructor, specifying the resource context to enter by using a refnum.
//	Destructor will revert to the existing resource context.

StResourceContext::StResourceContext(
	SInt16 inContext)
{
	mThisContext	= inContext;
	mIsValid		= (mThisContext != ResourceContext_NoFile);
	mInContext		= false;
	mSavedContext	= false;
	mSaveContext	= nil;

	Enter();
}


// ---------------------------------------------------------------------------
//	¥ ~StResourceContext											  [public]
// ---------------------------------------------------------------------------
//	Make sure that CurResFile is returned to its initial state.

StResourceContext::~StResourceContext()
{
	Exit();
}


// ---------------------------------------------------------------------------
//	¥ Enter															  [public]
// ---------------------------------------------------------------------------
//	Enter this resource file's context.

void
StResourceContext::Enter()
{
		// Update current context pointer.
	mSaveContext	= sCurrentContext;
	sCurrentContext	= this;
	mInContext		= true;
	mPrevContext	= ::CurResFile();

		// Set new context (if necessary).
	if (IsValid()) {
		::UseResFile(mThisContext);
	}
}


// ---------------------------------------------------------------------------
//	¥ Exit															  [public]
// ---------------------------------------------------------------------------
//	Leave this resource file's context & go back to whatever we had before.

void
StResourceContext::Exit()
{
		// Update current context pointer.
	if (InContext() && (mSaveContext)) {
		sCurrentContext = mSaveContext;
		mSaveContext = nil;
	}

		// If we switched the resource context, switch it back.
	if (mSavedContext || (IsValid() && InContext())) {
		::UseResFile(mPrevContext);
	}

		// Now remember that we left context.
	mInContext = false;

}


// ---------------------------------------------------------------------------
//	¥ Save															  [public]
// ---------------------------------------------------------------------------
//	Save the current resource context so when we Exit()
//	the current context is restored.

void
StResourceContext::Save()
{
		// Make sure we don't already have a context sitting around.
	Exit();

		// Now save the current context.
	mPrevContext	= ::CurResFile();
	mThisContext	= ResourceContext_NoFile;
	mSavedContext	= true;
	mIsValid		= false;

}


// ---------------------------------------------------------------------------
//	¥ CurrentContextIsValid									  [static, public]
// ---------------------------------------------------------------------------

bool
StResourceContext::CurrentContextIsValid()
{
	if (!sCurrentContext) {
		return false;
	}

	return sCurrentContext->IsValid();
}


// ---------------------------------------------------------------------------
//	¥ GetCurrentRefNum										  [static, public]
// ---------------------------------------------------------------------------
//	Return the refnum for the current resource file context.

SInt16
StResourceContext::GetCurrentRefNum()
{
	if (!sCurrentContext) {
		return 0;
	}

	return sCurrentContext->mThisContext;
}


PP_End_Namespace_PowerPlant
