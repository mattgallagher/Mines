// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UATSUI.h					PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UATSUI
#define _H_UATSUI
#pragma once

#include <PP_Prefix.h>
#include <Appearance.h>
#include <ATSUnicode.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ IntToFixed
// ---------------------------------------------------------------------------
//	Returns the Fixed value corresponding to an int
//
//	A Fixed number is a 16-bit signed integer with a 16-bit fraction,
//	packed into a 32-bit value.

#ifndef IntToFixed				// OS Headers may implement this as a #define

inline Fixed	IntToFixed( int inInt )
	{
		return (((SInt32) inInt) << 16);
	}

#endif


// ===========================================================================
//	LATSUIStyle
// ===========================================================================
//	Wrapper class for an ATSUStyle entity

class	LATSUIStyle {
public:
						LATSUIStyle();
						
						LATSUIStyle( const LATSUIStyle& inOriginal );
						
						~LATSUIStyle();
						
	operator			ATSUStyle() const;
						
	ATSUStyle			Get() const;
						
	void				SetOneAttribute(
								ATSUAttributeTag		inTag,
								ByteCount				inDataSize,
								ATSUAttributeValuePtr	inValuePtr);
								
	void				SetMultipleAttributes(
								ItemCount					inNumAttrs,
								const ATSUAttributeTag		inTags[],
								const ByteCount				inValueSizes[],
								const ATSUAttributeValuePtr	inValues[]);
								
	void				ClearOneAttribute( ATSUAttributeTag inTag );
	
	void				ClearAllAttributes();
	
	bool				IsEmpty() const;
	
	void				SetThemeFont(
								ThemeFontID		inThemeFontID,
								ScriptCode		inScript = smSystemScript);
	
	void				SetBoldface( Boolean inBoldface );
	
	void				SetItalic( Boolean inItalic );
	
	void				SetUnderline( Boolean inUnderline );
	
	void				SetCondensed( Boolean inCondensed );
	
	void				SetExtended( Boolean inExtended );
	
	void				SetATSUFont( ATSUFontID inFontID );
	
	void				SetFontSize( Fixed inFontSize );
	
	void				SetColor( const RGBColor& inColor );

protected:
	ATSUStyle			mStyle;
};

// ---------------------------------------------------------------------------
//	Inline functions


inline
LATSUIStyle::operator ATSUStyle() const
{
	return mStyle;
}


inline
ATSUStyle
LATSUIStyle::Get() const
{
	return mStyle;
}


// ===========================================================================
//	LATSUITextLayout
// ===========================================================================
//	Wrapper class for an ATSUTextLayout entity

class	LATSUITextLayout {
public:
						LATSUITextLayout();
						
						LATSUITextLayout( const LATSUITextLayout &inOriginal );
						
						LATSUITextLayout(
								ConstUniCharArrayPtr	inTextPtr,
								UniCharArrayOffset		inTextOffset,
								UniCharCount			inTextLength,
								UniCharCount			inTextTotalLength,
								const LATSUIStyle&		inStyle);
						
						~LATSUITextLayout();
						
	operator			ATSUTextLayout() const;
						
	ATSUTextLayout		Get() const;
								
	void				SetTextPointer(
								ConstUniCharArrayPtr	inTextPtr,
								UniCharArrayOffset		inTextOffset,
								UniCharCount			inTextLength,
								UniCharCount			inTextTotalLength);
								
	void				SetOneLayoutControl(
								ATSUAttributeTag		inTag,
								ByteCount				inDataSize,
								ATSUAttributeValuePtr	inValuePtr);
								
	void				SetMultipleLayoutControls(
								ItemCount					inCount,
								const ATSUAttributeTag		iTag[],
								const ByteCount				iValueSize[],
								const ATSUAttributeValuePtr	iValue[]);
								
	void				ClearOneLayoutControl( ATSUAttributeTag inTag );
	
	void				ClearAllLayoutControls();
	
	void				SetLineWidth( ATSUTextMeasurement inLineWidth );
	
	void				SetRotation( Fixed inRotation );
	
	void				SetFlushFactor( Fixed inFlushFactor );
	
	void				SetTruncation( ATSULineTruncation inTruncation );
	
	void				SetCGContext( CGContextRef inCGContext );
	
	void				SetRunStyle(
								const LATSUIStyle&		inStyle,
								UniCharArrayOffset		inRunStart,
								UniCharCount			inRunLength);
								
	void				DetermineLineBreaks( ATSUTextMeasurement inLineWidth );
	
	void				MeasureLines(
								ATSUTextMeasurement		inLineWidth,
								ItemCount&				outLineCount,
								UniCharArrayOffset*&	outLineBreaks,
								ATSUTextMeasurement*&	outLineHeights);
								
	void				DrawLinesAt(
								ItemCount				inLineCount,
								UniCharArrayOffset*		inLineBreaks,
								ATSUTextMeasurement*	inLineHeights,
								ATSUTextMeasurement		inLeftCoord,
								ATSUTextMeasurement		inTopCoord) const;
								
	void				DrawOneLineAt(
								UniCharArrayOffset		inLineOffset,
								UniCharCount			inLineLength,
								ATSUTextMeasurement		inHoriz,
								ATSUTextMeasurement		inVert) const;
								
	void				DrawInBox( const Rect& inBox );
								
protected:
	ATSUTextLayout		mTextLayout;
};

// ---------------------------------------------------------------------------
//	Inline functions

inline
LATSUITextLayout::operator ATSUTextLayout() const
{
	return mTextLayout;
}


inline
ATSUTextLayout
LATSUITextLayout::Get() const
{
	return mTextLayout;
}


// ===========================================================================
//	LATSUCoordinates
// ===========================================================================

class	LATSUCoordinates {
public:
						LATSUCoordinates(
								ATSUTextLayout			inLayout,
								GrafPtr					inPort);
						
						~LATSUCoordinates();
						
	void				QDToATSU(
								ATSUTextMeasurement		inQDHoriz,
								ATSUTextMeasurement		inQDVert,
								ATSUTextMeasurement&	outATSUHoriz,
								ATSUTextMeasurement&	outATSUVert);

private:
	ATSUTextMeasurement	mHorizOffset;
	ATSUTextMeasurement	mVertOffset;
	bool				mHasCGContext;
};



PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif


#endif
