// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UGWorld.cp					PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	Two classes for using Offscreen GWorlds:
//		StOffscreenGWorld
//		LGWorld

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UGWorld.h>
#include <UAppearance.h>
#include <UDrawingState.h>
#include <UTBAccessors.h>

PP_Begin_Namespace_PowerPlant


// ===========================================================================
//	¥ StOffscreenGWorld Class
// ===========================================================================
//	Use this class to create temporary GWorlds for offscreen drawing
//
//	Constructor creates an offscreen GWorld and prepares it for drawing
//	Destructor copies offscreen image and destroys offscreen GWorld
//
//
//	Example with PowerPlant Panes:
//
//	void MyPane::DrawSelf()
//	{
//		Rect	frame;
//		CalcLocalFrameRect(frame);
//
//			// Allocate offscreen GWorld where subsequent drawing
//			// operations will take place
//
//		StOffscreenGWorld	offWorld(frame);
//
//		PaintRect(&frame);			// With offscreen drawing, we won't
//		MacInsetRect(&frame, 8, 8);	// see the flash when the white oval
//		EraseOval(&frame);			// gets drawn over the black rectangle
//
//			// offWorld goes out of scope at end of function
//			// It's Destructor gets called, which copies the
//			// offscreen image into the Pane and then destroys
//			// the offscreen GWorld
//	}
//
//	Please note that nothing in the StOffscreenGWorld class depends on the
//	PowerPlant Pane system. The only thing you need to do is to properly
//	set the current port before declaring a StOffscrenGWorld object.


// ---------------------------------------------------------------------------
//	¥ StOffscreenGWorld						Constructor				  [public]
// ---------------------------------------------------------------------------
//	Upon entry, the current port must to set to where you want to copy
//	the offscreen image when the StOffscreenGWorld object is destroyed
//
//	The inPixelDepth, inFlags, inCTableH, inGDeviceH, inBackColor, and
//	inXferMode parameters all have default values (see the header file), so
//	you don't need to specify them.
//
//	However, in some cases, you may want to specify an absolute
//	pixel depth. It may also be useful to set the useTempMem flag
//	so that the offscreen pixels are allocated in temporary memory.
//	This is appropriate because StOffscreenGWorlds are meant to be
//	temporary, stack-based, objects.
//
//	If you specify an inBackColor, it becomes the background color of the
//	offscreen world and all pixels are initialized to that color.
//
//	If inBackColor is nil, then the offscreen world uses the ThemeDrawingState
//	of the current port. If the Appearance Manager version is less than 1.1,
//	the background color is white (there are no themes)

StOffscreenGWorld::StOffscreenGWorld(
	const Rect&		inBounds,
	SInt16			inPixelDepth,
	GWorldFlags		inFlags,
	CTabHandle		inCTableH,
	GDHandle		inGDeviceH,
	RGBColor*		inBackColor,
	SInt16			inXferMode)
{
		// We need to save the current GWorld because the image from
		// the offscreen GWorld will be copied into this GWorld
		// in the Destructor.

	::GetGWorld(&mSavePort, &mSaveDevice);

	mBounds		= inBounds;
	mXferMode	= inXferMode;
	mMacGWorld	= nil;

		// NewGWorld interprets the bounds in global coordinates
		// when specifying a zero pixel depth. It uses the maximum
		// depth of all screen devices intersected by the bounds.

	Rect	gwRect = inBounds;
	if (inPixelDepth == 0) {
		::LocalToGlobal(&topLeft(gwRect));
		::LocalToGlobal(&botRight(gwRect));
	}

	QDErr	err = ::NewGWorld(&mMacGWorld, inPixelDepth, &gwRect,
								inCTableH, inGDeviceH, inFlags);

	ThrowIfOSErr_(err);
	ThrowIfNil_(mMacGWorld);

		// Make offscreen GWorld the current one and prepare it
		// for drawing by setting the coordinate system, locking
		// its pixels, setting the background color, and erasing.

	ThemeDrawingState	themeState;
	if (inBackColor == nil) {
		UAppearance::GetThemeDrawingState(&themeState);
	}

	::SetGWorld(mMacGWorld, nil);
	::SetOrigin(inBounds.left, inBounds.top);
	::ClipRect(&inBounds);

	::LockPixels(::GetGWorldPixMap(mMacGWorld));

	if (inBackColor == nil) {
		UAppearance::SetThemeDrawingState(themeState, true);

	} else {
		::RGBBackColor(inBackColor);
	}

	::EraseRect(&inBounds);
}

// ---------------------------------------------------------------------------
//	¥ ~StOffscreenGWorld					Destructor				  [public]
// ---------------------------------------------------------------------------
//	Restores the current Port to what it was before the StOffscreenGWorld
//	was created and copies the offscreen image to that Port

