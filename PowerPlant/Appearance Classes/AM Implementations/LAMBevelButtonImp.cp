// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMBevelButtonImp.cp		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LAMBevelButtonImp.h>
#include <LBevelButton.h>
#include <PP_Resources.h>
#include <UTBAccessors.h>

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//	StBevelMenuSetter
// ===========================================================================
//	Constructor sets MenuHandle for underlying Control
//	Destructor clears MenuHandle

class StBevelMenuSetter {
public:
		StBevelMenuSetter(
			ControlHandle	inControlH,
			MenuHandle		inMenuH,
			SInt16			inMenuValue);
		~StBevelMenuSetter();

protected:
	ControlHandle	mControlH;
	MenuHandle		mMenuH;
};


StBevelMenuSetter::StBevelMenuSetter(
	ControlHandle	inControlH,
	MenuHandle		inMenuH,
	SInt16			inMenuValue)
{
	mMenuH    = inMenuH;
	mControlH = inControlH;

	if (inMenuH != nil) {
		::SetControlData(inControlH, 0, kControlBevelButtonMenuHandleTag,
					sizeof(MenuHandle), (Ptr) &inMenuH);

		::SetControlData(inControlH, 0, kControlBevelButtonMenuValueTag,
					sizeof(SInt16), (Ptr) &inMenuValue);
	}
}

