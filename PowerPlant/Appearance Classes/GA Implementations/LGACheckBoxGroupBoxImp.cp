// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGACheckBoxGroupBoxImp.cp	PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGACheckBoxGroupBoxImp.h>
#include <UGraphicUtils.h>

#include <LControlImp.h>
#include <LControlView.h>
#include <LView.h>
#include <LStream.h>
#include <LString.h>
#include <UDrawingState.h>
#include <UTextTraits.h>
#include <PP_Messages.h>
#include <LCheckBox.h>

PP_Begin_Namespace_PowerPlant


enum {
	groupBox_HeaderLeftOffset		=	9,
	groupBox_HeaderLineOffset		=	3,
	groupBox_DefaultHeaderPaneID	=	FOUR_CHAR_CODE('grph')
};


// ---------------------------------------------------------------------------
//	¥ LGACheckBoxGroupBoxImp				Constructor				  [public]
// ---------------------------------------------------------------------------

LGACheckBoxGroupBoxImp::LGACheckBoxGroupBoxImp(
	LControlPane*	inControlPane)

	: LGAGroupBoxImp(inControlPane)
{
}


// ---------------------------------------------------------------------------
//	¥ LGACheckBoxGroupBoxImp				Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGACheckBoxGroupBoxImp::LGACheckBoxGroupBoxImp(
	LStream* 	inStream)

	: LGAGroupBoxImp(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LGACheckBoxGroupBoxImp				Destructor				  [public]
// ---------------------------------------------------------------------------

LGACheckBoxGroupBoxImp::~LGACheckBoxGroupBoxImp ()
{
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxGroupBoxImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream)
{
	SInt16	controlKind;
	Str255	title;
	ResIDT	textTraitID;

	*inStream >> controlKind;
	*inStream >> textTraitID;
	inStream->ReadPString(title);

	Init(inControlPane, controlKind, title, textTraitID);
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxGroupBoxImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			/* inRefCon */)
{
	mControlPane   = inControlPane;
	mPrimaryBorder = (inControlKind == kControlGroupBoxCheckBoxProc);

	BuildCheckBoxHeader(inTitle, inTextTraitsID,
							(mControlPane->GetValue() > 0));

}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxGroupBoxImp::SetValue(
	SInt32		inValue)
{
											//	We need to pass the value on to the header object
											// so that it can get itself setup correctly
	if ( HasHeader () && mHeaderObject->GetValue () != inValue ) {
		mHeaderObject->SetValue ( inValue );
	}
}


// ---------------------------------------------------------------------------
//	¥ PostSetValue													  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxGroupBoxImp::PostSetValue()
{
											//	We need to make sure that we reenable the header as
											// it will get disabled whenever the checkbox is turned
											// off
	if ( HasHeader() && !mHeaderObject->IsEnabled()) {
		mHeaderObject->Enable();
	}
											//	Then we need to make sure that we poke the value
											// into the control pane so that when GetValue is
											// called the correct value will be in the control
											// pane's value field, unfortunately we have to do
											// this here otherwise SetValue messes with the value
											// because it is out of range so we have to get the
											// value from the header and use that to get the value
											// set directly
	mControlPane->PokeValue ( mHeaderObject->GetValue ());

}


// ---------------------------------------------------------------------------
//	¥ FindHotSpot													  [public]
// ---------------------------------------------------------------------------

SInt16
LGACheckBoxGroupBoxImp::FindHotSpot(
	Point	/* inPoint */) const
{
		// Group Box has no hot spots. Its CheckBox is a separate
		// Pane that will track the mouse itself.

	return 0;
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ListenToMessage												  [public]
// ---------------------------------------------------------------------------

void
LGACheckBoxGroupBoxImp::ListenToMessage(
	MessageT	inMessage,
	void*		/* ioParam */)
{
											//	If we get a control clicked message we make sure
											// we call PostSetValue so that the header can be
											// enabled if needed
	if ( inMessage == msg_ControlClicked ) {
		LControlView*	super = (LControlView*)mControlPane->GetSuperView ();
		super->SetValue(mHeaderObject->GetValue());
		PostSetValue ();
	}
}


#pragma mark -
#pragma mark === HEADER CONSTRUCTION

// ---------------------------------------------------------------------------
//	¥ BuildCheckBoxHeader										   [protected]
// ---------------------------------------------------------------------------

void
LGACheckBoxGroupBoxImp::BuildCheckBoxHeader(
	ConstStringPtr	inLabel,
	ResIDT			inTextTraitID,
	Boolean			inChecked)
{
	SPaneInfo paneInfo;						//	Set info for CheckBox

	paneInfo.paneID  = PaneIDT_Unspecified;
	paneInfo.width   = 50;
	paneInfo.height  = 12;
	paneInfo.visible = true;
	paneInfo.enabled = true;
	paneInfo.bindings.left   = true;
	paneInfo.bindings.top    = true;
	paneInfo.bindings.right  = false;
	paneInfo.bindings.bottom = false;
	paneInfo.left = groupBox_HeaderLeftOffset + groupBox_HeaderLineOffset;
	paneInfo.top  = 0;
	paneInfo.userCon = 0;
	paneInfo.superView = mControlPane->GetSuperView();

											//	Now create the CheckBox object
	mHeaderObject = new LCheckBox(	paneInfo,							//	Pane info
									msg_ControlClicked,					//	Message
									inChecked ? Button_On : Button_Off, //  Value
									inLabel,							//	Label
									inTextTraitID );					//	Text trait ID

											// We also add the group box as a listener to the
											// header
	mHeaderObject->AddListener(this);
											//	Now we need to get the caption resized to fit the
											// header
	ResizeToBestRect(mBaseline, false);
}


PP_End_Namespace_PowerPlant