StOffscreenGWorld::~StOffscreenGWorld()
{
		// Restore current GWorld to the one which was current
		// when the Constructor was called

	::SetGWorld(mSavePort, mSaveDevice);

		// Copy image from the offscreen GWorld to the current GWorld,
		// then destroy the offscreen GWorld

	if (mMacGWorld != nil) {
		StColorState	saveColors;
		StColorState::Normalize();	// CopyBits wants this
		::CopyBits((BitMap *) *::GetPortPixMap(mMacGWorld),
					(BitMap *) *::GetPortPixMap(mSavePort),
					&mBounds, &mBounds, mXferMode, nil);
		::UnlockPixels(::GetGWorldPixMap(mMacGWorld));
		::DisposeGWorld(mMacGWorld);
	}
}

#pragma mark -
// ===========================================================================
//	¥ LGWorld Class
// ===========================================================================
//	Use this class to create GWorlds for offscreen drawing, when you want
//	to keep the GWorld for longer than the execution of a single function.
//
//	For example:
//
//	void MyPane::FinishCreateSelf() {
//		FocusDraw();
//		Rect	frame;
//		CalcLocalFrameRect(frame);
//		mGWorld = new LGWorld(frame);
//	}
//
//	MyPane::~MyPane() {
//		delete mGWorld;
//	}
//
//	void MyPane::DrawSelf() {
//		Rect	frame;
//		CalcLocalFrameRect(frame);
//		mGWorld->CopyImage(GetMacPort(), frame);
//	}
//
//	void MyPane::AddRectToImage(Rect &inRect) {
//		mGWorld->BeginDrawing();		// Draw to offscreem GWorld
//		PaintRect(&inRect);
//		mGWorld->EndDrawing();
//
//		RefreshRect(inRect);			// Force update of new drawing
//	}
//
//	With this MyPane class, The AddRectToImage function draws a rectangle to
//	the offscreen image and forces an update of the newly drawn area.
//	The DrawSelf function just copies the image from the offscreen image
//	into the frame of the Pane. All the rectangles painted by calling
//	AddRectToImage are accumulated in the offscreen GWorld, so they will
//	get redrawn properly on any subsequent screen update.


// ---------------------------------------------------------------------------
//	¥ LGWorld								Constructor				  [public]
// ---------------------------------------------------------------------------
//	The inPixelDepth, inFlags, inCTableH, and inGDeviceH parameters
//	all have default values (see the header file), so you don't need
//	to specify them.
//
//	NOTE: If you pass inPixelDepth of zero (or use the default value which
//	is zero), you must set the Focus beforehand to the View whose local
//	coordinates are those in which inBounds is specified. Usually, this is
//	the View in which the LGWorld draws.

LGWorld::LGWorld(
	const Rect&		inBounds,			// In local coords
	SInt16			inPixelDepth,
	GWorldFlags		inFlags,
	CTabHandle		inCTableH,
	GDHandle		inGDeviceH,
	RGBColor*		inBackColor)
{
	StGDeviceSaver	saveGDevice;

	mBounds		= inBounds;
	mMacGWorld	= nil;
	mSavePort	= nil;
	mSaveDevice	= nil;

		// NewGWorld interprets the bounds in global coordinates
		// when specifying a zero pixel depth. It uses the maximum
		// depth of all screen devices intersected by the bounds.

	Rect	gwRect = inBounds;
	if (inPixelDepth == 0) {
		::LocalToGlobal(&topLeft(gwRect));
		::LocalToGlobal(&botRight(gwRect));
	}

	QDErr	err = ::NewGWorld(&mMacGWorld, inPixelDepth, &gwRect,
								inCTableH, inGDeviceH, inFlags);

	ThrowIfOSErr_(err);
	ThrowIfNil_(mMacGWorld);

		// Set up coordinates and erase pixels in GWorld

	::SetGWorld(mMacGWorld, nil);
	::SetOrigin(inBounds.left, inBounds.top);
	::ClipRect(&inBounds);

	if (inBackColor != nil) {
		::RGBBackColor(inBackColor);
	}

	::LockPixels(::GetGWorldPixMap(mMacGWorld));
	::EraseRect(&inBounds);
	::UnlockPixels(::GetGWorldPixMap(mMacGWorld));
}


// ---------------------------------------------------------------------------
//	¥ LGWorld								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct an LGWorld object from an existing GWorld. Object takes
//	ownership of the input GWorld and will dispose it when deleted.

LGWorld::LGWorld(
	GWorldPtr	inGWorld)
{
	mMacGWorld	= inGWorld;
	mBounds		= (**GetGWorldPixMap(mMacGWorld)).bounds;
	mSavePort	= nil;
	mSaveDevice	= nil;
}


// ---------------------------------------------------------------------------
//	¥ ~LGWorld								Destructor				  [public]
// ---------------------------------------------------------------------------

LGWorld::~LGWorld()
{
	if (mMacGWorld != nil) {
		::DisposeGWorld(mMacGWorld);
	}
}


// ---------------------------------------------------------------------------
//	¥ BeginDrawing													  [public]
// ---------------------------------------------------------------------------
//	Sets the current port to the offscreen GWorld and locks its pixels
//
//	Returns false if the offscreen pixels can't be locked. This will
//	happen if the pixPurge flag was set and the pixels were purged
//	from memory. If so, you might want to try to reallocate the pixels
//	by calling Update().
//
//	Every BeginDrawing call must be balanced by a corresponding
//	EndDrawing call.

