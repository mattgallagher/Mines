// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UExtractFromAEDesc.h	  	PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Functions for extracting data from AppleEvent Descriptor (AEDesc) records

#ifndef _H_UExtractFromAEDesc
#define _H_UExtractFromAEDesc
#pragma once

#include <PP_Prefix.h>

#include <Aliases.h>
#include <AEDataModel.h>
#include <AERegistry.h>
#include <Quickdraw.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

namespace UExtractFromAEDesc {

	void	TheSInt16	(const AEDesc& inDesc, SInt16& outValue);
	
	void	TheSInt32	(const AEDesc& inDesc, SInt32& outValue);
	
	void	TheUInt32	(const AEDesc& inDesc, UInt32& outValue);
	
	void	TheSInt64	(const AEDesc& inDesc, SInt64& outValue);
	
	void	ThePoint	(const AEDesc& inDesc, Point& outValue);
	
	void	TheRect		(const AEDesc& inDesc, Rect& outValue);
	
	void	TheBoolean	(const AEDesc& inDesc, Boolean& outValue);
	
	void	TheType		(const AEDesc& inDesc, OSType& outValue);
	
	void	TheEnum		(const AEDesc& inDesc, OSType& outValue);
	
	void	TheRGBColor	(const AEDesc& inDesc, RGBColor& outValue);
	
	void	TheFSSpec	(const AEDesc& inDesc, FSSpec& outFSSpec);
	
	void	ThePString	(const AEDesc&	inDesc,
						 StringPtr		outString,
						 SInt32			inMaxSize = 256);
	
	void	TheAlias	(const AEDesc& inDesc, AliasHandle& outAliasHandle);
	
	void	TheData		(const AEDesc&	inDesc,
						 DescType		inDescType,
						 void*			inDataPtr,
						 Size			inMaxSize );
						 
	OSErr	AEGetDescData(
						 const AEDesc&	inDesc,
						 void*			inDataPtr,
						 Size			inMaxSize);
					
	Size	AEGetDescDataSize( const AEDesc& inDesc );
}


// ===========================================================================
//	Inline function definitions

// ---------------------------------------------------------------------------
//	¥ TheSInt16
// ---------------------------------------------------------------------------

inline void
UExtractFromAEDesc::TheSInt16(
	const AEDesc&	inDesc,
	SInt16&			outValue)
{
	TheData(inDesc, typeSInt16, &outValue, sizeof(SInt16));
}


// ---------------------------------------------------------------------------
//	¥ TheSInt32
// ---------------------------------------------------------------------------

inline void
UExtractFromAEDesc::TheSInt32(
	const AEDesc&	inDesc,
	SInt32&			outValue)
{
	TheData(inDesc, typeSInt32, &outValue, sizeof(SInt32));
}


// ---------------------------------------------------------------------------
//	¥ TheUInt32
// ---------------------------------------------------------------------------

inline void
UExtractFromAEDesc::TheUInt32(
	const AEDesc&	inDesc,
	UInt32&			outValue)
{
	TheData(inDesc, typeUInt32, &outValue, sizeof(UInt32));
}


// ---------------------------------------------------------------------------
//	¥ TheSInt64
// ---------------------------------------------------------------------------

inline void
UExtractFromAEDesc::TheSInt64(
	const AEDesc&	inDesc,
	SInt64&			outValue)
{
	TheData(inDesc, typeSInt64, &outValue, sizeof(SInt64));
}


// ---------------------------------------------------------------------------
//	¥ ThePoint
// ---------------------------------------------------------------------------

inline void
UExtractFromAEDesc::ThePoint(
	const AEDesc&	inDesc,
	Point&			outValue)
{
	TheData(inDesc, typeQDPoint, &outValue, sizeof(Point));
}


// ---------------------------------------------------------------------------
//	¥ TheRect
// ---------------------------------------------------------------------------

inline void
UExtractFromAEDesc::TheRect(
	const AEDesc&	inDesc,
	Rect&			outValue)
{
	TheData(inDesc, typeQDRectangle, &outValue, sizeof(Rect));
}


// ---------------------------------------------------------------------------
//	¥ TheBoolean
// ---------------------------------------------------------------------------

inline void
UExtractFromAEDesc::TheBoolean(
	const AEDesc&	inDesc,
	Boolean&		outValue)
{
	TheData(inDesc, typeBoolean, &outValue, sizeof(Boolean));
}


// ---------------------------------------------------------------------------
//	¥ TheType
// ---------------------------------------------------------------------------

inline void
UExtractFromAEDesc::TheType(
	const AEDesc&	inDesc,
	OSType&			outValue)
{
	TheData(inDesc, typeType, &outValue, sizeof(OSType));
}


// ---------------------------------------------------------------------------
//	¥ TheEnum
// ---------------------------------------------------------------------------

inline void
UExtractFromAEDesc::TheEnum(
	const AEDesc&	inDesc,
	OSType&			outValue)
{
	TheData(inDesc, typeEnumeration, &outValue, sizeof(DescType));
}


// ---------------------------------------------------------------------------
//	¥ TheRGBColor
// ---------------------------------------------------------------------------

inline void
UExtractFromAEDesc::TheRGBColor(
	const AEDesc&	inDesc,
	RGBColor&		outValue)
{
	TheData(inDesc, typeRGBColor, &outValue, sizeof(RGBColor));
}


// ---------------------------------------------------------------------------
//	¥ TheFSSpec
// ---------------------------------------------------------------------------

inline void
UExtractFromAEDesc::TheFSSpec(
	const AEDesc&	inDesc,
	FSSpec&			outValue)
{
	TheData(inDesc, typeFSS, &outValue, sizeof(FSSpec));
}


PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
