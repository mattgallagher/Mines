// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UGWorld.h					PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UGWorld
#define _H_UGWorld
#pragma once

#include <PP_Prefix.h>
#include <QDOffscreen.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//	¥ StOffscreenGWorld ¥
// ===========================================================================

class	StOffscreenGWorld {
public:
				StOffscreenGWorld(
						const Rect&		inBounds,
						SInt16			inPixelDepth = 0,
						GWorldFlags		inFlags = 0,
						CTabHandle		inCTableH = nil,
						GDHandle		inGDeviceH = nil,
						RGBColor*		inBackColor = nil,
						SInt16			inXferMode = srcCopy);

				~StOffscreenGWorld();

	GWorldPtr	GetMacGWorld() const	{ return mMacGWorld; }

protected:
	GWorldPtr	mMacGWorld;
	Rect		mBounds;
	CGrafPtr	mSavePort;
	GDHandle	mSaveDevice;
	SInt16		mXferMode;
};


// ===========================================================================
//	¥ LGWorld ¥
// ===========================================================================

class	LGWorld {
public:
				LGWorld(
						const Rect&		inBounds,
						SInt16			inPixelDepth = 0,
						GWorldFlags		inFlags = 0,
						CTabHandle		inCTableH = nil,
						GDHandle		inGDeviceH = nil,
						RGBColor*		inBackColor = nil);

				LGWorld( GWorldPtr inGWorld );

				~LGWorld();

	GWorldPtr	GetMacGWorld() const	{ return mMacGWorld; }

	Boolean		BeginDrawing();

	void		EndDrawing();

	void		CopyImage(
						GrafPtr			inDestPort,
						const Rect&		inDestRect,
						SInt16			inXferMode = srcCopy,
						RgnHandle		inMaskRegion = nil) const;

	GWorldFlags	Update(	const Rect&		inBounds,
						SInt16			inPixelDepth = 0,
						GWorldFlags		inFlags = clipPix,
						CTabHandle		inCTableH = nil,
						GDHandle		inGDeviceH = nil);

	void		SetBounds( const Rect& inBounds );

	void		GetBounds( Rect& outBounds ) const
					{
						outBounds = mBounds;
					}

protected:
	GWorldPtr	mMacGWorld;
	Rect		mBounds;
	CGrafPtr	mSavePort;
	GDHandle	mSaveDevice;
};

PP_End_Namespace_PowerPlant

// ---------------------------------------------------------------------------

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
