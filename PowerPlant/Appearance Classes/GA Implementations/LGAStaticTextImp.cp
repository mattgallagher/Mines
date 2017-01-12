// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAStaticTextImp.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGAStaticTextImp.h>

#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UTextTraits.h>
#include <LStream.h>
#include <LString.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LGAStaticTextImp						Constructor				  [public]
// ---------------------------------------------------------------------------

LGAStaticTextImp::LGAStaticTextImp(
	LControlPane*	inControlPane)

	: LGAControlImp (inControlPane)
{
}


// ---------------------------------------------------------------------------
//	¥ LGAStaticTextImp						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGAStaticTextImp::LGAStaticTextImp(
	LStream*	inStream)

	: LGAControlImp (inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LGAStaticTextImp						Destructor				  [public]
// ---------------------------------------------------------------------------

LGAStaticTextImp::~LGAStaticTextImp()
{
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGAStaticTextImp::Init(
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
LGAStaticTextImp::Init(
	LControlPane*	inControlPane,
	SInt16			/* inControlKind */,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			/* inRefCon */)
{
	mControlPane	= inControlPane;
	mTitle			= inTitle;
	mTextTraitsID	= inTextTraitsID;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------

StringPtr
LGAStaticTextImp::GetDescriptor (
	Str255	outDescriptor ) const
{
	return LString::CopyPStr ( mTitle, outDescriptor );
}


// ---------------------------------------------------------------------------
//	¥ CalcTitleRect													  [public]
// ---------------------------------------------------------------------------

void
LGAStaticTextImp::CalcTitleRect	(
	Rect	&outRect )
{
														// Get the local inset frame rectangle
	CalcLocalFrameRect ( outRect );

}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------

void
LGAStaticTextImp::SetDescriptor(
	ConstStringPtr	 inDescriptor)
{
	mTitle = inDescriptor;
	Refresh ();
}


// ---------------------------------------------------------------------------
//	¥ SetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

void
LGAStaticTextImp::SetTextTraitsID (
		ResIDT	 inTextTraitsID )
{
	if ( mTextTraitsID != inTextTraitsID ) {
		mTextTraitsID = inTextTraitsID;
		Refresh ();
	}
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LGAStaticTextImp::SetValue (
		SInt32	inValue )
{
	mTitle = inValue;
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ GetValue														  [public]
// ---------------------------------------------------------------------------

bool
LGAStaticTextImp::GetValue (
		SInt32	&outValue ) const
{
	outValue = mTitle;
	return true;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SupportsCalcBestRect											  [public]
// ---------------------------------------------------------------------------

Boolean
LGAStaticTextImp::SupportsCalcBestRect () const
{
	return true;
}


// ---------------------------------------------------------------------------
//	¥ CalcBestControlRect											  [public]
// ---------------------------------------------------------------------------

void
LGAStaticTextImp::CalcBestControlRect(
	SInt16&		outWidth,
	SInt16&		outHeight,
	SInt16&		outTextBaseline) const
{
	StTextState		theTextState;

	UTextTraits::SetPortTextTraits(GetTextTraitsID());

	FontInfo	fi;
	::GetFontInfo(&fi);

	outWidth		= (SInt16) (::StringWidth(mTitle) + 1);
	outHeight		= (SInt16) (fi.ascent + fi.descent + fi.leading);
	outTextBaseline = fi.ascent;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LGAStaticTextImp::SetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
	switch (inTag) {
	
		case kControlStaticTextTextTag: {
			mTitle.Assign(inDataPtr, (UInt8) inDataSize);
			Refresh();
			break;
		}
		
		default:
			LGAControlImp::SetDataTag(inPartCode, inTag, inDataSize, inDataPtr);
			break;
	}
}



// ---------------------------------------------------------------------------
//	¥ GetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LGAStaticTextImp::GetDataTag (
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size*			outDataSize ) const
{
	switch (inTag) {
	
		case kControlStaticTextTextTag: {
			Size	dataSize = mTitle.LongLength();

			SInt32	bytesToCopy = dataSize;
			if (inBufferSize < dataSize) {
				bytesToCopy = inBufferSize;
			}

			::BlockMoveData(mTitle.ConstTextPtr(), inBuffer, bytesToCopy );
			
			if (outDataSize != nil) {
				*outDataSize = dataSize;
			}
		}
		break;

		case kControlStaticTextTextHeightTag:{
			StTextState			theTextState;
			StColorPenState	thePenState;

														// Get the text traits ID for the control
			ResIDT	textTID = GetTextTraitsID ();

														// Get the port setup with the text traits
			UTextTraits::SetPortTextTraits ( textTID );

														// Figure out the height of the text for the
														// selected font
			FontInfo fi;
			::GetFontInfo ( &fi );
														// The height of the title is the sum of the
														// ascent and descent
			SInt16	textheight = (SInt16) (fi.ascent + fi.descent);
			*(SInt16 *)inBuffer =  textheight;
			
			if (outDataSize != nil) {
				*outDataSize = sizeof ( textheight );;
			}
		}
		break;
		
		default:
			LGAControlImp::GetDataTag(inPartCode, inTag, inBufferSize,
										inBuffer, outDataSize);
			break;
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGAStaticTextImp::DrawSelf ()
{
	Rect	localFrame;
	CalcLocalFrameRect(localFrame);
														// Setup the text traits and get the
														// justification
	SInt16	just = UTextTraits::SetPortTextTraits(mTextTraitsID);

														// Get the text color which was setup by the
														// text traits setup
	RGBColor	textColor;
	::GetForeColor(&textColor);
														// Get the fore and back colors applied
	mControlPane->ApplyForeAndBackColors();

														// If we are drawing to a color screen then
														// we are going to lighten the color of the
														// text when we are disabled
	if ( (mControlPane->GetBitDepth() > 4) && (!IsActive() || !IsEnabled()) ) {
		UTextDrawing::DimTextColor ( textColor );
	}
													// Set the foreground color
	::RGBForeColor(&textColor);
													// Now we can finally get the text drawn
	UTextDrawing::DrawWithJustification(mTitle.TextPtr(), mTitle.LongLength(), localFrame, just);
}


// ---------------------------------------------------------------------------
//	¥ TrackHotSpot													  [public]
// ---------------------------------------------------------------------------

Boolean
LGAStaticTextImp::TrackHotSpot(
	SInt16		/* inHotSpot */,
	Point		/* inPoint */,
	SInt16		/* inModifiers */)
{
	return false;
}


PP_End_Namespace_PowerPlant
