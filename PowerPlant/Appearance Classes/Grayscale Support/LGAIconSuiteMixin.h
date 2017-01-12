// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAIconSuiteMixin.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGAIconSuiteMixin
#define _H_LGAIconSuiteMixin
#pragma once

#include <PP_Prefix.h>
#include <Icons.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


//	=== CONSTANTS ===

// Used by the LGAIconSuiteMixin class and any of the classes it is mixed into, it
// defines a number of constants for the various positions an icon can be placed
// within its pane

enum EIconPosition {
	iconPosition_None,
	iconPosition_TopLeft,
	iconPosition_TopCenter,
	iconPosition_TopRight,
	iconPosition_RightCenter,
	iconPosition_BottomRight,
	iconPosition_BottomCenter,
	iconPosition_BottomLeft,
	iconPosition_LeftCenter,
	iconPosition_Center
};

// Constants for the various size supported by the icon suite class

enum ESizeSelector {
	sizeSelector_None,
	sizeSelector_LargeIconSize = 32,
	sizeSelector_SmallIconSize = 16,
	sizeSelector_MiniIconSize  = 12
};


//	=== TYPES ===

typedef struct SIconSuiteInfo {

	ResIDT			iconSuiteID;
	SInt16			transform;
	ESizeSelector	sizeSelector;
	EIconPosition	iconPlacement;
	SInt16			edgeOffset;
	Boolean			hiliteIcon;
	Boolean			offsetIconOnHilite;
	Boolean			clickInIcon;

} SIconSuiteInfo;


//====================================================================================
//	Class: LGAIconSuiteMixin
//====================================================================================

class LGAIconSuiteMixin {
public:
						LGAIconSuiteMixin();
						
						LGAIconSuiteMixin( const LGAIconSuiteMixin& inOriginal );

	virtual				~LGAIconSuiteMixin();


						// Getters

	Handle				GetIconSuiteH() const		{ return mIconSuiteH; }

	ResIDT				GetIconResourceID() const	{ return mIconSuiteID; }

	SInt16				GetIconTransform() const	{ return mTransform; }

	ESizeSelector		GetIconSize() const			{ return mSizeSelector; }

	EIconPosition		GetIconPosition() const		{ return mIconPlacement; }

	SInt16				GetEdgeOffset() const		{ return mEdgeOffset; }

	bool				GetHiliteIcon() const		{ return mHiliteIcon; }

	bool				GetOffsetIconOnHilite() const	{ return mOffsetIconOnHilite; }

	bool				GetClickInIcon() const		{ return mClickInIcon; }

	bool				DisposesIconSuiteH() const	{ return mDisposeIconSuiteH; }

	virtual	void		LoadIconSuiteHandle();

	virtual	void		CalcLocalIconRect( Rect& outRect );


						// Setters

	virtual	void		SetIconSuiteH (
								Handle	inIconSuiteH,
								bool	inDisposesHandle = false );

	virtual	void		SetIconResourceID( ResIDT inResID );

	virtual	void		SetIconTransform( SInt16 inTransform );

	virtual	void		SetIconSize( ESizeSelector inSizeSelector );

	virtual	void		SetIconPosition( EIconPosition inPosition );

	virtual	void		SetEdgeOffset( SInt16 inOffset );

	virtual	void		SetHiliteIcon( bool inHiliteIcon )
							{
								mHiliteIcon = inHiliteIcon;
							}

	virtual	void		SetOffsetIconOnHilite( bool inOffsetIcon )
							{
								mOffsetIconOnHilite = inOffsetIcon;
							}

	virtual	void		SetClickInIcon( bool inClickInIcon )
							{
								mClickInIcon = inClickInIcon;
							}


protected:

	Handle			mIconSuiteH;			//	Handle to the actual icon suite
	ResIDT			mIconSuiteID;			//	Resource ID of iconsuite we will be drawing
	SInt16			mTransform;				//	Transform to be applied to icon
	SInt16			mLastTransform;			//	This is used when enabling and deactivating
	ESizeSelector	mSizeSelector;			//	What size should the icon be drawn
	EIconPosition	mIconPlacement;			//  Where in the pane will the icon be drawn
	Point			mIconLocation;			//	Location of the icon within the pane
	SInt16			mEdgeOffset;			//	Amount icon is inset from edge
	bool			mHiliteIcon;			//	Does icon hilite when button is pressed
	bool			mOffsetIconOnHilite;	//	Does icon move down and to right when hilited
	bool			mClickInIcon;			//	This flag is used to tell if hit testing
											//		should only be handled in the actual mask
											//		of the icon suite, instead of the entire
											//		pane frame.
	bool			mDisposeIconSuiteH;		//	Specifies if we are responsible for disposing
											//		of the cached handle


	virtual void		CalcIconLocation(
								Point&		outIconLocation);


};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