StBevelMenuSetter::~StBevelMenuSetter()
{
	if (mMenuH != nil) {

			// Install dummmy menu with no items

		MenuHandle	emptyMenuH = LMenuController::GetEmptyMenuH();

		::SetControlData(mControlH, 0, kControlBevelButtonMenuHandleTag,
					sizeof(MenuHandle), (Ptr) &emptyMenuH);

			// Some versions of the OS do not automatically uninstall
			// the existing popup MenuHandle from the global menu list.
			// Unfortunately, MacDeleteMenu() takes a MenuID, which is
			// not necessarily unique (multiple menus can have the same
			// MenuID). So, check if our MenuHandle is installed before
			// removing the menu with its ID.
			
		MenuID		theID	= ::GetMenuID(mMenuH);
		MenuHandle	theMenu = ::GetMenuHandle(theID);
		
		if (theMenu == mMenuH) {
			::MacDeleteMenu(theID);
		}
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LAMBevelButtonImp					Stream/Default Constructor	  [public]
// ---------------------------------------------------------------------------
//	Do not access inStream. It may be nil. Read Stream data in Init()

LAMBevelButtonImp::LAMBevelButtonImp(
	LStream*	inStream)

	: LAMControlImp(inStream)
{
		// This class requires the special empty MENU resource in order
		// to prevent the Toolbox from mucking with the real MenuHandle.
		// Signal and throw if we can't find this menu.

	MenuHandle	emptyMenuH = LMenuController::GetEmptyMenuH();

	if (emptyMenuH == nil) {
		SignalStringLiteral_("LAMBevelButtonImp requires MENU 12345. "
							 "Add EmptyMenu.rsrc to your project.");
		Throw_(resNotFound);
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LAMBevelButtonImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LAMBevelButtonImp::~LAMBevelButtonImp()
{
	if (mPictHandle.IsValid()) {
	
			// Destructor for mPictHandle will dispose it. However, the
			// Toolbox control hasn't yet been deleted. To avoid leaving
			// the control with a dangling Handle, we set the content of
			// the control to "no content".
	
		ControlButtonContentInfo	info;
		info.contentType = kControlNoContent;
		
		SetDataTag(kControlButtonPart, kControlBevelButtonContentTag,
					sizeof(ControlButtonContentInfo), &info);
	}
}


// ---------------------------------------------------------------------------
//	¥ Init									Parameterized Initializer [public]
// ---------------------------------------------------------------------------

void
LAMBevelButtonImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			inRefCon)
{
		// [Workaround] Appearance Manager feature
		//
		// With PICT resource content, a bevel button releases the
		// resource when disposed. This is bad if there are multiple
		// buttons sharing the same PICT resource. As a workaround,
		// we change from PICT resource content to PICT Handle content,
		// getting and detaching the PICT resource.

	bool	usePictHandle = false;
	ResIDT	pictID;
									// Lo byte of min value stores the
									//   content type
	if ((UInt8) inControlPane->GetMinValue() == kControlContentPictRes) {
	
									// Max value stores the resource ID,
									//   which we save for later
		pictID = (ResIDT) inControlPane->GetMaxValue();
		
									// Content type is in the low byte of
									//   the min value. Change that to
									//   Handle-based picture without
									//   changing the high byte.
		inControlPane->PokeMinValue(
							(inControlPane->GetMinValue() & 0xFF00) |
							kControlContentPictHandle );		
		usePictHandle = true;
	}

									// Inherited function will create button
	LAMControlImp::Init(inControlPane, inControlKind, inTitle,
							inTextTraitsID, inRefCon);
							
	if (usePictHandle) {			// Set button content to the PICT resource
									//   by calling our SetDataTag function,
									//   which implements the workaround
		ControlButtonContentInfo	info;
		info.contentType = kControlContentPictRes;
		info.u.resID	 = pictID;
		
		SetDataTag(kControlButtonPart, kControlBevelButtonContentTag,
					sizeof(ControlButtonContentInfo), &info);
	}	
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LAMBevelButtonImp::SetValue(
	SInt32		inValue)
{
	StBevelMenuSetter	setMenu(mMacControlH, GetMacMenuH(), GetMenuValue());

	LAMControlImp::SetValue(inValue);
}


// ---------------------------------------------------------------------------
//	¥ SetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LAMBevelButtonImp::SetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
	if (inTag == kControlBevelButtonContentTag) {

		// [Workaround] Appearance Manager feature
		//
		// With PICT resource content, a bevel button releases the
		// resource when disposed. This is bad if there are multiple
		// buttons sharing the same PICT resource. As a workaround,
		// we change from PICT resource content to PICT Handle content,
		// getting and detaching the PICT resource.

									// Copy info record because we may
									//   need to change it
		ControlButtonContentInfo	info = 
				* static_cast<ControlButtonContentInfo*>(inDataPtr);
				
		StHandleBlock	newPicture;
					
		if (info.contentType == kControlContentPictRes) {
			 						// Changing content to a PICT resource
			 							
			 						// Get and detach PICT resource
			PicHandle	pictH = ::GetPicture(info.u.resID);
			ThrowIfNil_(pictH);
			
			::DetachResource((Handle) pictH);
			
			newPicture.Adopt((Handle) pictH);
			
									// Change content type to Picture Handle
			info.contentType = kControlContentPictHandle;
			info.u.picture	 = pictH;
		}
									// Install new content
		LAMControlImp::SetDataTag(inPartCode, inTag, sizeof(info), &info);
		
		if (inTag == kControlBevelButtonContentTag) {
									// Dispose stored Picture whenever
									//   content changes
			mPictHandle.Dispose();
									// Store new Picture content (will be
									//   nil if content isn't a Pict resource)
			mPictHandle.Adopt(newPicture.Release());
		}
		
	} else {						// Not changing content.
									// Call inherited function.
		LAMControlImp::SetDataTag(inPartCode, inTag, inDataSize, inDataPtr);
	}
}


// ---------------------------------------------------------------------------
//	¥ TrackHotSpot													  [public]
// ---------------------------------------------------------------------------

Boolean
LAMBevelButtonImp::TrackHotSpot(
	SInt16		inHotSpot,
	Point		inPoint,
	SInt16		inModifiers)
{
	StBevelMenuSetter	setMenu(mMacControlH, GetMacMenuH(), GetMenuValue());

	return LAMControlImp::TrackHotSpot(inHotSpot, inPoint, inModifiers);
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LAMBevelButtonImp::DrawSelf()
{
	StBevelMenuSetter	setMenu(mMacControlH, GetMacMenuH(), GetMenuValue());

	LAMControlImp::DrawSelf();
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetMacMenuH												   [protected]
// ---------------------------------------------------------------------------

MenuHandle
LAMBevelButtonImp::GetMacMenuH() const
{
	MenuHandle		menuH = nil;
	LBevelButton*	button = dynamic_cast<LBevelButton*>(mControlPane);

	if (button != nil) {
		menuH = button->GetMacMenuH();
	}

	return menuH;
}


// ---------------------------------------------------------------------------
//	¥ GetMenuValue												   [protected]
// ---------------------------------------------------------------------------

SInt16
LAMBevelButtonImp::GetMenuValue() const
{
	SInt16			menuValue = 0;
	LBevelButton*	button = dynamic_cast<LBevelButton*>(mControlPane);

	if (button != nil) {
		menuValue = button->GetCurrentMenuItem();
	}

	return menuValue;
}


PP_End_Namespace_PowerPlant
