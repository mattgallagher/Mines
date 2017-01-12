// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LStdPopupButtonImp.cp		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LStdPopupButtonImp.h>
#include <LPopupButton.h>
#include <LStream.h>
#include <PP_Resources.h>
#include <UTBAccessors.h>

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//	StPopupMenuInserter
// ===========================================================================
//	Constructor inserts Menu into Toolbox MenuList
//	Destructor removes Menu from MenuList

class	StPopupMenuInserter {
public:
		StPopupMenuInserter(MenuHandle inMenuH);
		~StPopupMenuInserter();
private:
		MenuHandle	mMenuH;
};


StPopupMenuInserter::StPopupMenuInserter(
	MenuHandle	inMenuH)
{
	mMenuH = inMenuH;

	if (inMenuH != nil) {
		::MacInsertMenu(inMenuH, hierMenu);
	}
}

StPopupMenuInserter::~StPopupMenuInserter()
{
	if (mMenuH != nil) {
		::MacDeleteMenu( ::GetMenuID(mMenuH) );
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LStdPopupButtonImp				Stream/Default Constructor	  [public]
// ---------------------------------------------------------------------------
//	Do not access inStream. It may be nil. Read Stream data in Init()

LStdPopupButtonImp::LStdPopupButtonImp(
	LStream*	inStream)

	: LStdControlImp(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ Init									Stream Initializer		  [public]
// ---------------------------------------------------------------------------

void
LStdPopupButtonImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream)
{
	SInt16	controlKind;				// Read data from Stream
	ResIDT	textTraitsID;
	Str255	title;
	SInt32	refCon;

	*inStream >> controlKind;
	*inStream >> textTraitsID;
	inStream->ReadPString(title);
	*inStream >> refCon;
										// Call parameterized intializer
	Init(inControlPane, controlKind, title, textTraitsID, refCon);
}


// ---------------------------------------------------------------------------
//	¥ Init									Parameterized Initializer [public]
// ---------------------------------------------------------------------------

void
LStdPopupButtonImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			inRefCon)
{
	ResIDT	menuID = (ResIDT) inControlPane->GetMinValue();
	if (menuID == MENU_Unspecified) {
		menuID = MENU_Empty;
	}
	::MacGetMenu(menuID);

	LStdControlImp::Init(inControlPane, inControlKind, inTitle,
							inTextTraitsID, inRefCon);

	::MacDeleteMenu(menuID);
}


// ---------------------------------------------------------------------------
//	¥ ~LStdPopupButtonImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LStdPopupButtonImp::~LStdPopupButtonImp()
{
}


// ---------------------------------------------------------------------------
//	¥ FindHotSpot													  [public]
// ---------------------------------------------------------------------------

SInt16
LStdPopupButtonImp::FindHotSpot(
	Point		/* inPoint */) const
{
	return kControlMenuPart;
}


// ---------------------------------------------------------------------------
//	¥ TrackHotSpot													  [public]
// ---------------------------------------------------------------------------

Boolean
LStdPopupButtonImp::TrackHotSpot(
	SInt16		inHotSpot,
	Point		inPoint,
	SInt16		inModifiers)
{
	StPopupMenuInserter	insert(GetMacMenuH());

	return LStdControlImp::TrackHotSpot(inHotSpot, inPoint, inModifiers);
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LStdPopupButtonImp::DrawSelf()
{
	StPopupMenuInserter	insert(GetMacMenuH());

	LStdControlImp::DrawSelf();
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LStdPopupButtonImp::SetValue(
	SInt32		inValue)
{
	StPopupMenuInserter	insert(GetMacMenuH());

	LStdControlImp::SetValue(inValue);
}


// ---------------------------------------------------------------------------
//	¥ GetMacMenuH												   [protected]
// ---------------------------------------------------------------------------

MenuHandle
LStdPopupButtonImp::GetMacMenuH() const
{
	MenuHandle	menuH = nil;

	LPopupButton	*popup = dynamic_cast<LPopupButton*>(mControlPane);
	if (popup) {
		menuH = popup->GetMacMenuH();
	}

	return menuH;
}


PP_End_Namespace_PowerPlant
