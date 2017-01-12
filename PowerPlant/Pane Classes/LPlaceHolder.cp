// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPlaceHolder.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A PlaceHolder temporarily holds an Occupant Pane. The Occupant Pane is
//	placed inside the PlaceHolder with a specified alignment, and later
//	restored to its original location.
//
//	Typically, you will use a PlaceHolder for printing. To print a Pane that
//	is inside a Window, you create a Printout that contains a PlaceHolder and
//	install the Pane in the PlaceHolder. After printing completes, the
//	Pane gets replaced back in its Window. This allows you to print a Pane
//	at a size and location that's different from how it exists in a Window.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LPlaceHolder.h>
#include <LHandleStream.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LPlaceHolder							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LPlaceHolder::LPlaceHolder()
{
	mOccupant			= nil;
	mOccupantSuperView	= nil;
	mOccupantPlaceH		= nil;
	mOccupantAlignment	= kAlignNone;
}


// ---------------------------------------------------------------------------
//	¥ LPlaceHolder							Copy Constructor		  [public]
// ---------------------------------------------------------------------------
//	Shallow copy; does not copy Occupant

LPlaceHolder::LPlaceHolder(
	const LPlaceHolder&		inOriginal)

	: LView(inOriginal)
{
	mOccupant			= nil;
	mOccupantSuperView	= nil;
	mOccupantPlaceH		= nil;
	mOccupantAlignment	= inOriginal.mOccupantAlignment;
}


// ---------------------------------------------------------------------------
//	¥ LPlaceHolder							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LPlaceHolder::LPlaceHolder(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	SInt16				inOccupantAlignment)

	: LView(inPaneInfo, inViewInfo)
{
	mOccupant			= nil;
	mOccupantSuperView	= nil;
	mOccupantPlaceH		= nil;
	mOccupantAlignment	= inOccupantAlignment;
}


// ---------------------------------------------------------------------------
//	¥ LPlaceHolder							Stream Constructor		  [public]
// ---------------------------------------------------------------------------
//	Construct PlaceHolder from data in a Stream

LPlaceHolder::LPlaceHolder(
	LStream*	inStream)

	: LView(inStream)
{
	*inStream >> mOccupantAlignment;

	mOccupant			= nil;
	mOccupantSuperView	= nil;
	mOccupantPlaceH		= nil;
}


// ---------------------------------------------------------------------------
//	¥ ~LPlaceHolder							Destructor				  [public]
// ---------------------------------------------------------------------------

LPlaceHolder::~LPlaceHolder()
{
	if (mOccupant != nil) {
		RemoveOccupant();
	}
}


// ---------------------------------------------------------------------------
//	¥ InstallOccupant												  [public]
// ---------------------------------------------------------------------------
//	Install a pane inside a PlaceHolder
//
//	Use the Toolbox IconAlignmentType values (in <Icons.h>) for inAlignment.
//	If you don't specify a horizontal alignment, the Occupant width is set
//	to the PlaceHolder width. Similarly, if you don't specify a vertical
//	alignment, the Occupant height is set to the PlaceHolder height.
//
//	The inAlignment paramter is optional. If not specified, the default
//	value (of -1) means to use the alignment stored in the PlaceHolder.

void
LPlaceHolder::InstallOccupant(
	LPane*	inOccupant,
	SInt16	inAlignment)
{
	if (mOccupant != nil) {			// Remove existing Occupant, if any
		RemoveOccupant();
	}

	mOccupant = inOccupant;
	if (inAlignment == -1) {
		inAlignment = mOccupantAlignment;
	}

	LHandleStream	placeStream;
	inOccupant->SavePlace(&placeStream);
	mOccupantPlaceH = placeStream.DetachDataHandle();

	mOccupantSuperView = inOccupant->GetSuperView();
	mOccupant->PutInside(this);

	SDimension16	mySize;
	GetFrameSize(mySize);

	SDimension16	occupantSize;
	inOccupant->GetFrameSize(occupantSize);

								// Find Horizontal location for Occupant
	SInt32		horizPos;
	Boolean		fitToWidth = false;
	if ((inAlignment & kAlignRight) == kAlignRight) {
		horizPos = mySize.width - occupantSize.width;

	} else if ((inAlignment & kAlignLeft) == kAlignLeft) {
		horizPos = 0;

	} else if ((inAlignment & kAlignHorizontalCenter) == kAlignHorizontalCenter) {
		horizPos = (mySize.width - occupantSize.width) / 2;

	} else {					// No Alignment, size Occupant to match
		horizPos = 0;			// width of PlaceHolder
		fitToWidth = true;
	}

								// Find Vertical location for Occupant
	SInt32		vertPos;
	Boolean		fitToHeight = false;
	if ((inAlignment & kAlignBottom) == kAlignBottom) {
		vertPos = mySize.height - occupantSize.height;

	} else if ((inAlignment & kAlignTop) == kAlignTop) {
		vertPos = 0;

	} else if ((inAlignment & kAlignVerticalCenter) == kAlignVerticalCenter) {
		vertPos = (mySize.height - occupantSize.height) / 2;

	} else {					// No Alignment, size Occupant to match
		vertPos = 0;			// height of PlaceHolder
		fitToHeight = true;
	}

								// Position Occupant within PlaceHolder
	inOccupant->PlaceInSuperImageAt(horizPos, vertPos, Refresh_No);

								// Resize Occupant to fit PlaceHolder
	if (fitToWidth || fitToHeight) {
		SInt16	occupantWidth = occupantSize.width;
		if (fitToWidth) {
			occupantWidth = mySize.width;
		}

		SInt16	occupantHeight = occupantSize.height;
		if (fitToHeight) {
			occupantHeight = mySize.height;
		}

		inOccupant->ResizeFrameTo(occupantWidth, occupantHeight, Refresh_No);
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveOccupant												  [public]
// ---------------------------------------------------------------------------
//	Remove Occupant from a PlaceHolder, restoring the Occupant to its
//	original state

LPane*
LPlaceHolder::RemoveOccupant()
{
	LPane*	theOccupant = mOccupant;

	if (theOccupant != nil) {
		theOccupant->PutInside(mOccupantSuperView, false);

		LHandleStream	placeStream(mOccupantPlaceH);
		theOccupant->RestorePlace(&placeStream);

		if (mOccupantSuperView != nil) {
			mOccupantSuperView->OrientSubPane(theOccupant);
		}

		mOccupant = nil;
		mOccupantPlaceH = nil;
	}

	return theOccupant;
}


// ---------------------------------------------------------------------------
//	¥ CountPanels													  [public]
// ---------------------------------------------------------------------------
//	Return the number of horizontal and vertical Panels. A Panel is a
//	"frameful" of a View's Image. For a PlaceHolder, the number of Panels
//	is the number of Panels in its Occupant Pane

void
LPlaceHolder::CountPanels(
	UInt32&		outHorizPanels,
	UInt32&		outVertPanels)
{
	outHorizPanels = outVertPanels = 1;
	if (mOccupant != nil) {
		mOccupant->CountPanels(outHorizPanels, outVertPanels);
	}
}


// ---------------------------------------------------------------------------
//	¥ ScrollToPanel													  [public]
// ---------------------------------------------------------------------------
//	Scroll View Image to the specified Panel. For a PlaceHolder, scroll
//	Occupant Pane to the specified Panel.

Boolean
LPlaceHolder::ScrollToPanel(
	const PanelSpec&	inPanel)
{
	Boolean	panelExists = false;
	if (mOccupant != nil) {
		panelExists = mOccupant->ScrollToPanel(inPanel);
	}
	return panelExists;
}


PP_End_Namespace_PowerPlant
