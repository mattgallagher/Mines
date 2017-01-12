// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UQuickTime.cp				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UQuickTime.h>
#include <UEnvironment.h>
#include <UStandardDialogs.h>

#include <Gestalt.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ QuickTimeIsPresent									 [static] [public]
// ---------------------------------------------------------------------------
//	Is QuickTime present?  Initialize must have been called prior to this.

bool
UQuickTime::QuickTimeIsPresent()
{
	return UEnvironment::HasFeature(env_HasQuickTime);
}


// ---------------------------------------------------------------------------
//	¥ Initialize											 [static] [public]
// ---------------------------------------------------------------------------
//	Initialize the Movie Toolbox if QuickTime is present

void
UQuickTime::Initialize()
{
	SInt32	qtVersion;
	Boolean	qtPresent = (::Gestalt(gestaltQuickTime, &qtVersion) == noErr)
							&& CFM_AddressIsResolved_(::EnterMovies);

	if (qtPresent) {
		UEnvironment::SetFeature(env_HasQuickTime, true);
		::EnterMovies();
	}
}


// ---------------------------------------------------------------------------
//	¥ Finalize												 [static] [public]
// ---------------------------------------------------------------------------
//	Perform any necessary cleanup

void
UQuickTime::Finalize()
{
	if (QuickTimeIsPresent()) {
		::ExitMovies();
	}
}


// ---------------------------------------------------------------------------
//	¥ GetMovieFromFile										 [static] [public]
// ---------------------------------------------------------------------------
//	Presents a dialog to select a movie. If successful, load the movie file
//	into memory and returns the Movie reference ready for manipulation.

Movie
UQuickTime::GetMovieFromFile()
{
	if (!QuickTimeIsPresent()) {
		SignalStringLiteral_("QuickTime is not present on this machine");
		return nil;
	}

	Movie	theMovie = nil;

	PP_StandardDialogs::LFileChooser	chooser;

	NavDialogOptions	*options = chooser.GetDialogOptions();
	if (options != nil) {
		options->dialogOptionFlags =	kNavDefaultNavDlogOptions	+
										kNavSelectAllReadableItem;
	}

	FSSpec	movieSpec;
	if (chooser.AskChooseOneFile(LFileTypeList((OSType)MovieFileType), movieSpec)) {
		OSErr	err;
		SInt16	movieRefNum;
		err = ::OpenMovieFile(&movieSpec, &movieRefNum, fsRdPerm);
		ThrowIfOSErr_(err);

		SInt16	actualResID = DoTheRightThing;
		Boolean	wasChanged;
		err = ::NewMovieFromFile(&theMovie, movieRefNum, &actualResID,
									nil, newMovieActive, &wasChanged);
		ThrowIfOSErr_(err);

		err = ::CloseMovieFile(movieRefNum);
		ThrowIfOSErr_(err);
	}

	return theMovie;
}


// ---------------------------------------------------------------------------
//	¥ SaveFlattenedMovie									 [static] [public]
// ---------------------------------------------------------------------------

void
UQuickTime::SaveFlattenedMovie(
	Movie	/* inMovie */)
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LMovieController						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LMovieController::LMovieController()
{
	mMovie = nil;
	mMovieController = nil;
}


// ---------------------------------------------------------------------------
//	¥ LMovieController						Constructor				  [public]
// ---------------------------------------------------------------------------

LMovieController::LMovieController(
	const SPaneInfo	&inPaneInfo,
	Movie			inMovie)

	: LPane(inPaneInfo)
{
	mMovie = inMovie;
	Rect	movieBounds;
	::GetMovieBox(inMovie, &movieBounds);
	::MacOffsetRect(&movieBounds, (SInt16) -movieBounds.left, (SInt16) -movieBounds.top);
	::SetMovieBox(inMovie, &movieBounds);

	::SetMovieGWorld(inMovie, (CGrafPtr) GetMacPort(), nil);

	Rect	frame;
	CalcLocalFrameRect(frame);
	mMovieController = ::NewMovieController(inMovie, &frame, mcTopLeftMovie);

	::MCSetControllerBoundsRect(mMovieController, &frame);
	::MCMovieChanged(mMovieController, mMovie);

	StartRepeating();
}


// ---------------------------------------------------------------------------
//	¥ ~LMovieController						Destructor				  [public]
// ---------------------------------------------------------------------------

LMovieController::~LMovieController()
{
	::DisposeMovieController(mMovieController);
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf													   [protected]
// ---------------------------------------------------------------------------
//	Draw the movie controller

void
LMovieController::DrawSelf()
{
	::MCDraw(mMovieController, GetMacWindow());
}


// ---------------------------------------------------------------------------
//	¥ SpendTime														  [public]
// ---------------------------------------------------------------------------
//	Handle events in the movie controller.

void
LMovieController::SpendTime(
	const EventRecord	&inMacEvent)
{
	FocusDraw();
	::MCIsPlayerEvent(mMovieController, &inMacEvent);
}


PP_End_Namespace_PowerPlant