Boolean
LGWorld::BeginDrawing()
{
	::GetGWorld(&mSavePort, &mSaveDevice);
	::SetGWorld(mMacGWorld, nil);
	return ::LockPixels(::GetGWorldPixMap(mMacGWorld));
}


// ---------------------------------------------------------------------------
//	¥ EndDrawing													  [public]
// ---------------------------------------------------------------------------
//	Unlocks the GWorld's pixels and restores the current port to what
//	it was before BeginDrawing was called.
//
//	Every EndDrawing call must be preceded by a corresponding
//	BeginDrawing call.

void
LGWorld::EndDrawing()
{
	::UnlockPixels(::GetGWorldPixMap(mMacGWorld));
	::SetGWorld(mSavePort, mSaveDevice);
}


// ---------------------------------------------------------------------------
//	¥ CopyImage														  [public]
// ---------------------------------------------------------------------------
//	Copies an image from the offscreen GWorld to the specified Port
//
//	NOTE: You will want to make sure that the foreground color is black
//	and the background color is white, and that inDestPort is the current
//	port before calling this function. CopyBits() can be unreliable if this
//	is not the case. We don't do this automatically here because we want
//	this routine to be fast (that's why you usually use offscreen drawing).

void
LGWorld::CopyImage(
	GrafPtr			inDestPort,
	const Rect&		inDestRect,
	SInt16			inXferMode,
	RgnHandle		inMaskRgn) const
{
	::CopyBits(	(BitMap *) *::GetPortPixMap(mMacGWorld),
				(BitMap *) *::GetPortPixMap((CGrafPtr) inDestPort),
				&mBounds, &inDestRect, inXferMode, inMaskRgn);
}


// ---------------------------------------------------------------------------
//	¥ Update														  [public]
// ---------------------------------------------------------------------------
//	Adjusts the bounds, pixel depth, and/or color table of the LGWorld
//
//	NOTE: If you pass inPixelDepth of zero (or use the default value which
//	is zero), you must set the Focus beforehand to the View whose local
//	coordinates are those in which inBounds is specified. Usually, this is
//	the View in which the LGWorld draws.
//
//	NOTE: You can read about the GWorldFlags and the ::UpdateGWorld()
//	function in IM: Imaging With QuickDraw starting on page 6-23. Here
//	are some quick hints:
//
//		If you are changing the size of the bounds, you probably want
//		to pass clipPix or stretchPix for the GWorldFlags. If making
//		the bounds smaller, clipPix will clip the bottom/right of
//		the image and stretchPix will shrink the image to fit. If making
//		the bounds larger, clipPix will leave the new area undefined
//		(and you should erase or draw into it) and stretchPix will
//		enlarge the image to fit.

GWorldFlags
LGWorld::Update(
	const Rect&		inBounds,			// In local coords
	SInt16			inPixelDepth,
	GWorldFlags		inFlags,
	CTabHandle		inCTableH,
	GDHandle		inGDeviceH)
{
		// UpdateGWorld interprets the bounds in global coordinates
		// when specifying a zero pixel depth. It uses the maximum
		// depth of all screen devices intersected by the bounds.

	Rect	gwRect = inBounds;
	if (inPixelDepth == 0) {
		::LocalToGlobal(&topLeft(gwRect));
		::LocalToGlobal(&botRight(gwRect));
	}

	GWorldFlags	flags = ::UpdateGWorld(&mMacGWorld, inPixelDepth, &gwRect,
										inCTableH, inGDeviceH, inFlags);

		// Inside Mac seems to be inaccurate about what happens
		// when UpdateGWorld fails. It says that the gwFlagErr
		// flag is set and that QDError will indicate the error code.
		// However, testing shows that the low word of flags is the
		// actual error code.

	if ((flags & gwFlagErr) != 0) {		// UpdateGWorld failed
		OSErr	err = ::QDError();
		if (err == noErr) {				// QDError is not set
			err = (OSErr) flags;		// flags is the error code
		}
		Throw_(err);
	}

		// Update succeeded. Now we store the new bounds and
		// set the coordinate system

	mBounds = inBounds;

	StGDeviceSaver	saveGDevice;
	::SetGWorld(mMacGWorld, nil);
	::SetOrigin(inBounds.left, inBounds.top);
	::ClipRect(&inBounds);

	return flags;
}


// ---------------------------------------------------------------------------
//	¥ SetBounds														  [public]
// ---------------------------------------------------------------------------
//	Adjusts the location of the bounds of the GWorld.
//
//	Use to change the GWorld's coordinate system, but don't pass a bounds
//	that is a different size from the existing bounds.

void
LGWorld::SetBounds(
	const Rect&		inBounds)			// In local coords
{
	StGDeviceSaver	saveGDevice;

	mBounds = inBounds;

	::SetGWorld(mMacGWorld, nil);
	::SetOrigin(inBounds.left, inBounds.top);
	::ClipRect(&inBounds);
}


PP_End_Namespace_PowerPlant
