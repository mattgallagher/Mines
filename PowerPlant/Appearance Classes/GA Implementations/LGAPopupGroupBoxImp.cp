// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAPopupGroupBoxImp.cp		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGAPopupGroupBoxImp.h>

#include <LStream.h>
#include <LPopupButton.h>
#include <LPopupGroupBox.h>
#include <PP_Messages.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Constants

enum {
	groupBox_HeaderLeftOffset		=	9,
	groupBox_HeaderLineOffset		=	3,
	groupBox_DefaultHeaderPaneID	=	FOUR_CHAR_CODE('grph')
};


// ---------------------------------------------------------------------------
//	¥ LGAPopupGroupBoxImp					Constructor				  [public]
// ---------------------------------------------------------------------------

LGAPopupGroupBoxImp::LGAPopupGroupBoxImp(
	LControlPane*	inControlPane)

	: LGAGroupBoxImp(inControlPane)
{
}


// ---------------------------------------------------------------------------
//	¥ LGAPopupGroupBoxImp					Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGAPopupGroupBoxImp::LGAPopupGroupBoxImp(
	LStream*	inStream)

	: LGAGroupBoxImp(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LGAPopupGroupBoxImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LGAPopupGroupBoxImp::~LGAPopupGroupBoxImp()
{
}


// ---------------------------------------------------------------------------
//	¥ Init									Stream Initializer		  [public]
// ---------------------------------------------------------------------------

void
LGAPopupGroupBoxImp::Init(
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
//	¥ Init									Parameterized Initializer [public]
// ---------------------------------------------------------------------------

void
LGAPopupGroupBoxImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			/* inRefCon */)
{
	mControlPane	= inControlPane;
	mPrimaryBorder	= (inControlKind == kControlGroupBoxPopupButtonProc);

	BuildPopupHeader(	inTitle,
						mControlPane->GetValue(),				// Options
						(SInt16) mControlPane->GetMaxValue(),	// Label Width
						inTextTraitsID);
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf											   [protected]
// ---------------------------------------------------------------------------

void
LGAPopupGroupBoxImp::FinishCreateSelf()
{
		// Set the MenuHandle of our PopupButton header to
		// that of the PopupGroupBox

	LPopupGroupBox*	box = dynamic_cast<LPopupGroupBox*>
								(mControlPane->GetSuperView());

	LPopupButton*	popup = dynamic_cast<LPopupButton*>(mHeaderObject);

	if (box && popup) {
									// PopupButton does NOT own the Menu
		popup->SetMacMenuH(box->GetMacMenuH(), false);

		ResizeToBestRect(mBaseline, false);	// Fit border to size of header
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetAllValues													  [public]
// ---------------------------------------------------------------------------

void
LGAPopupGroupBoxImp::GetAllValues(
	SInt32&		outValue,
	SInt32&		outMinValue,
	SInt32&		outMaxValue)
{
	mHeaderObject->GetAllValues(outValue, outMinValue, outMaxValue);
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LGAPopupGroupBoxImp::SetValue(
	SInt32	inValue)
{
	mHeaderObject->SetValue(inValue);		// Set value of Popup Button
}


// ---------------------------------------------------------------------------
//	¥ PostSetValue													  [public]
// ---------------------------------------------------------------------------

void
LGAPopupGroupBoxImp::PostSetValue ()
{
	// Then we need to make sure that we poke the value into the control
	// pane so that when GetValue is called the correct value will be in
	// the control pane's value field, unfortunately we have to do this here
	// otherwise SetValue messes with the value because it is out of range
	// so we have to get the value from the header and use that to get the
	// value set directly

	mControlPane->PokeValue ( mHeaderObject->GetValue () );
}


// ---------------------------------------------------------------------------
//	¥ SetMinValue													  [public]
// ---------------------------------------------------------------------------


void
LGAPopupGroupBoxImp::SetMinValue(
	SInt32	inMinValue)
{
	mHeaderObject->SetMinValue(inMinValue);		// Set min of Popup Button
}



// ---------------------------------------------------------------------------
//	¥ SetMaxValue													  [public]
// ---------------------------------------------------------------------------


void
LGAPopupGroupBoxImp::SetMaxValue(
	SInt32	inMaxValue)
{
	mHeaderObject->SetMaxValue(inMaxValue);		// Set max of Popup Button
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ListenToMessage												  [public]
// ---------------------------------------------------------------------------

void
LGAPopupGroupBoxImp::ListenToMessage(
	MessageT	inMessage,
	void*		/* ioParam */)
{
	if (inMessage == msg_ControlClicked) {
										// Our Popup Menu has changed value
		 								//   Set value of our ControlPane to
		 								//   match
		SInt32	value = mHeaderObject->GetValue();
		mControlPane->PokeValue( value );

										// Since we're poking the value, we
										//   need to tell the ControlPane
										//   to broadcast its value message
		MessageT	theMessage = mControlPane->GetValueMessage();
		if (theMessage != msg_Nothing) {
			mControlPane->BroadcastMessage(theMessage, &value);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LGAPopupGroupBoxImp::SetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
	if (inTag == LPopupGroupBox::dataTag_MenuHandle) {
		mHeaderObject->SetDataTag(inPartCode, kControlPopupButtonMenuHandleTag,
							inDataSize, inDataPtr);
							
	} else {
		LGAGroupBoxImp::SetDataTag(inPartCode, inTag, inDataSize, inDataPtr);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ BuildPopupHeader											   [protected]
// ---------------------------------------------------------------------------

void
LGAPopupGroupBoxImp::BuildPopupHeader(
	ConstStringPtr	inLabel,
	SInt32			inTitleOptions,
	SInt16			inLabelWidth,
	ResIDT			inTextTraitID)
{
	SPaneInfo paneInfo;				// Set info for popup button

	paneInfo.paneID  = groupBox_DefaultHeaderPaneID;
	paneInfo.width   = 100;
	paneInfo.height  = 18;
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

									// Now create the actual button
	mHeaderObject = new LPopupButton ( 	paneInfo,
										msg_ControlClicked,
										(SInt16) inTitleOptions,
										MENU_Unspecified,
										inLabelWidth,
										kControlPopupButtonProc,
										inTextTraitID,
										inLabel,
										0,					// Resource type
										1);					// Initial item

	mHeaderObject->AddListener(this);		// We need to set the group box's value
											//   when the menu value changes
}


PP_End_Namespace_PowerPlant
